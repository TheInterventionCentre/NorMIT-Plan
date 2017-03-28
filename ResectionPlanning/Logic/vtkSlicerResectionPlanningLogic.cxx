/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkSlicerResectionPlanningLogic.cxx

  Copyright (c) 2017, The Intervention Centre, Oslo University Hospital

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  3. Neither the name of the copyright holder nor the names of its contributors
  may be used to endorse or promote products derived from this software
  without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  =========================================================================*/

// ResectionPlanning Logic includes
#include "vtkSlicerResectionPlanningLogic.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"
#include "vtkMRMLResectionSurfaceStorageNode.h"
#include "vtkMRMLResectionInitializationNode.h"
#include "vtkMRMLResectionInitializationDisplayNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtksys/RegularExpression.hxx>
#include <vtkAppendPolyData.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkCutter.h>
#include <vtkDoubleArray.h>
#include <vtkCenterOfMass.h>
#include <vtkTable.h>
#include <vtkPCAStatistics.h>
#include <vtkCacheManager.h>

/// ITK includes
#include <itksys/Directory.hxx>
#include <itksys/SystemTools.hxx>

// STD includes
#include <cassert>
#include <string>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerResectionPlanningLogic);

//----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic::vtkSlicerResectionPlanningLogic()
{
  this->AppendTumors = vtkSmartPointer<vtkAppendPolyData>::New();
  this->AppendTumors->GlobalWarningDisplayOff();
}

//----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic::~vtkSlicerResectionPlanningLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  // Hook up the scene observation events
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());

  // Create the joint tumor models  display node and add it to the scene
  vtkSmartPointer<vtkMRMLModelDisplayNode> jointTumorsModelDisplayNode =
    vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
  jointTumorsModelDisplayNode->HideFromEditorsOn();
  jointTumorsModelDisplayNode->VisibilityOff();
  jointTumorsModelDisplayNode->SetName("LRPJointTumorsModelDisplay");
  jointTumorsModelDisplayNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(jointTumorsModelDisplayNode);

  //Create the joint tumor models and add it to the scene
  vtkSmartPointer<vtkMRMLModelNode> jointTumorsModelNode =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  jointTumorsModelNode->HideFromEditorsOn();
  jointTumorsModelNode->SetName("LRPJointTumorsModel");
  jointTumorsModelNode->SetScene(this->GetMRMLScene());
  jointTumorsModelNode->
    SetAndObserveDisplayNodeID(jointTumorsModelDisplayNode->GetID());
  this->GetMRMLScene()->AddNode(jointTumorsModelNode);

  // Create the append poly data filter and associate it to the modelnode
  jointTumorsModelNode->SetPolyDataConnection(this->AppendTumors->GetOutputPort());
}

