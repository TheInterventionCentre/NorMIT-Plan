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
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"
#include "vtkBezierSurfaceWidget.h"
#include "vtkHausdorffDistancePointSetFilter.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtk3DWidget.h>
#include <vtkCollection.h>
#include <vtkDoubleArray.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkPointData.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataNormals.h>
#include <vtkColorTransferFunction.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkClipPolyData.h>
#include <vtkDistancePolyDataFilter.h>

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
void vtkMRMLResectionDisplayableManager3D::
PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Number of vtkMRMLResectionSurfaceNodes handled: "
     << this->NodeWidgetMap.size() << std::endl;

  NodeWidgetIt it;
  for(it=this->NodeWidgetMap.begin(); it!=this->NodeWidgetMap.end(); ++it)
    {
    os << indent << it->first << "associated to " << it->second << std::endl;
    }
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
  vtkDebugMacro("SetAndObserveNode");

  if (!resectionNode)
    {
    vtkErrorMacro("No node passed");
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
  vtkDebugMacro("ProcessMRMLNodesEvents");

  vtkMRMLResectionSurfaceNode *node =
    vtkMRMLResectionSurfaceNode::SafeDownCast(object);

  // Check for null node
  if (!node)
    {
    return;
    }

  // Check whether the manager is handling the node
  NodeWidgetIt it = this->NodeWidgetMap.find(node);
  if (it == this->NodeWidgetMap.end())
    {
    vtkErrorMacro("Resection node is not currently "
                  << "handled by the displayable manager");
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
  vtkDebugMacro("AddWidget");

  if (!resectionNode)
    {
    vtkErrorMacro("Node not set!");
    return false;
    }

  if (!this->GetMRMLViewNode())
    {
    vtkErrorMacro("No view node.");
    return false;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return false;
    }

  vtkSmartPointer<vtkBezierSurfaceWidget> surfaceWidget =
    vtkSmartPointer<vtkBezierSurfaceWidget>::New();
  surfaceWidget->SetInteractor(this->GetInteractor());
  surfaceWidget->SetCurrentRenderer(this->GetRenderer());
  surfaceWidget->SetHandleSizeFactor(0.8);
  surfaceWidget->On();
  surfaceWidget->ComputeNormalsOn();
  surfaceWidget->SetControlPoints(resectionNode->GetControlPoints());

  // Register the node-widget association.
  this->NodeWidgetMap[resectionNode] = surfaceWidget;

  vtkSmartPointer<vtkCallbackCommand> updateMRMLCallback =
    vtkSmartPointer<vtkCallbackCommand>::New();
  updateMRMLCallback->SetCallback(this->UpdateMRML);
  updateMRMLCallback->SetClientData(resectionNode);
  surfaceWidget->AddObserver(vtkCommand::StartInteractionEvent,
                             updateMRMLCallback);
  surfaceWidget->AddObserver(vtkCommand::EndInteractionEvent,
                             updateMRMLCallback);

  vtkSmartPointer<vtkCallbackCommand> updateDistanceMapCallback =
    vtkSmartPointer<vtkCallbackCommand>::New();
  updateDistanceMapCallback->SetCallback(this->UpdateDistanceMap);
  updateDistanceMapCallback->SetClientData(this);
  surfaceWidget->AddObserver(vtkCommand::StartInteractionEvent,
                             updateDistanceMapCallback);
  surfaceWidget->AddObserver(vtkCommand::EndInteractionEvent,
                             updateDistanceMapCallback);

  return true;
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
AddDistanceMapPipeline(vtkMRMLResectionSurfaceNode *node)
{
  vtkDebugMacro("CreateDistanceMapPipeline");

  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer present.");
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode * displayNode =
    vtkMRMLResectionSurfaceDisplayNode::
    SafeDownCast(node->GetDisplayNode());
  if (!displayNode)
    {
    vtkErrorMacro("No display node associated to the resection node.");
    return;
    }

  // Create and register the distance-to-parenchyma
  vtkSmartPointer<vtkDistancePolyDataFilter> parDistanceFilter =
    vtkSmartPointer<vtkDistancePolyDataFilter>::New();
  parDistanceFilter->ComputeSecondDistanceOff();
  parDistanceFilter->NegateDistanceOff();
  parDistanceFilter->SignedDistanceOn();
  parDistanceFilter->GlobalWarningDisplayOff();
  this->NodeParDistanceFilterMap[node] = parDistanceFilter;

  // Create and register the clipPolyData filter
  vtkSmartPointer<vtkClipPolyData> clipper  =
    vtkSmartPointer<vtkClipPolyData>::New();

  clipper->SetValue(0);
  clipper->InsideOutOn();
  clipper->GenerateClippedOutputOff();
  this->NodeClipperMap[node] = clipper;

  // Create and register the distance-to-tumors filter
  vtkSmartPointer<vtkHausdorffDistancePointSetFilter> distanceFilter =
    vtkSmartPointer<vtkHausdorffDistancePointSetFilter>::New();
  distanceFilter->SetTargetDistanceMethod(0);
  this->NodeDistanceFilterMap[node] = distanceFilter;

  // Create normals filter
  vtkSmartPointer<vtkPolyDataNormals> normals =
    vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputConnection(distanceFilter->GetOutputPort());

  // Create the contour filter
  vtkSmartPointer<vtkContourFilter> contourFilter =
    vtkSmartPointer<vtkContourFilter>::New();
  contourFilter->SetInputConnection(distanceFilter->GetOutputPort());
  contourFilter->SetNumberOfContours(1);
  contourFilter->SetValue(0, node->GetResectionMargin());
  this->NodeContourFilterMap[node] = contourFilter;

  // Create and register the color map
  vtkSmartPointer<vtkColorTransferFunction> colorMap =
    vtkSmartPointer<vtkColorTransferFunction>::New();
  colorMap->AddRGBPoint(0.0, 1.0, 1.0, 0.0);
  colorMap->AddRGBPoint(node->GetResectionMargin(), 1.0, 1.0, 0.0);
  colorMap->AddRGBPoint(node->GetResectionMargin(), 1.0, 1.0, 1.0);
  colorMap->AddRGBPoint(100.0, 1.0, 1.0, 1.0);
  colorMap->AllowDuplicateScalarsOn();
  this->NodeColorMap[node] = colorMap;

  // Create the mapper for the distance map
  vtkSmartPointer<vtkPolyDataMapper> distanceMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  distanceMapper->SetInputConnection(normals->GetOutputPort());
  distanceMapper->SetLookupTable(colorMap);
  distanceMapper->ScalarVisibilityOn();
  distanceMapper->SetScalarRange(0,100);

  // Create the mapper for the contour
  vtkSmartPointer<vtkPolyDataMapper> contourMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  contourMapper->SetInputConnection(contourFilter->GetOutputPort());

    // Create and register the distance actor
  vtkSmartPointer<vtkActor> distanceActor = vtkSmartPointer<vtkActor>::New();
  distanceActor->SetMapper(distanceMapper);
  //distanceActor->VisibilityOff();
  this->NodeDistanceActorMap[node] = distanceActor;

  //Create and register the contour actor
  vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
  contourActor->SetMapper(contourMapper);
  contourActor->GetProperty()->SetLineWidth(3);
  //contourActor->VisibilityOff();
  this->NodeContourActorMap[node] = contourActor;

  //vtkPolyDataMapper::SetResolveCoincidentTopologyToPolygonOffset();

  node->SetPolyDataConnection(normals->GetOutputPort());

  // Add actors to the scene
  if (displayNode->GetVisibility())
    {
    vtkProperty *prop = distanceActor->GetProperty();
    if (prop)
      {
      prop->SetOpacity(displayNode->GetOpacity());
      }

    prop = contourActor->GetProperty();
    if (prop)
      {
      prop->SetOpacity(displayNode->GetOpacity());
      }

    this->GetRenderer()->AddActor(distanceActor);
    this->GetRenderer()->AddActor(contourActor);
    }
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRMLScene()
{
  vtkDebugMacro("UpdateFromMRMLScene");
  this->RequestRender();
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::UpdateFromMRML()
{
  vtkDebugMacro("UpdateFromMRML");

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

    NodeWidgetIt it = this->NodeWidgetMap.find(resectionNode);
    if (it != this->NodeWidgetMap.end())
      {
      this->UpdateGeometry(resectionNode);
      this->UpdateVisibility(resectionNode);
      }
    }

  this->SetUpdateFromMRMLRequested(0);
}

//-------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer present");
    return;
    }

  // Removing the widgets
  NodeWidgetIt widgetIt;
  for(widgetIt=this->NodeWidgetMap.begin();
      widgetIt!=this->NodeWidgetMap.end();
      widgetIt++)
    {
    widgetIt->second->Off();
    vtkUnObserveMRMLNodeMacro(widgetIt->first);
    this->NodeWidgetMap.erase(widgetIt);
    }

  // Removing contour actors
  NodeContourActorIt contourActorIt;
  for(contourActorIt = this->NodeContourActorMap.begin();
      contourActorIt != this->NodeContourActorMap.end();
      contourActorIt++)
    {
    this->GetRenderer()->RemoveActor(contourActorIt->second);
    this->NodeContourActorMap.erase(contourActorIt);
    }

  // Removing contour filters
  NodeContourFilterIt contourFilterIt;
  for(contourFilterIt = this->NodeContourFilterMap.begin();
      contourFilterIt != this->NodeContourFilterMap.end();
      contourFilterIt++)
    {
    this->NodeContourFilterMap.erase(contourFilterIt);
    }

  // Removing distance actors
  NodeDistanceActorIt distanceActorIt;
  for(distanceActorIt = this->NodeDistanceActorMap.begin();
      distanceActorIt != this->NodeDistanceActorMap.end();
      distanceActorIt++)
    {
    this->GetRenderer()->RemoveActor(distanceActorIt->second);
    this->NodeDistanceActorMap.erase(distanceActorIt);
    }

  // Removing colormaps
  NodeColorIt colorIt;
  for(colorIt = this->NodeColorMap.begin();
      colorIt != this->NodeColorMap.end();
      colorIt++)
    {
    this->NodeColorMap.erase(colorIt);
    }

  // Removing distance filters
  NodeDistanceFilterIt distanceFilterIt;
  for(distanceFilterIt = this->NodeDistanceFilterMap.begin();
      distanceFilterIt != this->NodeDistanceFilterMap.end();
      distanceFilterIt++)
    {
    this->NodeDistanceFilterMap.erase(distanceFilterIt);
    }

  this->SetUpdateFromMRMLRequested(1);
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
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

  if (!this->GetMRMLViewNode())
    {
    vtkErrorMacro("No view node present");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  // Check if the scene is updating, exit if so
  if (this->GetMRMLScene()->IsBatchProcessing())
    {
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionSurfaceNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (!resectionSurfaceNode)
    {
    return;
    }

  // Check that the node does not have any widget associated.
  NodeWidgetIt it = NodeWidgetMap.find(resectionSurfaceNode);
  if (it != NodeWidgetMap.end())
    {
    return;
    }

  // Add a widget
  if (!this->AddWidget(resectionSurfaceNode))
    {
    vtkErrorMacro("OnMRMLSceneNodeAddedEvent: widget not created");
    return;
    }

  // Add the distance map pipeline
  this->AddDistanceMapPipeline(resectionSurfaceNode);

  this->SetAndObserveNode(resectionSurfaceNode);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *node)
{
  vtkDebugMacro("OnMRMLSceneNodeRemoved.");

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
    return;
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

  // Remove contour actor
  NodeContourActorIt contourActorIt =
    this->NodeContourActorMap.find(resectionNode);
  if (contourActorIt == this->NodeContourActorMap.end())
    {
    return;
    }
  this->GetRenderer()->RemoveActor(contourActorIt->second);
  this->NodeContourActorMap.erase(contourActorIt);

  // Remove contour filter
  NodeContourFilterIt contourFilterIt =
    this->NodeContourFilterMap.find(resectionNode);
  if (contourFilterIt == this->NodeContourFilterMap.end())
    {
    return;
    }
  this->NodeContourFilterMap.erase(contourFilterIt);

  // Remove distance actor
  NodeDistanceActorIt distanceActorIt =
    this->NodeDistanceActorMap.find(resectionNode);
  if (distanceActorIt == this->NodeDistanceActorMap.end())
    {
    return;
    }
  this->GetRenderer()->RemoveActor(distanceActorIt->second);
  this->NodeDistanceActorMap.erase(distanceActorIt);

  // Remove color map
  NodeColorIt colorIt = this->NodeColorMap.find(resectionNode);
  if (colorIt == this->NodeColorMap.end())
    {
    return;
    }
  this->NodeColorMap.erase(colorIt);

  // Remove distance filter
  NodeDistanceFilterIt distanceFilterIt =
    this->NodeDistanceFilterMap.find(resectionNode);
  if (distanceFilterIt == this->NodeDistanceFilterMap.end())
    {
    return;
    }
  this->NodeDistanceFilterMap.erase(distanceFilterIt);

  // Remove the cutter
  NodeClipperIt clipperIt =
    this->NodeClipperMap.find(resectionNode);
  if (clipperIt == this->NodeClipperMap.end())
    {
    return;
    }
  this->NodeClipperMap.erase(clipperIt);

  // Remove distance filter
  NodeParDistanceFilterIt parDistanceFilterIt =
    this->NodeParDistanceFilterMap.find(resectionNode);
  if (parDistanceFilterIt == this->NodeParDistanceFilterMap.end())
    {
    return;
    }
  this->NodeParDistanceFilterMap.erase(parDistanceFilterIt);


  // Remove widget
  NodeWidgetIt it = this->NodeWidgetMap.find(resectionNode);
  if (it == this->NodeWidgetMap.end())
    {
    return;
    }
  it->second->RemoveAllObservers();
  it->second->Off();
  this->NodeWidgetMap.erase(it);

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateGeometry(vtkMRMLResectionSurfaceNode *node)
{
  vtkDebugMacro("UpdateGeometry.");

  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  NodeWidgetIt it = this->NodeWidgetMap.find(node);
  if (it == this->NodeWidgetMap.end())
    {
    vtkErrorMacro("Node not handled by the displayable manager.");
    return;
    }

  vtkBezierSurfaceWidget *widget = it->second;
  widget->SetControlPoints(node->GetControlPoints());

  NodeColorIt colorIt = this->NodeColorMap.find(node);
  if (colorIt != this->NodeColorMap.end())
    {
    colorIt->second->RemoveAllPoints();
    colorIt->second->AddRGBPoint(0.0, 1.0, 1.0, 0.0);
    colorIt->second->AddRGBPoint(node->GetResectionMargin(), 1.0, 1.0, 0.0);
    colorIt->second->AddRGBPoint(node->GetResectionMargin(), 1.0, 1.0, 1.0);
    colorIt->second->AddRGBPoint(100.0, 1.0, 1.0, 1.0);
    }

  NodeContourFilterIt contourIt =
    this->NodeContourFilterMap.find(node);

  if (contourIt != this->NodeContourFilterMap.end())
    {
    contourIt->second->SetValue(0, node->GetResectionMargin());
    }
}


//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
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


  // Bezier widget (show/hide accordingly)
  NodeWidgetIt widgetIt = this->NodeWidgetMap.find(node);
  if (widgetIt == this->NodeWidgetMap.end())
    {
    vtkErrorMacro("Node not handled by the displayable manager.");
    return;
    }

  vtkMRMLModelNode *targetParenchyma = node->GetTargetParenchyma();
  if (!targetParenchyma)
    {
    vtkErrorMacro("No target parenchyma found.");
    return;
    }

  vtkBezierSurfaceWidget *widget = widgetIt->second;
  widget->SetEnabled(resectionDisplayNode->GetVisibility() &&
                     resectionDisplayNode->GetWidgetVisibility());

  NodeParDistanceFilterIt parDistFiIt = this->NodeParDistanceFilterMap.find(node);
  if (parDistFiIt == this->NodeParDistanceFilterMap.end())
    {
    vtkErrorMacro("No distance filter (parenchyma) associated to the node.");
    return;
    }

  vtkDistancePolyDataFilter *parDistanceFilter = parDistFiIt->second;
  parDistanceFilter->SetInputData(0, widget->GetBezierSurfacePolyData());
  parDistanceFilter->SetInputData(1, targetParenchyma->GetPolyData());

  NodeClipperIt clipperIt = this->NodeClipperMap.find(node);
  if (clipperIt == this->NodeClipperMap.end())
    {
    vtkErrorMacro("No clipper associated to the node.");
    return;
    }

  vtkClipPolyData *clipper = clipperIt->second;
  clipper->SetInputConnection(parDistanceFilter->GetOutputPort());

  NodeDistanceFilterIt distFilIt = this->NodeDistanceFilterMap.find(node);
  if (distFilIt == this->NodeDistanceFilterMap.end())
    {
    vtkErrorMacro("No distance filter (tumors) associated to the node.");
    return;
    }

  vtkHausdorffDistancePointSetFilter *distanceFilter = distFilIt->second;
  if (!resectionDisplayNode->GetWidgetVisibility())
    {
    distanceFilter->SetInputConnection(0, clipper->GetOutputPort());
    }
  else
    {
    distanceFilter->SetInputData(0, widget->GetBezierSurfacePolyData());
    }

  distanceFilter->Update();
  distanceFilter->GetOutput(0)->GetPointData()->
    SetScalars(distanceFilter->GetOutput(0)->
               GetPointData()->GetArray("Distance"));

  vtkProperty *prop = widget->GetBezierSurfaceProperty();
  prop->SetOpacity(resectionDisplayNode->GetOpacity());

  // Node contour actor (show/hide accordingly)
  NodeContourActorIt contourIt = this->NodeContourActorMap.find(node);
  if (contourIt == this->NodeContourActorMap.end())
    {
    vtkErrorMacro("No contour actor associated with resection node.");
    return;
    }

  //

  if (resectionDisplayNode->GetVisibility())
    {
    vtkProperty *prop = contourIt->second->GetProperty();
      if (prop)
        {
          prop->SetOpacity(resectionDisplayNode->GetOpacity());
        }
    this->GetRenderer()->AddActor(contourIt->second);

    }
  else
    {
    this->GetRenderer()->RemoveActor(contourIt->second);
    }

  NodeDistanceActorIt distanceIt = this->NodeDistanceActorMap.find(node);
  if (distanceIt == this->NodeDistanceActorMap.end())
    {
    vtkErrorMacro("No bezier surface (with distance) actor associated "
                  << "with the given resection node");
    return;
    }

  if (resectionDisplayNode->GetVisibility())
    {
    vtkProperty *prop = distanceIt->second->GetProperty();
      if (prop)
        {
          prop->SetOpacity(resectionDisplayNode->GetOpacity());
        }
    this->GetRenderer()->AddActor(distanceIt->second);

    }
  else
    {
    this->GetRenderer()->RemoveActor(distanceIt->second);
    }
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateDistanceMap(vtkObject *caller,
                  unsigned long int eventId,
                  void *clientData,
                  void *vtkNotUsed(callData))
{
  vtkBezierSurfaceWidget *widget =
    vtkBezierSurfaceWidget::SafeDownCast(caller);

  if (!widget)
    {
    std::cerr << "Update from a non vtkBezierWidget" << std::endl;
    return;
    }

  vtkMRMLResectionDisplayableManager3D *self =
    static_cast<vtkMRMLResectionDisplayableManager3D*>(clientData);
  if (!self)
    {
    std::cerr << "No displayable manager passed as clientData." << std::endl;
    return;
    }

  //Find the associated resection node
  NodeWidgetIt widgetIt;
  for(widgetIt=self->NodeWidgetMap.begin();
      widgetIt!=self->NodeWidgetMap.end();
      widgetIt++)
    {
    if (widgetIt->second.GetPointer() == widget)
      {
      break;
      }
    }

  if (widgetIt == self->NodeWidgetMap.end())
    {
    std::cerr << "No resection node is associated to the current widget."
              << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *node = widgetIt->first;

  if (!node)
    {
    std::cerr << "Surface node not valid" << std::endl;
    return;
    }

  if (eventId == vtkCommand::StartInteractionEvent)
    {
    widget->BezierSurfaceOn();
    NodeDistanceActorIt distIt = self->NodeDistanceActorMap.find(node);
    if (distIt != self->NodeDistanceActorMap.end())
      {
      distIt->second->VisibilityOff();
      }

    NodeContourActorIt contIt = self->NodeContourActorMap.find(node);
    if (contIt != self->NodeContourActorMap.end())
      {
      contIt->second->VisibilityOff();
      }
    }

  if (eventId == vtkCommand::EndInteractionEvent)
    {
    widget->BezierSurfaceOff();

    if (!self->GetMRMLScene())
      {
      std::cerr << "No mrml scene is present." << std::endl;
      return;
      }

    vtkSmartPointer<vtkCollection> nodes;
    nodes.TakeReference(self->GetMRMLScene()->
                        GetNodesByName("LRPJointTumorsModel"));
    vtkMRMLModelNode *jointTumorsModelNode =
      vtkMRMLModelNode::SafeDownCast(nodes->GetItemAsObject(0));
    if (!jointTumorsModelNode)
      {
      std::cerr << "No tumors model node found." << std::endl;
      return;
      }

    NodeDistanceFilterIt distFilIt = self->NodeDistanceFilterMap.find(node);
    if (distFilIt == self->NodeDistanceFilterMap.end())
      {
      std::cerr << "No distance filter (tumors) associated to the node."
                << std::endl;
      return;
      }


    vtkHausdorffDistancePointSetFilter *distanceFilter = distFilIt->second;
    distanceFilter->SetInputData(0, widget->GetBezierSurfacePolyData());
    distanceFilter->SetInputData(1, jointTumorsModelNode->GetPolyData());
    distanceFilter->Update();
    distanceFilter->GetOutput(0)->GetPointData()->
      SetScalars(distanceFilter->GetOutput(0)->
                 GetPointData()->GetArray("Distance"));

    NodeDistanceActorIt distActorIt = self->NodeDistanceActorMap.find(node);
    if (distActorIt != self->NodeDistanceActorMap.end())
      {
      distActorIt->second->VisibilityOn();
      }

    NodeContourActorIt contActorIt = self->NodeContourActorMap.find(node);
    if (contActorIt != self->NodeContourActorMap.end())
      {
      contActorIt->second->VisibilityOn();
      }
    }

  self->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLResectionDisplayableManager3D::
UpdateMRML(vtkObject *caller,
           unsigned long int eventId,
           void *clientData,
           void *vtkNotUsed(callData))
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
    std::cerr << "Client data (resection node) not valid." << std::endl;
    return;
    }

  node->SetControlPoints(widget->GetControlPoints());
  node->Modified();

  if (eventId == vtkCommand::StartInteractionEvent)
    {
    node->InvokeEvent(vtkCommand::StartInteractionEvent);
    }

  else if (eventId == vtkCommand::EndInteractionEvent)
    {
    node->InvokeEvent(vtkCommand::EndInteractionEvent);
    }
}
