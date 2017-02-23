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
bool vtkMRMLResectionDisplayableManager3D::
AddWidget(vtkMRMLResectionSurfaceNode *resectionNode)
{
  vtkDebugMacro("AddWidget: calling createWidget");

  if (!resectionNode)
    {
    vtkErrorMacro("CreateWdiget: Node not set!");
    return false;
    }

  vtkSmartPointer<vtkBezierSurfaceWidget> surfaceWidget =
    vtkSmartPointer<vtkBezierSurfaceWidget>::New();
  surfaceWidget->SetInteractor(this->GetInteractor());
  surfaceWidget->SetCurrentRenderer(this->GetRenderer());
  surfaceWidget->On();

  this->Helper->RecordWidgetForNode(surfaceWidget.GetPointer(),
                                    resectionNode);

  vtkDebugMacro("AddWidget: saved to helper ");

  this->SetAndObserveNode(resectionNode);
  this->RequestRender();

  return true;
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRMLScene()
{
  //this->UpdateFromMRML();
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRML()
{

}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");
}

void vtkMRMLResectionDisplayableManager3D::OnMRMLSceneNodeAdded(vtkMRMLNode *node)
{
  vtkDebugMacro("Adding node");
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

  if (node->IsA("vtkMRMLResectionSurfaceDisplayNode"))
    {
    vtkObserveMRMLNodeMacro(node);
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
                  << "The node is already associated to "
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
  else
    {
    vtkDebugMacro("OnMRMLSceneNodeAddedEvent: widget was created and "
                  << "saved to helper records");
    }

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *vtkNotUsed(node))
{

}