//-----------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionSurface =
    vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
  scene->RegisterNodeClass(resectionSurface);

  vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
  scene->RegisterNodeClass(resectionDisplayNode);

  vtkSmartPointer<vtkMRMLResectionInitializationNode> initializationNode =
    vtkSmartPointer<vtkMRMLResectionInitializationNode>::New();
  scene->RegisterNodeClass(initializationNode);

  vtkSmartPointer<vtkMRMLResectionInitializationDisplayNode>
    initializationDisplayNode =
    vtkSmartPointer<vtkMRMLResectionInitializationDisplayNode>::New();
  scene->RegisterNodeClass(initializationDisplayNode);
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* addedNode)
{
  // Check whether the added node was a model
  vtkMRMLModelNode *modelNode = vtkMRMLModelNode::SafeDownCast(addedNode);
  if (!modelNode)
    {
    return;
    }

  vtksys::RegularExpression tumorModelEx("^LRPTumor[0-9]+Model$");
  vtksys::RegularExpression parenchymaModelEx("^LRPParenchymaModel$");
  vtksys::RegularExpression portalModelEx("^LRPPortalModel$");
  vtksys::RegularExpression hepaticModelEx("^LRPHepaticModel$");

  std::pair<std::string, std::string> id_name;
  id_name.first = std::string(modelNode->GetID());
  id_name.second = std::string(modelNode->GetName());

  // If the model node is a tumor
  if (tumorModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Tumor model node added "
                  << modelNode->GetName());

    // Add the tumor to the append filter.
    this->AppendTumors->AddInputData(modelNode->GetPolyData());

    // Set scalar visibility of tumor
    vtkMRMLModelDisplayNode *tumorModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (tumorModelDisplayNode)
      {
      tumorModelDisplayNode->ScalarVisibilityOn();
      }

    //Inform about the tumor model added.
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorModelAdded,
                      static_cast<void*>(&id_name));
    return;
    }

  //Check whether the added model was a parenchyma node.
  if (parenchymaModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Parenchyma model node added "
                  << modelNode->GetName());

    // Set scalar visibility of parenchyma.
    vtkMRMLModelDisplayNode *parenchymaModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (parenchymaModelDisplayNode)
      {
      parenchymaModelDisplayNode->ScalarVisibilityOn();
      }

    this->ParenchymaModelNode = modelNode;

    // Inform that a hepatic node was added
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ParenchymaModelAdded,
                      static_cast<void*>(&id_name));

    return;
    }

  // Check whether the added model was a hepatic node.
  if (hepaticModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Hepatic model node added "
                  << modelNode->GetName());

    //Set scalar visibility of hepatic model.
    vtkMRMLModelDisplayNode *hepaticModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (hepaticModelDisplayNode)
      {
      hepaticModelDisplayNode->ScalarVisibilityOn();
      }

    // Inform that a hepatic node was added
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::HepaticModelAdded,
                      static_cast<void*>(&id_name));
    return;
    }

  //Check wether the added model was a portal node
  if (portalModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Portal model added "
                  << modelNode->GetName());

    //Set scalar visibility of the portal model.
    vtkMRMLModelDisplayNode *portalModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (portalModelDisplayNode)
      {
      portalModelDisplayNode->ScalarVisibilityOn();
      }

    // Inform that a hepatic node was added
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::PortalModelAdded,
                      static_cast<void*>(&id_name));
    return;
    }

  // Check whether the added node was a resection node
  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(addedNode);
  if (resectionNode)
    {
    vtkDebugMacro("Resection planning logic: Resection node added "
                  << resectionNode->GetName());
    // Inform that a resedtion node was added
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                      static_cast<void*>(&id_name));
    return;
    }
}



//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode *removedNode)
{
  // Check whether the added node was a model
  vtkMRMLModelNode *modelNode = vtkMRMLModelNode::SafeDownCast(removedNode);
  if (!modelNode)
    {
    return;
    }

  vtksys::RegularExpression tumorModelEx("^LRPTumor[0-9]+Model$");
  vtksys::RegularExpression parenchymaModelEx("^LRPParenchymaModel$");
  vtksys::RegularExpression portalModelEx("^LRPPortalModel$");
  vtksys::RegularExpression hepaticModelEx("^LRPHepaticModel$");

  std::pair<std::string, std::string> id_name;
  id_name.first = std::string(modelNode->GetID());
  id_name.second = std::string(modelNode->GetName());

  // If the model node is a tumor
  if (tumorModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Tumor model node removed "
                  << modelNode->GetName());

    // Add the tumor to the append filter.
    this->AppendTumors->AddInputData(modelNode->GetPolyData());

    // Set scalar visibility of tumor
    vtkMRMLModelDisplayNode *tumorModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (tumorModelDisplayNode)
      {
      tumorModelDisplayNode->ScalarVisibilityOn();
      }

    //Inform about the tumor model removed.
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorModelRemoved,
                      static_cast<void*>(&id_name));
    return;
    }

  //Check whether the removed model was a parenchyma node.
  if (parenchymaModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Parenchyma model node removed "
                  << modelNode->GetName());

    // Set scalar visibility of parenchyma.
    vtkMRMLModelDisplayNode *parenchymaModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (parenchymaModelDisplayNode)
      {
      parenchymaModelDisplayNode->ScalarVisibilityOn();
      }

    // Inform that a parenchuma node was removed
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ParenchymaModelRemoved,
                      static_cast<void*>(&id_name));
    return;
    }

  // Check whether the removed model was a hepatic node.
  if (hepaticModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Hepatic model node removed "
                  << modelNode->GetName());

    //Set scalar visibility of hepatic model.
    vtkMRMLModelDisplayNode *hepaticModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (hepaticModelDisplayNode)
      {
      hepaticModelDisplayNode->ScalarVisibilityOn();
      }

    // Inform that a hepatic node was removed
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::HepaticModelRemoved,
                      static_cast<void*>(&id_name));
    return;
    }

  //Check wether the removed model was a portal node
  if (portalModelEx.find(modelNode->GetName()))
    {
    vtkDebugMacro("Resection planning logic: Portal model removed "
                  << modelNode->GetName());

    //Set scalar visibility of the portal model.
    vtkMRMLModelDisplayNode *portalModelDisplayNode =
      vtkMRMLModelDisplayNode::SafeDownCast(modelNode->GetDisplayNode());
    if (portalModelDisplayNode)
      {
      portalModelDisplayNode->ScalarVisibilityOn();
      }

    // Inform that a hepatic node was removed
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::PortalModelRemoved,
                      static_cast<void*>(&id_name));
    return;
    }

  // Check whether the removed node was a resection node
  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(removedNode);
  if (resectionNode)
    {
    vtkDebugMacro("Resection planning logic: Resection node removed "
                  << resectionNode->GetName());
    // Inform that a resedtion node was removed
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeRemoved,
                      static_cast<void*>(&id_name));
    return;
    }
}

