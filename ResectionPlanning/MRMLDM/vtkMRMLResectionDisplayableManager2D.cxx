/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager2D.cxx

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
#include "vtkMRMLResectionDisplayableManager2D.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkBezierSurfaceSource.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkPlane.h>
#include <vtkMatrix4x4.h>
#include <vtkCutter.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkRenderer.h>
#include <vtkProperty2D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionDisplayableManager2D);


//------------------------------------------------------------------------------
vtkMRMLResectionDisplayableManager2D::
vtkMRMLResectionDisplayableManager2D()
{

}

//------------------------------------------------------------------------------
vtkMRMLResectionDisplayableManager2D::
~vtkMRMLResectionDisplayableManager2D()
{
}


//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
SetMRMLSceneInternal(vtkMRMLScene *newScene)
{
  this->Superclass::SetMRMLSceneInternal(newScene);

  this->OnMRMLSceneEndClose();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer");
    return;
    }

  // Removing actors and node observations
  ResectionActorIt actorIt;
  for(actorIt=this->ResectionActorMap.begin();
      actorIt!=this->ResectionActorMap.end(); actorIt++)
    {
    this->GetRenderer()->RemoveActor(actorIt->second);
    vtkUnObserveMRMLNodeMacro(actorIt->first);
    this->ResectionActorMap.erase(actorIt);
    }

  // Removing mappers
  ResectionMapperIt mapIt;
  for(mapIt=this->ResectionMapperMap.begin();
      mapIt!=this->ResectionMapperMap.end(); mapIt++)
    {
    this->ResectionMapperMap.erase(mapIt);
    }

  // Removing transform filters
  ResectionTransformFilterIt transformIt;
  for(transformIt=ResectionTransformFilterMap.begin();
      transformIt!=ResectionTransformFilterMap.end();
      transformIt++)
    {
    this->ResectionTransformFilterMap.erase(transformIt);
    }

  // Removing cutters
  ResectionCutterIt cutterIt;
  for(cutterIt=ResectionCutterMap.begin();
      cutterIt!=ResectionCutterMap.end();
      cutterIt++)
    {
    this->ResectionCutterMap.erase(cutterIt);
    }

  // Removing Bézier surfaces
  ResectionBezierIt bezierIt;
  for(bezierIt=ResectionBezierMap.begin();
      bezierIt!=ResectionBezierMap.end();
      bezierIt++)
    {
    this->ResectionBezierMap.erase(bezierIt);
    }


  this->SetUpdateFromMRMLRequested(1);
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
OnMRMLSceneNodeAdded(vtkMRMLNode *node)
{
  vtkDebugMacro("OnMRMLSceneNodeAdded");

  if (!node)
    {
    vtkErrorMacro("No node passed");
    return;
    }

  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene has been set");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  vtkMRMLSliceNode *sliceNode = this->GetMRMLSliceNode();
  if (!sliceNode)
    {
    vtkErrorMacro("No slice node present");
    return;
    }

  // Jump out if scene is batch processing
  if (this->GetMRMLScene()->IsBatchProcessing())
    {
    return;
    }

  // Check this is a resection node (we do not care about the others)
  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (!resectionNode)
    {
    return;
    }

  // Check whether the node has an associated representation
  ResectionActorIt it = ResectionActorMap.find(resectionNode);
  if (it != ResectionActorMap.end())
    {
    return;
    }

  if (!this->AddRepresentation(resectionNode))
    {
    return;
    }

  vtkObserveMRMLNodeMacro(node);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (!resectionNode)
    {
    return;
    }

  // this->SetUpdateFromMRMLRequested(1);

  // if (this->GetMRMLScene()->IsBatchProcessing())
  //   {
  //   return;
  //   }

  // Check if the node is in our association map (actors)
  ResectionActorIt actorIt = this->ResectionActorMap.find(resectionNode);
  if (actorIt != this->ResectionActorMap.end())
    {
    // Remove the corresponding actor from the scene
    vtkActor2D *actor = actorIt->second;
    this->GetRenderer()->RemoveActor(actor);

    // Remove the association
    this->ResectionActorMap.erase(actorIt);
    }

  // Check if the node is in our association map (mapper)
  ResectionMapperIt mapIt = this->ResectionMapperMap.find(resectionNode);
  if (mapIt != this->ResectionMapperMap.end())
    {
    this->ResectionMapperMap.erase(mapIt);
    }

  // Check if the node is in our association map (transform filter)
  ResectionTransformFilterIt trIt =
    this->ResectionTransformFilterMap.find(resectionNode);
  if (trIt != this->ResectionTransformFilterMap.end())
    {
    this->ResectionTransformFilterMap.erase(trIt);
    }

  // Check inf the node is in our association map (cutter)
  ResectionCutterIt cutIt =
    this->ResectionCutterMap.find(resectionNode);
  if (cutIt != this->ResectionCutterMap.end())
    {
    this->ResectionCutterMap.erase(cutIt);
    }

  ResectionBezierIt bezierIt = this->ResectionBezierMap.find(resectionNode);
  if (bezierIt != this->ResectionBezierMap.end())
    {
    this->ResectionBezierMap.erase(bezierIt);
    }

  // Remove the observations from the node
  vtkUnObserveMRMLNodeMacro(resectionNode);
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::UpdateFromMRMLScene()
{
  vtkDebugMacro("UpdateFromMRMLScene");
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::UpdateFromMRML()
{
  vtkDebugMacro("UpdateFromMRML");

  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }

  if ( this->GetInteractor() &&
       this->GetInteractor()->GetRenderWindow() &&
       this->GetInteractor()->GetRenderWindow()->CheckInRenderStatus())
    {
    vtkDebugMacro("skipping update during render");
    return;
    }
  std::vector<vtkMRMLNode*> dNodes;
  int nNodes =
    scene ? scene->GetNodesByClass("vtkMRMLResectionSurfaceNode", dNodes):0;

  for(int n=0; n<nNodes; n++)
    {

    vtkMRMLResectionSurfaceNode *resectionNode =
      vtkMRMLResectionSurfaceNode::SafeDownCast(dNodes[n]);

    ResectionActorIt it = this->ResectionActorMap.find(resectionNode);
    if (it != this->ResectionActorMap.end())
      {
      this->UpdateGeometry(resectionNode);
      this->UpdateVisibility(resectionNode);
      }
    }

  this->SetUpdateFromMRMLRequested(0);
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
SetAndObserveNode(vtkMRMLResectionSurfaceNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);
  nodeEvents->InsertNextValue(vtkMRMLDisplayableNode::DisplayModifiedEvent);

  vtkUnObserveMRMLNodeMacro(node);
  vtkObserveMRMLNodeEventsMacro(node, nodeEvents.GetPointer());
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
ProcessMRMLNodesEvents(vtkObject *caller,
                       unsigned long int eventId,
                       void *vtkNotUsed(callData))
{
  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(caller);

  if (!resectionNode)
    {
    return;
    }

  // Check whether the manager is handling the node
  ResectionActorIt it = this->ResectionActorMap.find(resectionNode);
  if (it == this->ResectionActorMap.end())
    {
    vtkErrorMacro("Resection node is not currently "
                  << "handled by the displayable manager");
    return;
    }

  switch(eventId)
    {
    case vtkCommand::ModifiedEvent:
      this->UpdateGeometry(resectionNode);
      break;

    case vtkMRMLDisplayableNode::DisplayModifiedEvent:
      this->UpdateVisibility(resectionNode);
      break;

    default:
      break;
    }
}

//------------------------------------------------------------------------------
bool vtkMRMLResectionDisplayableManager2D::
AddRepresentation(vtkMRMLResectionSurfaceNode *node)
{
  vtkDebugMacro("AddRepresentation");

  if (!node)
    {
    vtkErrorMacro("No resection surface node passed");
    return false;
    }

  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node.");
    return false;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return false;
    }

  // Set up the bezier surface
  vtkSmartPointer<vtkBezierSurfaceSource> bezierSource =
    vtkSmartPointer<vtkBezierSurfaceSource>::New();
  bezierSource->SetNumberOfControlPoints(4,4);
  bezierSource->SetResolution(40,40);

  // Register bezier source
  this->ResectionBezierMap[node] = bezierSource;

  //Set up the cutter
  vtkSmartPointer<vtkCutter> cutter =
    vtkSmartPointer<vtkCutter>::New();
  cutter->SetInputConnection(bezierSource->GetOutputPort());
  cutter->GlobalWarningDisplayOff();
  cutter->GenerateCutScalarsOff();
  cutter->GenerateTrianglesOn();

  // Register the cutter
  this->ResectionCutterMap[node] = cutter;

  // Set the transformation filter
  vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
    vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  transformFilter->SetInputConnection(cutter->GetOutputPort());
  transformFilter->GlobalWarningDisplayOff();

  // Register the transformation filter
  this->ResectionTransformFilterMap[node] = transformFilter;

  // Set the mapper, actor and add it to the scene
  vtkSmartPointer<vtkPolyDataMapper2D> mapper =
    vtkSmartPointer<vtkPolyDataMapper2D>::New();
  mapper->SetInputConnection(transformFilter->GetOutputPort());
  mapper->SetInputConnection(cutter->GetOutputPort());
  mapper->GlobalWarningDisplayOff();

  // Register the mapper
  this->ResectionMapperMap[node] = mapper;

  // Set the actor
  vtkSmartPointer<vtkActor2D> actor =
    vtkSmartPointer<vtkActor2D>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(3);

  // Register the actor
  this->ResectionActorMap[node] = actor;

  this->GetRenderer()->AddActor2D(actor);

  this->UpdateGeometry(node);

  return true;
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
UpdateGeometry(vtkMRMLResectionSurfaceNode *node)
{
  vtkDebugMacro("UpdateGeometry");

  if (!node)
    {
    vtkErrorMacro("No node passed");
    return;
    }

  ResectionBezierIt resIt = this->ResectionBezierMap.find(node);
  if (resIt == this->ResectionBezierMap.end())
    {
    vtkErrorMacro("No bezier surface associated to resection node provided.");
    return;
    }

  ResectionCutterIt cutIt = this->ResectionCutterMap.find(node);
  if (cutIt == this->ResectionCutterMap.end())
    {
    vtkErrorMacro("No cutter associated ot the resection node provided.");
    return;
    }

  ResectionTransformFilterIt trIt = this->ResectionTransformFilterMap.find(node);
  if (trIt == this->ResectionTransformFilterMap.end())
    {
    vtkErrorMacro("No transform filter associated to the resection node provided.");
    return;
    }

  ResectionMapperIt mapIt = this->ResectionMapperMap.find(node);
  if (mapIt == this->ResectionMapperMap.end())
    {
    vtkErrorMacro("No mapper associated to the resection node provided.");
    return;
    }

  ResectionActorIt actorIt = this->ResectionActorMap.find(node);
  if (actorIt == this->ResectionActorMap.end())
    {
    vtkErrorMacro("No actor associated to the resection node provided.");
    return;
    }

  // Update the control points based on the node information
  if (!resIt->second || !cutIt->second || !trIt->second || !actorIt->second)
    {
    return;
    }

  resIt->second->SetControlPoints(node->GetControlPoints());

  // Check for slice node
  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node.");
    return;
    }

  // Compute the slice normal
  vtkMatrix4x4 *sliceToRASMatrix = this->GetMRMLSliceNode()->GetSliceToRAS();

  double slicePlaneNormal[3];
  slicePlaneNormal[0] = sliceToRASMatrix->GetElement(0,2);
  slicePlaneNormal[1] = sliceToRASMatrix->GetElement(1,2);
  slicePlaneNormal[2] = sliceToRASMatrix->GetElement(2,2);

  vtkSmartPointer<vtkPlane> cutPlane =
    vtkSmartPointer<vtkPlane>::New();
  cutPlane->SetNormal(slicePlaneNormal);

  cutIt->second->SetCutFunction(cutPlane);
  cutIt->second->GenerateValues(1,
                                this->GetMRMLSliceNode()->GetSliceOffset(),
                                this->GetMRMLSliceNode()->GetSliceOffset());

  // Transformation (3d to 2d)
  vtkSmartPointer<vtkMatrix4x4> invertedRASToXYMatrix =
    vtkSmartPointer<vtkMatrix4x4>::New();
  invertedRASToXYMatrix->DeepCopy(this->GetMRMLSliceNode()->GetXYToRAS());

  vtkSmartPointer<vtkTransform> rasToXYTrasnform =
    vtkSmartPointer<vtkTransform>::New();
  rasToXYTrasnform->SetMatrix(invertedRASToXYMatrix);

  trIt->second->SetTransform(rasToXYTrasnform);

  mapIt->second->SetInputConnection(trIt->second->GetOutputPort());
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager2D::
UpdateVisibility(vtkMRMLResectionSurfaceNode *node)
{
  vtkDebugMacro("UpdateVisibility.");

  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode *resectionDisplayNode =
    vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(node->GetDisplayNode());

  if (!resectionDisplayNode)
    {
    vtkErrorMacro("Node resection node does not have a display node.");
    return;
    }

  ResectionActorIt actorIt = this->ResectionActorMap.find(node);
  if (actorIt == this->ResectionActorMap.end())
    {
    vtkErrorMacro("Node not handled by the displayable manager.");
    return;
    }

  vtkActor2D *actor = actorIt->second;
  actor->SetVisibility(resectionDisplayNode->GetVisibility());
}
