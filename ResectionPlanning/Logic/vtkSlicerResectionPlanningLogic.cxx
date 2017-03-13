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

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtksys/RegularExpression.hxx>
#include <vtkAppendPolyData.h>

// STD includes
#include <cassert>
#include <iostream>
#include <map>
#include <list>
#include <QString>
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
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New());
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New());

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

    // get name of node
    QString Qname = modelNode->GetName();
    QString Qid = modelNode->GetID();

    //Inform about the tumor model added.
    std::pair<QString, QString> pair;
    pair.first = Qid;
    pair.second = Qname;
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorModelAdded,
                      &pair);
    this->tumorList.push_back(modelNode);

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

    this->resectionList.push_back(resectionNode);

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

    // get name of node
    QString Qname = modelNode->GetName();
    QString Qid = modelNode->GetID();

    //Inform about the tumor model removed.
    std::pair<QString, QString> pair;
    pair.first = Qid;
    pair.second = Qname;
    this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorModelRemoved,
                      &pair);
    this->tumorList.remove(modelNode);

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

    // Inform that a hepatic node was removed
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

    this->resectionList.remove(resectionNode);

    return;
    }
}


void vtkSlicerResectionPlanningLogic
::SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::cout << "'Resection: " << rsNodeName << ", associated to tumor: " << tumorNodeName << '\n';
  this->resectionToTumorMap.insert(std::pair<std::string, std::string>(rsNodeName, tumorNodeName));
}

void vtkSlicerResectionPlanningLogic
::RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::multimap<std::string, std::string>::iterator it;

  for (it=resectionToTumorMap.begin(); it!=resectionToTumorMap.end(); ++it) {
    if(((*it).first == rsNodeName) && ((*it).second == tumorNodeName))
    {
      std::cout << "'Resection: " << (*it).first << ", removed association to tumor: " << (*it).second << '\n';
      this->resectionToTumorMap.erase(it);
    }
  }
}