//-------------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::AddResectionSurface()
{
  assert(this->GetMRMLScene() != 0);

  if (!this->ParenchymaModelNode)
    {
    vtkErrorMacro("LRPParenchymaModel node needed to add a resection");
    return;
    }

  vtkMRMLScene *scene = this->GetMRMLScene();

  // Add a resection initialization node
  vtkSmartPointer<vtkMRMLResectionInitializationDisplayNode>
    resectionInitializationDisplayNode =
    vtkSmartPointer<vtkMRMLResectionInitializationDisplayNode>::New();
  scene->AddNode(resectionInitializationDisplayNode);

  vtkSmartPointer<vtkMRMLResectionInitializationNode> resectionInitializationNode =
    vtkSmartPointer<vtkMRMLResectionInitializationNode>::New();
  resectionInitializationNode->SetTargetParenchyma(this->ParenchymaModelNode);
  resectionInitializationNode->HideFromEditorsOn();
  resectionInitializationNode->SaveWithSceneOff();
  resectionInitializationNode->SetAndObserveDisplayNodeID(
    resectionInitializationDisplayNode->GetID());
  scene->AddNode(resectionInitializationNode);

  // Add display node first
  vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
  resectionDisplayNode->SetScene(this->GetMRMLScene());
  resectionDisplayNode->ScalarVisibilityOn();
  scene->AddNode(resectionDisplayNode);

  // Then add resection node
  vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
    vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
  resectionNode->SetScene(this->GetMRMLScene());
  resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());
  scene->AddNode(resectionNode);

  // Link the resection initialization node with the resection
  ResectionInitializationMap[resectionInitializationNode] = resectionNode;

  // Observe the interaction events
  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::StartInteractionEvent);
  nodeEvents->InsertNextValue(vtkCommand::EndInteractionEvent);
  vtkUnObserveMRMLNodeMacro(resectionNode);
  vtkUnObserveMRMLNodeMacro(resectionInitializationNode);
  vtkObserveMRMLNodeEventsMacro(resectionNode, nodeEvents.GetPointer());
  vtkObserveMRMLNodeEventsMacro(resectionInitializationNode,
                                nodeEvents.GetPointer());

  // Update the bezier surface
  this->UpdateBezierWidgetOnInitialization(resectionInitializationNode);

  // Inform that resection was added
  std::pair<std::string, std::string> id_name;
  id_name.first = std::string(resectionNode->GetID());
  id_name.second = std::string(resectionNode->GetName());
  this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                    static_cast<void*>(&id_name));
}

