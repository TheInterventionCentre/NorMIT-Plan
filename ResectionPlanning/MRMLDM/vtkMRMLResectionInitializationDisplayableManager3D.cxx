/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionInitializationDisplayableManager3D.cxx

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

// This module includes
#include "vtkMRMLResectionInitializationDisplayableManager3D.h"
#include "vtkMRMLResectionInitializationNode.h"
#include "vtkMRMLResectionInitializationDisplayNode.h"
#include "vtkLineWidget3.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkCallbackCommand.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionInitializationDisplayableManager3D);

//------------------------------------------------------------------------------
vtkMRMLResectionInitializationDisplayableManager3D::
vtkMRMLResectionInitializationDisplayableManager3D()
{
  vtkDebugMacro("Creating vtkMRMLResectionInitializationDisplayableManager3D");
}

//------------------------------------------------------------------------------
vtkMRMLResectionInitializationDisplayableManager3D::
~vtkMRMLResectionInitializationDisplayableManager3D()
{
  vtkDebugMacro("Deleteing vtkMRMLResectionInitializationDisplayableManager3D");
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
SetMRMLSceneInternal(vtkMRMLScene *newScene)
{
  Superclass::SetMRMLSceneInternal(newScene);

  this->OnMRMLSceneEndClose();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
SetAndObserveNode(vtkMRMLResectionInitializationNode *initializationNode)
{
  if (!initializationNode)
    {
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);
  nodeEvents->InsertNextValue(vtkMRMLDisplayableNode::DisplayModifiedEvent);

  vtkUnObserveMRMLNodeMacro(initializationNode);
  vtkObserveMRMLNodeEventsMacro(initializationNode, nodeEvents.GetPointer());
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
ProcessMRMLNodesEvents(vtkObject *object,
                       unsigned long int eventId,
                       void *vtkNotUsed(data))
{
  vtkMRMLResectionInitializationNode *node =
    vtkMRMLResectionInitializationNode::SafeDownCast(object);

  if (!node)
    {
    return;
    }

  bool isUpdating = this->GetMRMLScene()->IsBatchProcessing();

  switch(eventId)
    {
    case vtkMRMLDisplayableNode::DisplayModifiedEvent:
      this->UpdateVisibility(node);
      break;

    case vtkCommand::ModifiedEvent:
      this->UpdateLineWidget(node);
      break;

    default:
      break;
    }

  if (!isUpdating)
    {
    this->RequestRender();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
UpdateFromMRMLScene()
{
  // this->UpdateFromMRML();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
UpdateFromMRML()
{

}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
OnMRMLSceneNodeAdded(vtkMRMLNode *node)
{
  vtkMRMLScene *scene = this->GetMRMLScene();

  if (!node || !scene)
    {
    return;
    }

  vtkDebugMacro("OnMRMLSceneNodeAddedEvent");

  // Check if the scene is updating, exit if so.
  if (scene->IsBatchProcessing())
    {
    return;
    }

  if (node->IsA("vtkMRMLInteractionNode"))
    {
    // TODO this->AddObserversToInteractionNode();
    return;
    }

  vtkMRMLResectionInitializationNode *initializationNode =
    vtkMRMLResectionInitializationNode::SafeDownCast(node);
  if (!initializationNode)
    {
    return;
    }

  vtkDebugMacro("OnMRMLSceneNodeAddedEvent: node " << node->GetID());

  // There should not be any widget for the new node
  vtkMRMLResectionInitializationDisplayableManager3D::NodeWidgetIt it;
  it = this->NodeWidgetMap.find(initializationNode);
  if (it != this->NodeWidgetMap.end())
    {
    vtkErrorMacro("OnMRMLSceneNodeAddedEvent: "
                  << "a widget is already associated to this node.");
    return;
    }


  if (!this->AddWidget(initializationNode))
    {
    vtkErrorMacro("OnMRMLSceneNodeAddedEvent: widget not created");
    return;
    }

  this->SetAndObserveNode(initializationNode);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *node)
{
  vtkDebugMacro("OnMRMLSceneNodeRemoved");

  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  vtkMRMLResectionInitializationNode *initializationNode =
    vtkMRMLResectionInitializationNode::SafeDownCast(node);
  if (!initializationNode)
    {
    vtkErrorMacro("Node is not a vtkMRMLResectionInitializationNode");
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(initializationNode);
  if (it == this->NodeWidgetMap.end())
    {
    vtkErrorMacro("Node does not have an associated vtkLineWidget3");
    return;
    }

  it->second->Off();
  this->NodeWidgetMap.erase(it);
}

//------------------------------------------------------------------------------
bool vtkMRMLResectionInitializationDisplayableManager3D::
AddWidget(vtkMRMLResectionInitializationNode *initializationNode)
{
  vtkDebugMacro("AddWidget: create widget.");

  if (!initializationNode)
    {
    vtkErrorMacro("AddWidget: node not set!");
    return false;
    }

  // Create the line widget and position it according to the bounds of the
  // parenchyma (if that exists).
  vtkSmartPointer<vtkLineWidget3> lineWidget =
    vtkSmartPointer<vtkLineWidget3>::New();
  lineWidget->SetInteractor(this->GetInteractor());
  lineWidget->SetCurrentRenderer(this->GetRenderer());

  lineWidget->On();
  if (initializationNode->GetTargetParenchyma())
    {
    if (initializationNode->GetTargetParenchyma()->GetPolyData())
      {
      lineWidget->PlaceWidget(initializationNode->GetTargetParenchyma()
                              ->GetPolyData()->GetBounds());
      }
    }

  if (!initializationNode->GetTargetParenchyma())
    {
    vtkErrorMacro("AddWidget: there is no parenchyma model.");
    return false;
    }

  lineWidget->SetCuttingTarget(initializationNode->
                               GetTargetParenchyma()->GetPolyData());


  // Register the node-widget association
  this->NodeWidgetMap[initializationNode] = lineWidget;

  vtkSmartPointer<vtkCallbackCommand> lineWidgetChanged =
    vtkSmartPointer<vtkCallbackCommand>::New();
  lineWidgetChanged->SetCallback(this->UpdateMRML);
  lineWidgetChanged->SetClientData(initializationNode);

  lineWidget->AddObserver(vtkCommand::StartInteractionEvent,
                           lineWidgetChanged);
  lineWidget->AddObserver(vtkCommand::EndInteractionEvent,
                          lineWidgetChanged);

  initializationNode->SetPoint1(lineWidget->GetPoint1());
  initializationNode->SetPoint2(lineWidget->GetPoint2());

  return true;
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
UpdateMRML(vtkObject *caller,
           long unsigned int eventId,
           void *clientData,
           void *vtkNotUsed(callData))
{
  vtkLineWidget3 *widget = vtkLineWidget3::SafeDownCast(caller);

  if (!widget)
    {
    std::cerr << "UpdateMRML from a non vtkLineWidget3" << std::endl;
    return;
    }

  vtkMRMLResectionInitializationNode *node =
    static_cast<vtkMRMLResectionInitializationNode*>(clientData);
  if (!node)
    {
    std::cerr << "Client data (initialization node) not valid." << std::endl;
    return;
    }

  node->SetPoint1(widget->GetPoint1());
  node->SetPoint2(widget->GetPoint2());
  node->Modified();

  if (eventId == vtkCommand::StartInteractionEvent)
    {
    node->InvokeEvent(vtkCommand::StartInteractionEvent);
    }

  if (eventId == vtkCommand::EndInteractionEvent)
    {
    node->InvokeEvent(vtkCommand::EndInteractionEvent);
    }
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
UpdateLineWidget(vtkMRMLResectionInitializationNode *node)
{
  if (!node)
    {
    vtkErrorMacro("Error: no node passed.");
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(node);
  if (it != this->NodeWidgetMap.end())
    {
    vtkLineWidget3 *widget = it->second;
    widget->SetPoint1(node->GetPoint1());
    widget->SetPoint2(node->GetPoint2());
    }
}

//------------------------------------------------------------------------------
void vtkMRMLResectionInitializationDisplayableManager3D::
UpdateVisibility(vtkMRMLResectionInitializationNode *node)
{
  if (!node)
    {
    return;
    }

  vtkMRMLResectionInitializationDisplayNode *displayNode =
    vtkMRMLResectionInitializationDisplayNode::SafeDownCast(
      node->GetDisplayNode());

  if (!displayNode)
    {
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(node);
  if (it != this->NodeWidgetMap.end())
    {
    vtkLineWidget3 *widget = it->second;
    widget->SetEnabled(displayNode->GetVisibility());
    }
}
