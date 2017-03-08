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

  vtksys::RegularExpression tumorModelEx("^LRPTumor[0-9]*Model$");
  vtksys::RegularExpression parenchymaModelEx("^LRPParenchymaModel$");
  vtksys::RegularExpression portalModelEx("^LRPPortalModel$");
  vtksys::RegularExpression hepaticModelEx("^LRPHepaticModel$");

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
    std::pair<std::string, std::string> id_name;
    id_name.first = std::string(modelNode->GetID());
    id_name.second =std::string(modelNode->GetName());
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

    // Inform that a hepatic node was added
    std::pair<std::string, std::string> id_name;
    id_name.first = std::string(modelNode->GetID());
    id_name.second = std::string(modelNode->GetName());

    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
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
    }

  //Check wether the



  // Check whether the added node was a resection node
  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(addedNode);
  if (resectionNode)
    {
    vtkDebugMacro("Resection planning logic: Resection node added "
                  << resectionNode->GetName());

    std::pair<std::string, std::string> id_name;
    id_name.first = std::string(resectionNode->GetID());
    id_name.second = std::string(resectionNode->GetName());

    this->InvokeEvent(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                      static_cast<void*>(&id_name));
    return;
    }


}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