//----------------------------------------------------------------------------
vtkMRMLResectionSurfaceNode* vtkSlicerResectionPlanningLogic::AddResectionSurface(const char* filename)
{
  std::cout << "Logic - AddResectionSurface" << std::endl;

  if (this->GetMRMLScene() == 0 || filename == 0)
  {
    return 0;
  }
  vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode = vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
  vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode = vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
  vtkSmartPointer<vtkMRMLResectionSurfaceStorageNode> resectionStorageNode = vtkSmartPointer<vtkMRMLResectionSurfaceStorageNode>::New();
  vtkSmartPointer<vtkMRMLStorageNode> storageNode;

  // check for local or remote files
  int useURI = 0; // false;
  if (this->GetMRMLScene()->GetCacheManager() != NULL)
  {
    useURI = this->GetMRMLScene()->GetCacheManager()->IsRemoteReference(filename);
    vtkDebugMacro("AddResectionSurface: file name is remote: " << filename);
  }
  const char *localFile=0;
  if (useURI)
  {
    resectionStorageNode->SetURI(filename);
    // reset filename to the local file name
    localFile = ((this->GetMRMLScene())->GetCacheManager())->GetFilenameFromURI(filename);
  }
  else
  {
    resectionStorageNode->SetFileName(filename);
    localFile = filename;
  }
  const std::string fname(localFile?localFile:"");
  // the model name is based on the file name (itksys call should work even if
  // file is not on disk yet)
  std::string name = itksys::SystemTools::GetFilenameName(fname);
  vtkDebugMacro("AddResectionSurface: got resection name = " << name.c_str());

  // check to see which node can read this type of file
  if (resectionStorageNode->SupportedFileType(name.c_str()))
  {
    storageNode = resectionStorageNode.GetPointer();
  }

  if (storageNode != NULL)
    {
    std::string baseName = itksys::SystemTools::GetFilenameWithoutExtension(fname);
    std::string uname( this->GetMRMLScene()->GetUniqueNameByString(baseName.c_str()));
    resectionNode->SetName(uname.c_str());

    this->GetMRMLScene()->AddNode(resectionStorageNode.GetPointer());
    this->GetMRMLScene()->AddNode(resectionDisplayNode.GetPointer());

    // Set the scene so that SetAndObserve[Display|Storage]NodeID can find the
    // node in the scene (so that DisplayNodes return something not empty)
    //resectionNode->SetScene(this->GetMRMLScene());
    resectionNode->SetAndObserveStorageNodeID(resectionStorageNode->GetID());
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());

    this->GetMRMLScene()->AddNode(resectionNode.GetPointer());

    // now set up the reading
    vtkDebugMacro("AddModel: calling read on the storage node");
    int retval = storageNode->ReadData(resectionNode.GetPointer());
    if (retval != 1)
      {
      vtkErrorMacro("AddModel: error reading " << filename);
      this->GetMRMLScene()->RemoveNode(resectionNode.GetPointer());
      return 0;
      }
    }
  else
    {
    vtkErrorMacro("Couldn't read file: " << filename);
    return 0;
    }

  /*
  // Inform that resection was added
  std::pair<std::string, std::string> id_name;
  id_name.first = std::string(resectionNode->GetID());
  id_name.second = std::string(resectionNode->GetName());
  this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                    static_cast<void*>(&id_name));
  */

  return resectionNode.GetPointer();
}

