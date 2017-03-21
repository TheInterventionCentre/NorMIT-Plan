/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager3D.cxx

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
#include "vtkMRMLResectionDisplayableManager3D.h"
#include "vtkMRMLResectionDisplayableManager3DHelper.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"
#include "vtkBezierSurfaceWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtk3DWidget.h>
#include <vtkCollection.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>

// STD includes
#include <iostream>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionDisplayableManager3D);


//------------------------------------------------------------------------------
vtkMRMLResectionDisplayableManager3D::vtkMRMLResectionDisplayableManager3D()
{
  vtkDebugMacro("Creating vtkMRMLResectionDisplayableManager3D");
}

//------------------------------------------------------------------------------
vtkMRMLResectionDisplayableManager3D::~vtkMRMLResectionDisplayableManager3D()
{
  vtkDebugMacro("Deleting vtkMRMLResectionDisplayableManager3D");
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::PrintSelf(ostream &vtkNotUsed(os),
                                                     vtkIndent vtkNotUsed(indent))
{

}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
SetMRMLSceneInternal(vtkMRMLScene *newScene)
{
  Superclass::SetMRMLSceneInternal(newScene);

  this->OnMRMLSceneEndClose();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
SetAndObserveNode(vtkMRMLResectionSurfaceNode* resectionNode)
{
  if (!resectionNode)
    {
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);
  nodeEvents->InsertNextValue(vtkMRMLDisplayableNode::DisplayModifiedEvent);

  vtkUnObserveMRMLNodeMacro(resectionNode);
  vtkObserveMRMLNodeEventsMacro(resectionNode, nodeEvents.GetPointer());
}


//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
ProcessMRMLNodesEvents(vtkObject *object,
                       unsigned long int eventId,
                       void *vtkNotUsed(data))
{
  vtkMRMLResectionSurfaceNode *node =
    vtkMRMLResectionSurfaceNode::SafeDownCast(object);

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
      this->UpdateGeometry(node);
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
bool vtkMRMLResectionDisplayableManager3D::
AddWidget(vtkMRMLResectionSurfaceNode *resectionNode)
{
  vtkDebugMacro("AddWidget: create widget.");

  if (!resectionNode)
    {
    vtkErrorMacro("AddWidget: Node not set!");
    return false;
    }

  vtkSmartPointer<vtkBezierSurfaceWidget> surfaceWidget =
    vtkSmartPointer<vtkBezierSurfaceWidget>::New();
  surfaceWidget->SetInteractor(this->GetInteractor());
  surfaceWidget->SetCurrentRenderer(this->GetRenderer());
  surfaceWidget->SetControlPoints(resectionNode->GetControlPoints());
  surfaceWidget->On();

  // Register the node-widget association.
  this->NodeWidgetMap[resectionNode] = surfaceWidget;

  vtkSmartPointer<vtkCallbackCommand> bezierWidgetChanged =
    vtkSmartPointer<vtkCallbackCommand>::New();
  bezierWidgetChanged->SetCallback(this->UpdateMRML);
  bezierWidgetChanged->SetClientData(resectionNode);
  surfaceWidget->AddObserver(vtkCommand::StartInteractionEvent,
                             bezierWidgetChanged);
  surfaceWidget->AddObserver(vtkCommand::EndInteractionEvent,
                             bezierWidgetChanged);

  resectionNode->SetControlPoints(surfaceWidget->GetControlPoints());

  return true;
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRMLScene()
{
  std::cout << "UpdateFROMMRMLScene()" << std::endl;

  this->RequestRender();
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRML()
{

  if ( this->GetInteractor() &&
       this->GetInteractor()->GetRenderWindow() &&
       this->GetInteractor()->GetRenderWindow()->CheckInRenderStatus())
    {
    vtkDebugMacro("skipping update during render");
    return;
    }

  vtkMRMLScene *scene = this->GetMRMLScene();

  std::vector<vtkMRMLNode*> dNodes;
  int nNodes =
    scene ? scene->GetNodesByClass("vtkMRMLResectionSurfaceNode", dNodes):0;

  for(int n=0; n<nNodes; n++)
    {

    vtkMRMLResectionSurfaceNode *resectionNode =
      vtkMRMLResectionSurfaceNode::SafeDownCast(dNodes[n]);

    this->UpdateGeometry(resectionNode);
    this->UpdateVisibility(resectionNode);
    }

  this->SetUpdateFromMRMLRequested(0);
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
OnMRMLSceneNodeAdded(vtkMRMLNode *node)
{
  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!node || !scene)
    {
    return;
    }

  vtkDebugMacro("OnMRMLSceneNodeAddedEvent");

  // Check if the scene is updating, exit if so
  if (scene->IsBatchProcessing())
    {
    return;
    }

  if (node->IsA("vtkMRMLInteractionNode"))
    {
    // TODO this->AddObserversToInteractionNode();
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionSurfaceNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (!resectionSurfaceNode)
    {
    return;
    }

  vtkDebugMacro("OnMRMLSceneNodeAddedEvent: node" << node->GetID());

  // Check if the node already exists in the list of associated nodes
  vtkMRMLResectionDisplayableManager3DHelper::ResectionSurfaceNodeListIt it;
  it = std::find(this->Helper->ResectionSurfaceNodeList.begin(),
                 this->Helper->ResectionSurfaceNodeList.end(),
                 resectionSurfaceNode);

  if (it != this->Helper->ResectionSurfaceNodeList.end())
    {
    vtkErrorMacro("OnMRMLSceneNodeAdded: "
                  << "the node is already associated to "
                  << "the displayable manager");
    return;
    }

  // There should not be a widget for the new node
  if (this->Helper->GetWidget(resectionSurfaceNode))
    {
    vtkErrorMacro("OnMRMLSceneNodeAddedEvent: "
                  << "A widget is already associated to this node.");
    return;
    }

  if (!this->AddWidget(resectionSurfaceNode))
    {
    vtkErrorMacro("OnMRMLSceneNodeAddedEvent: widget not created");
    return;
    }

  vtkDebugMacro("OnMRMLSceneNodeAddedEvent: widget was created and "
                << "saved to helper records");

  this->SetAndObserveNode(resectionSurfaceNode);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *node)
{
  vtkDebugMacro("From vtkMRMLResectionDisplayableManager3D: "
                << "node removed from scene.");

  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (!resectionNode)
    {
    return;
    }

  this->SetUpdateFromMRMLRequested(1);

  if (this->GetMRMLScene()->IsBatchProcessing())
    {
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(resectionNode);
  if (it == this->NodeWidgetMap.end())
    {
    return;
    }

  it->second->Off();
  this->NodeWidgetMap.erase(it);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateGeometry(vtkMRMLResectionSurfaceNode *node)
{
  if (!node)
    {
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(node);
  if (it != this->NodeWidgetMap.end())
    {
    vtkBezierSurfaceWidget *widget = it->second;
    widget->SetControlPoints(node->GetControlPoints());
    }
}


//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateVisibility(vtkMRMLResectionSurfaceNode *node){

  if (!node)
    {
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode *resectionDisplayNode =
    vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(node->GetDisplayNode());

  if (resectionDisplayNode)
    {
    NodeWidgetIt it = this->NodeWidgetMap.find(node);

    if (it != this->NodeWidgetMap.end())
      {
      vtkBezierSurfaceWidget *widget = it->second;
      widget->SetEnabled(resectionDisplayNode->GetVisibility());
      }
    }
}


//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateMRML(vtkObject *caller,
           unsigned long int eventId,
           void *clientData,
           void *vtkNotUsed(callData()))
{
  vtkBezierSurfaceWidget *widget =
    vtkBezierSurfaceWidget::SafeDownCast(caller);

  if (!widget)
    {
    std::cerr << "Update from a non vtkBezierWidget" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *node =
    static_cast<vtkMRMLResectionSurfaceNode*>(clientData);

  if (!node)
    {
    std::cerr << "Client data (surface node) not valid" << std::endl;
    return;
    }

  node->SetControlPoints(widget->GetControlPoints());
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