//------------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::
ProcessMRMLNodesEvents(vtkObject *object,
                       unsigned long int eventId,
                       void *vtkNotUsed(data))
{
  vtkMRMLResectionInitializationNode *initializationNode =
    vtkMRMLResectionInitializationNode::SafeDownCast(object);

  if (initializationNode)
    {
    switch(eventId)
      {
      case vtkCommand::EndInteractionEvent:
        this->UpdateBezierWidgetOnInitialization(initializationNode);
        break;

      case vtkCommand::StartInteractionEvent:
        this->HideResectionSurfaceOnInitialization(initializationNode);
        break;
      }
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(object);

  if (resectionNode)
    {
    if (eventId ==  vtkCommand::StartInteractionEvent)
      {
        this->HideInitializationOnResectionModification(resectionNode);
      }
    }

  return;
}

//------------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::
UpdateBezierWidgetOnInitialization(vtkMRMLResectionInitializationNode *initNode)
{
  if (!initNode)
    {
    vtkErrorMacro("Error: no initialization node passed");
    return;
    }

  // Check for parenchyma node
  if (!this->ParenchymaModelNode)
    {
    vtkErrorMacro("Error: no parenchyma model node is selected.");
    return;
    }

  // Check for parenchyma polydata
  if (!this->ParenchymaModelNode->GetPolyData())
    {
    vtkErrorMacro("Error: parenchyma model node does not contain poly data.");
    return;
    }

  // Check for association between initalization node and resection node
  ResectionInitializationIt it =
    this->ResectionInitializationMap.find(initNode);
  if (it == this->ResectionInitializationMap.end())
    {
    vtkErrorMacro("Error: initialization node is not "
                  << "associated with any resection node");
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode = it->second;

  double point1[3];
  double point2[3];
  double midPoint[3];
  double normal[3];

  initNode->GetPoint1(point1);
  initNode->GetPoint2(point2);

  midPoint[0] = (point1[0] + point2[0]) / 2.0;
  midPoint[1] = (point1[1] + point2[1]) / 2.0;
  midPoint[2] = (point1[2] + point2[2]) / 2.0;

  normal[0] = point2[0] - point1[0];
  normal[1] = point2[1] - point1[1];
  normal[2] = point2[2] - point1[2];

  // Cut the parenchyma (generate contour).
  vtkNew<vtkPlane> cuttingPlane;
  cuttingPlane->SetOrigin(midPoint);
  cuttingPlane->SetNormal(normal);
  vtkNew<vtkCutter> cutter;
  cutter->SetInputData(this->ParenchymaModelNode->GetPolyData());
  cutter->SetCutFunction(cuttingPlane.GetPointer());
  cutter->Update();

  vtkPolyData *contour = cutter->GetOutput();

  // Perform Principal Component Analysis
  vtkNew<vtkDoubleArray> xArray;
  xArray->SetNumberOfComponents(1);
  xArray->SetName("x");
  vtkNew<vtkDoubleArray> yArray;
  yArray->SetNumberOfComponents(1);
  yArray->SetName("y");
  vtkNew<vtkDoubleArray> zArray;
  zArray->SetNumberOfComponents(1);
  zArray->SetName("z");

  vtkNew<vtkCenterOfMass> centerOfMass;
  centerOfMass->SetInputData(contour);
  centerOfMass->Update();
  double com[3]={0};
  centerOfMass->GetCenter(com);

  for(unsigned int i=0; i<contour->GetNumberOfPoints(); i++)
    {
    double point[3];
    contour->GetPoint(i, point);
    xArray->InsertNextValue(point[0]);
    yArray->InsertNextValue(point[1]);
    zArray->InsertNextValue(point[2]);
    }

  vtkNew<vtkTable> dataTable;
  dataTable->AddColumn(xArray.GetPointer());
  dataTable->AddColumn(yArray.GetPointer());
  dataTable->AddColumn(zArray.GetPointer());

  vtkNew<vtkPCAStatistics> pcaStatistics;
  pcaStatistics->SetInputData(vtkStatisticsAlgorithm::INPUT_DATA,
                              dataTable.GetPointer());
  pcaStatistics->SetColumnStatus("x", 1);
  pcaStatistics->SetColumnStatus("y", 1);
  pcaStatistics->SetColumnStatus("z", 1);
  pcaStatistics->RequestSelectedColumns();
  pcaStatistics->SetDeriveOption(true);
  pcaStatistics->SetFixedBasisSize(3);
  pcaStatistics->Update();

  vtkNew<vtkDoubleArray> eigenvalues;
  pcaStatistics->GetEigenvalues(eigenvalues.GetPointer());
  vtkNew<vtkDoubleArray> eigenvector1;
  pcaStatistics->GetEigenvector(0, eigenvector1.GetPointer());
  vtkNew<vtkDoubleArray> eigenvector2;
  pcaStatistics->GetEigenvector(1, eigenvector2.GetPointer());
  vtkNew<vtkDoubleArray> eigenvector3;
  pcaStatistics->GetEigenvector(2, eigenvector3.GetPointer());

  double length1 = 4.0*sqrt(pcaStatistics->GetEigenvalue(0,0));
  double length2 = 4.0*sqrt(pcaStatistics->GetEigenvalue(0,1));

  double v1[3] =
    {
      eigenvector1->GetValue(0),
      eigenvector1->GetValue(1),
      eigenvector1->GetValue(2)
    };

  double v2[3] =
    {
      eigenvector2->GetValue(0),
      eigenvector2->GetValue(1),
      eigenvector2->GetValue(2)
    };

  double origin[3] =
    {
      com[0] - v1[0]*length1/2.0 - v2[0]*length2/2.0,
      com[1] - v1[1]*length1/2.0 - v2[1]*length2/2.0,
      com[2] - v1[2]*length1/2.0 - v2[2]*length2/2.0,
    };

  point1[0] = origin[0] + v1[0]*length1;
  point1[1] = origin[1] + v1[1]*length1;
  point1[2] = origin[2] + v1[2]*length1;

  point2[0] = origin[0] + v2[0]*length2;
  point2[1] = origin[1] + v2[1]*length2;
  point2[2] = origin[2] + v2[2]*length2;

  //Create bezier surface according to initial plane
  vtkNew<vtkPlaneSource> planeSource;
  planeSource->SetOrigin(origin);
  planeSource->SetPoint1(point1);
  planeSource->SetPoint2(point2);
  planeSource->SetXResolution(3);
  planeSource->SetYResolution(3);
  planeSource->Update();

  vtkMRMLResectionSurfaceDisplayNode *displayNode =
    vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(
      resectionNode->GetDisplayNode());

  if (!displayNode)
    {
    vtkErrorMacro("Error: no display node associated with the resectio node");
    return;
    }

  displayNode->VisibilityOn();
  resectionNode->SetControlPoints(planeSource->GetOutput()->GetPoints());
}

//------------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::
HideResectionSurfaceOnInitialization(vtkMRMLResectionInitializationNode* initNode)
{
  if (!initNode)
    {
    vtkErrorMacro("Error: no initialization node passed");
    return;
    }

  // Check for association between initalization node and resection node
  ResectionInitializationIt it =
    this->ResectionInitializationMap.find(initNode);
  if (it == this->ResectionInitializationMap.end())
    {
    vtkErrorMacro("Error: initialization node is not "
                  << "associated with any resection node");
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode = it->second;
  vtkMRMLResectionSurfaceDisplayNode *displayNode =
    vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(
      resectionNode->GetDisplayNode());

  if (!displayNode)
    {
    vtkErrorMacro("Error: no display node associated with the resection node.");
    return;
    }

  displayNode->VisibilityOff();
}

//------------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::
HideInitializationOnResectionModification(vtkMRMLResectionSurfaceNode* node)
{
  if (!node)
    {
    vtkErrorMacro("Error: no initialization node passed.");
    return;
    }


  // Find the initializatio node
  ResectionInitializationIt it = ResectionInitializationMap.begin();
  for(it; it!=ResectionInitializationMap.end(); it++)
    {
    if (it->second == node)
      {
      break;
      }
    }

  if (it == ResectionInitializationMap.end())
    {
    vtkErrorMacro("Error: Resection surface node does not have the "
                  << "corresponding resection initialization node.");
    return;
    }

  vtkMRMLResectionInitializationNode *initializationNode  = it->first;

  vtkMRMLResectionInitializationDisplayNode *displayNode =
    vtkMRMLResectionInitializationDisplayNode::SafeDownCast(
      initializationNode->GetDisplayNode());

  if (!displayNode)
    {
    vtkErrorMacro("Error: no display node associated "
                  << "with the initialization node.");
    return;
    }

  displayNode->VisibilityOff();
}
