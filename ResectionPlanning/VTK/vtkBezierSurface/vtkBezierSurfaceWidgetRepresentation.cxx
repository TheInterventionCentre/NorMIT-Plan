/*===============================================================================

  Project: vtkBezierSurface
  Module: vtkBezierSurfaceWidgetRepresentation.cxx

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is propietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/

// This module includes
#include "vtkBezierSurfaceWidgetRepresentation.h"
#include "vtkBezierSurfaceSource.h"

// VTK includes
#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkObjectFactory.h>
#include <vtkPickingManager.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkInteractorObserver.h>
#include <vtkBox.h>
#include <vtkPlaneSource.h>
#include <vtkCellArray.h>

// STD includes
#include <float.h> //for FLT_EPSILONs

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkBezierSurfaceWidgetRepresentation);

//------------------------------------------------------------------------------
vtkBezierSurfaceWidgetRepresentation::vtkBezierSurfaceWidgetRepresentation()
  : MovingInteraction(1)
{

  this->BezierSource = vtkSmartPointer<vtkBezierSurfaceSource>::New();

  this->InitializeWidget();
}

//------------------------------------------------------------------------------
vtkBezierSurfaceWidgetRepresentation::~vtkBezierSurfaceWidgetRepresentation()
{

}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::InitializeWidget()
{
  // Handle size is in pixels for this widget
  this->TubeSizeFactor = 0.15;

  this->HandleSize = 4.0;

  vtkSmartPointer<vtkCellArray> controlPolygonTopology =
    vtkSmartPointer<vtkCellArray>::New();

  for(unsigned int i=0; i<this->BezierSource->GetNumberOfControlPointsX() - 1; i++)
    {
      for(unsigned int j=0; j<this->BezierSource->GetNumberOfControlPointsY() - 1; j++)
        {
          int base = j*this->BezierSource->GetNumberOfControlPointsX() + i;
          int a = base;
          int b = base+1;
          int c = base+this->BezierSource->GetNumberOfControlPointsX()+1;
          int d = base+this->BezierSource->GetNumberOfControlPointsX();

          vtkIdType ls[2];

          ls[0] = a; ls[1] = b;
          controlPolygonTopology->InsertNextCell(2, ls);

          ls[0] = b; ls[1] = c;
          controlPolygonTopology->InsertNextCell(2, ls);

          ls[0] = c; ls[1] = d;
          controlPolygonTopology->InsertNextCell(2, ls);

          ls[0] = d; ls[1] = a;
          controlPolygonTopology->InsertNextCell(2, ls);
        }
    }

  this->ControlPolygon->SetPoints(this->BezierSource->GetControlPoints());
  this->ControlPolygon->SetLines(controlPolygonTopology);

  this->LastHandleInteracted = 0;

  Tuber->SetInputData(this->ControlPolygon.GetPointer());
  Tuber->SetNumberOfSides(12);

  this->ControlPolygonMapper->SetInputConnection(this->Tuber->GetOutputPort());
  this->ControlPolygonActor->SetMapper(this->ControlPolygonMapper.GetPointer());

  vtkSmartPointer<vtkSphereSource> sphere;
  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor> actor;
  for(unsigned int i=0; i<this->BezierSource->GetNumberOfControlPointsX(); i++)
    {
      for(unsigned int j=0; j<this->BezierSource->GetNumberOfControlPointsY(); j++)
        {
          //Create spheres
          vtkIdType pointIdx = i*this->BezierSource->GetNumberOfControlPointsY()+j;
          double point[3];
          this->BezierSource->GetControlPoints()->GetPoint(pointIdx,point);
          sphere = vtkSmartPointer<vtkSphereSource>::New();
          sphere->SetThetaResolution(32);
          sphere->SetPhiResolution(16);
          sphere->SetRadius(3);
          sphere->SetCenter(point);
          this->ControlPoints->AddItem(sphere);

          //Assign spheres to mappers
          mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
          mapper->SetInputConnection(sphere->GetOutputPort());
          this->ControlPointsMappers->AddItem(mapper);

          //Link mappers to actors
          actor = vtkSmartPointer<vtkActor>::New();
          actor->SetMapper(mapper);
          this->ControlPointsActors->AddItem(actor);
        }
    }

  this->ScaleEnabled = 1;
  this->OutsideBounds = 1;
  this->OutlineTranslation = 1;

  // Define the point coordinates
  double bounds[6];
  bounds[0] = -0.5;
  bounds[1] = 0.5;
  bounds[2] = -0.5;
  bounds[3] = 0.5;
  bounds[4] = -0.5;
  bounds[5] = 0.5;

  // Initial creation of the widget, serves to initialize it
  this->PlaceWidget(bounds);

  //Manage the picking stuff
  this->Picker->SetTolerance(0.005);

  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      this->Picker->AddPickList(actor);
    }

  this->Picker->AddPickList(this->ControlPolygonActor.GetPointer());

  this->Picker->PickFromListOn();

  // Set up the initial properties
  this->CreateDefaultProperties();

  numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      actor->SetProperty(this->HandlesProperty.GetPointer());
    }

  this->ControlPolygonActor->SetProperty(this->ControlPolygonProperty.GetPointer());
  this->RepresentationState = vtkBezierSurfaceWidgetRepresentation::Outside;
}


//------------------------------------------------------------------------------
int vtkBezierSurfaceWidgetRepresentation::ComputeInteractionState(int X, int Y,
                                                                  int vtkNotUsed(modify))
{
  // See if anything has been selected
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->Picker.GetPointer());

  if ( path == NULL ) // Not picking this widget
    {
    this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Outside);
    this->InteractionState = vtkBezierSurfaceWidgetRepresentation::Outside;
    return this->InteractionState;
    }

  // Something picked, continue
  this->ValidPick = 1;

   vtkProp *prop = path->GetFirstNode()->GetViewProp();

   if (prop == this->ControlPolygonActor.GetPointer() )
     {
       // If moving interaction is disables
       if (!this->MovingInteraction)
         {
           this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Outside);
           this->InteractionState = vtkBezierSurfaceWidgetRepresentation::Outside;
           return this->InteractionState;
         }

       // If moving interaction is enabled
       this->InteractionState = vtkBezierSurfaceWidgetRepresentation::Moving;
       this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Moving);
     }
   else
     {
     vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
     for(vtkIdType i=0; i<numControlActors; i++)
       {
       vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
       vtkActor *actor = vtkActor::SafeDownCast(object);
       if (prop == actor)
         {
         this->LastHandleInteracted = i;
         }
       }
     this->InteractionState = vtkBezierSurfaceWidgetRepresentation::Deforming;
     this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Deforming);
     }
    return this->InteractionState;
}

//------------------------------------------------------------------------------
int vtkBezierSurfaceWidgetRepresentation::ComputeMultiInteractionState(int X, int Y,
                                                                       int vtkNotUsed(modify))
{
  // See if anything has been selected
  vtkAssemblyPath* path = this->GetAssemblyPath(X, Y, 0., this->Picker.GetPointer());

  if ( path == NULL ) // Not picking this widget
    {
    this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Outside);
    this->InteractionState = vtkBezierSurfaceWidgetRepresentation::Outside;
    return this->InteractionState;
    }

  // Something picked, continue
  this->ValidPick = 1;

   vtkProp *prop = path->GetFirstNode()->GetViewProp();

   if (prop == this->ControlPolygonActor.GetPointer() )
     {
       return this->InteractionState;
     }
   else
     {
     vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
     for(vtkIdType i=0; i<numControlActors; i++)
       {
       vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
       vtkActor *actor = vtkActor::SafeDownCast(object);
       if (prop == actor)
         {
         this->LastHandleInteracted = i;
         }
       }
     this->InteractionState = vtkBezierSurfaceWidgetRepresentation::MultiDeforming;
     this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::MultiDeforming);
     }
    return this->InteractionState;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::SetRepresentationState(int state)
{
  if (this->RepresentationState == state)
    {
    return;
    }

  // Clamp the state
  state = (state < vtkBezierSurfaceWidgetRepresentation::Outside ?
           vtkBezierSurfaceWidgetRepresentation::Outside :
           (state > vtkBezierSurfaceWidgetRepresentation::MultiDeforming ?
            vtkBezierSurfaceWidgetRepresentation::MultiDeforming : state));

  this->RepresentationState = state;
  this->Modified();

  if ( state == vtkBezierSurfaceWidgetRepresentation::Deforming )
    {
    this->HighlightHandle(1,this->LastHandleInteracted);
    this->HighlightControlPolygon(0);
    }
  else if ( state == vtkBezierSurfaceWidgetRepresentation::MultiDeforming )
    {

      unsigned int row = this->LastHandleInteracted / 4;
      unsigned int column = this->LastHandleInteracted - row * 4;
      unsigned int xRes = this->BezierSource->GetNumberOfControlPointsX();
      unsigned int yRes = this->BezierSource->GetNumberOfControlPointsY();

      if ( row == 0 || row == xRes-1 || column == 0 || column == yRes-1)
        {
          for(unsigned int i=0; i<xRes; i++)
            {
              for(unsigned int j=0; j<yRes; j++)
                {
                  if (i == 0 || j== 0 || i == xRes-1 || j== yRes-1)
                    {
                      this->HighlightHandle(1,i*yRes+j);
                    }
                  else
                    {
                      this->HighlightHandle(0,i*yRes+j);
                    }
                }
            }
        }
      else
        {
          for(unsigned int i=0; i<xRes; i++)
            {
              for(unsigned int j=0; j<yRes; j++)
                {
                  if (i == 0 || j== 0 || i == xRes-1 || j== yRes-1)
                    {
                      this->HighlightHandle(0,i*yRes+j);
                    }
                  else
                    {
                      this->HighlightHandle(1,i*yRes+j);
                    }
                }
            }
        }
    }
  else if (state == vtkBezierSurfaceWidgetRepresentation::Moving)
    {
    this->HighlightControlPolygon(1);
    this->HighlightHandle(0,this->LastHandleInteracted);
    }
  else
    {
    this->HighlightHandle(0,this->LastHandleInteracted);
    this->HighlightControlPolygon(0);
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::StartWidgetInteraction(double e[2])
{
  this->StartEventPosition[0] = e[0];
  this->StartEventPosition[1] = e[1];
  this->StartEventPosition[2] = 0.0;

  this->LastEventPosition[0] = e[0];
  this->LastEventPosition[1] = e[1];
  this->LastEventPosition[2] = 0.0;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::WidgetInteraction(double e[2])
{
  // Do different things depending on state
  // Calculations everybody does
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z;

  vtkCamera *camera = this->Renderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }

  // Compute the two points defining the motion vector
  double pos[3];
  this->Picker->GetPickPosition(pos);
  vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,
                                               pos[0], pos[1], pos[2],
                                               focalPoint);
  z = focalPoint[2];
  vtkInteractorObserver::
    ComputeDisplayToWorld(this->Renderer,this->LastEventPosition[0],
                          this->LastEventPosition[1], z, prevPickPoint);
  vtkInteractorObserver::
    ComputeDisplayToWorld(this->Renderer, e[0], e[1], z, pickPoint);

  if (this->InteractionState == vtkBezierSurfaceWidgetRepresentation::Deforming)
    {
    this->Deform(pickPoint);
    }
  else if (this->InteractionState ==
           vtkBezierSurfaceWidgetRepresentation::MultiDeforming)
    {
    this->MultiDeform(prevPickPoint,pickPoint);
    }
  else if (this->InteractionState ==
           vtkBezierSurfaceWidgetRepresentation::Moving)
    {
    this->Move(prevPickPoint, pickPoint);
    }

  this->LastEventPosition[0] = e[0];
  this->LastEventPosition[1] = e[1];
  this->LastEventPosition[2] = 0.0;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::
EndWidgetInteraction(double vtkNotUsed(e)[2])
{
  this->SetRepresentationState(vtkBezierSurfaceWidgetRepresentation::Outside);
}

//------------------------------------------------------------------------------
double *vtkBezierSurfaceWidgetRepresentation::GetBounds()
{
  this->BuildRepresentation();

  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      this->BoundingBox->AddBounds(actor->GetBounds());
    }

  this->BoundingBox->AddBounds(this->ControlPolygonActor->GetBounds());

  return this->BoundingBox->GetBounds();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::GetActors(vtkPropCollection *pc)
{
  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      actor->GetActors(pc);
    }

  this->ControlPolygonActor->GetActors(pc);
}

//----------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::ReleaseGraphicsResources(vtkWindow *w)
{
  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      actor->ReleaseGraphicsResources(w);
    }

  this->ControlPolygonActor->ReleaseGraphicsResources(w);
}

//------------------------------------------------------------------------------
int vtkBezierSurfaceWidgetRepresentation::RenderOpaqueGeometry(vtkViewport *v)
{
  int count=0;
  this->BuildRepresentation();

  // Render Control Points
  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
    vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
    vtkActor *actor = vtkActor::SafeDownCast(object);

    if (!actor->GetVisibility())
      {
        break;
      }

    count += actor->RenderOpaqueGeometry(v);
    }

  //Render Control Polygon
  if (this->ControlPolygonActor->GetVisibility())
    {
     count += this->ControlPolygonActor->RenderOpaqueGeometry(v);
    }

  return count;
}

//------------------------------------------------------------------------------
int vtkBezierSurfaceWidgetRepresentation::RenderTranslucentPolygonalGeometry(
  vtkViewport *v)
{
  int count=0;
  this->BuildRepresentation();

  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
    vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
    vtkActor *actor = vtkActor::SafeDownCast(object);
    count += actor->RenderTranslucentPolygonalGeometry(v);
    }

    count += this->ControlPolygonActor->RenderTranslucentPolygonalGeometry(v);

  return count;
}

//------------------------------------------------------------------------------
int vtkBezierSurfaceWidgetRepresentation::HasTranslucentPolygonalGeometry()
{
  int result=0;

  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
    vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
    vtkActor *actor = vtkActor::SafeDownCast(object);
    result |= actor->HasTranslucentPolygonalGeometry();
    }

  result |= this->ControlPolygonActor->HasTranslucentPolygonalGeometry();

  return result;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::HighlightHandle(int highlight,
                                                           vtkIdType handle)
{
  for(vtkIdType i=0; i<ControlPointsActors->GetNumberOfItems(); i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);

      if (highlight && i==handle)
        {
          actor->SetProperty(this->SelectedHandlesProperty.GetPointer());
        }
      else
        {
          if (this->InteractionState !=
              vtkBezierSurfaceWidgetRepresentation::MultiDeforming)
            {
              actor->SetProperty(this->HandlesProperty.GetPointer());
            }
        }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::HighlightControlPolygon(int highlight)
{
  if (highlight)
    {
      this->ControlPolygonActor->
        SetProperty(this->SelectedControlPolygonProperty.GetPointer());
    }
  else
    {
      this->ControlPolygonActor->
        SetProperty(this->ControlPolygonProperty.GetPointer());
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::Move(double *originPoint,
                                                double *endPoint)
{
  double motionVector[3];
  motionVector[0] = endPoint[0] - originPoint[0];
  motionVector[1] = endPoint[1] - originPoint[1];
  motionVector[2] = endPoint[2] - originPoint[2];

  for(vtkIdType i=0; i<this->ControlPoints->GetNumberOfItems();i++)
    {
    //Move the control points
    vtkObject *object =
      this->ControlPoints->GetItemAsObject(i);
    vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);
    double *spherePoint = sphere->GetCenter();

    double destinationPoint[3];
    destinationPoint[0] = spherePoint[0] +  motionVector[0];
    destinationPoint[1] = spherePoint[1] +  motionVector[1];
    destinationPoint[2] = spherePoint[2] +  motionVector[2];

    sphere->SetCenter(destinationPoint);

    //Move the control polygon
    double *controlPoint = this->ControlPolygon->GetPoint(i);
    destinationPoint[0] = controlPoint[0] +  motionVector[0];
    destinationPoint[1] = controlPoint[1] +  motionVector[1];
    destinationPoint[2] = controlPoint[2] +  motionVector[2];
    this->ControlPolygon->GetPoints()->SetPoint(i, destinationPoint);
    this->ControlPolygon->Modified();
    }

  this->BezierSource->SetControlPoints(this->ControlPolygon->GetPoints());
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::Deform(double *ep)
{
  vtkObject *object =
    this->ControlPoints->GetItemAsObject(this->LastHandleInteracted);
  vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);
  sphere->SetCenter(ep);
  vtkSmartPointer<vtkPoints> points = this->BezierSource->GetControlPoints();
  points->SetPoint(this->LastHandleInteracted,ep);
  this->BezierSource->SetControlPoints(points);
  this->ControlPolygon->SetPoints(points);
  this->ControlPolygon->Modified();
  this->SizeHandles();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::MultiDeform(double *originPoint,
                                                       double *endPoint)
{

  double motionVector[3];
  motionVector[0] = endPoint[0] - originPoint[0];
  motionVector[1] = endPoint[1] - originPoint[1];
  motionVector[2] = endPoint[2] - originPoint[2];

  unsigned int row = this->LastHandleInteracted / 4;
  unsigned int column = this->LastHandleInteracted - row * 4;
  unsigned int xRes = this->BezierSource->GetNumberOfControlPointsX();
  unsigned int yRes = this->BezierSource->GetNumberOfControlPointsY();
  vtkSmartPointer<vtkPoints> points = this->BezierSource->GetControlPoints();

  if ( row == 0 || row == xRes-1 || column == 0 || column == yRes-1)
    {
    for(unsigned int i=0; i<xRes; i++)
      {
      for(unsigned int j=0; j<yRes; j++)
        {
        if (i == 0 || j== 0 || i == xRes-1 || j== yRes-1)
          {
          vtkObject *object = this->ControlPoints->GetItemAsObject(i*yRes+j);
          vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);

          double *spherePoint = sphere->GetCenter();

          double destinationPoint[3];
          destinationPoint[0] = spherePoint[0] +  motionVector[0];
          destinationPoint[1] = spherePoint[1] +  motionVector[1];
          destinationPoint[2] = spherePoint[2] +  motionVector[2];

          sphere->SetCenter(destinationPoint);
          points->SetPoint(i*yRes+j,destinationPoint);
          }
        }
      }
    }
  else
    {
    for(unsigned int i=0; i<xRes; i++)
      {
      for(unsigned int j=0; j<yRes; j++)
        {
        if (i != 0 && j != 0 && i != xRes-1 && j != yRes-1)
          {

          vtkObject *object = this->ControlPoints->GetItemAsObject(i*yRes+j);
          vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);

          double *spherePoint = sphere->GetCenter();

          double destinationPoint[3];
          destinationPoint[0] = spherePoint[0] +  motionVector[0];
          destinationPoint[1] = spherePoint[1] +  motionVector[1];
          destinationPoint[2] = spherePoint[2] +  motionVector[2];

          sphere->SetCenter(destinationPoint);
          points->SetPoint(i*yRes+j,destinationPoint);
          }
        }
      }
    }

  this->BezierSource->SetControlPoints(points);
  this->ControlPolygon->SetPoints(points);
  this->ControlPolygon->Modified();
  this->SizeHandles();
}


//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::CreateDefaultProperties()
{
  // Handles properties
  this->HandlesProperty->SetColor(1,1,1);
  this->HandlesProperty->SetLineWidth(2);

  this->SelectedHandlesProperty->SetColor(1,0,0);
  this->HandlesProperty->SetLineWidth(2);

  this->ControlPolygonProperty->SetAmbient(0.2);
  this->ControlPolygonProperty->SetDiffuse(1.0);
  this->ControlPolygonProperty->SetAmbientColor(0.5,0.5,0.5);

  this->SelectedControlPolygonProperty->SetAmbient(0.2);
  this->SelectedControlPolygonProperty->SetDiffuse(1.0);
  this->SelectedControlPolygonProperty->SetColor(0.0, 1.0, 0.0);

}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::PlaceWidget(double bds[6])
{
  double bounds[6], origin[3];

  this->AdjustBounds(bds, bounds, origin);

  this->ValidPick = 1; // since we have positioned the widget successfully
  this->BuildRepresentation();
}


//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::BuildRepresentation()
{
  if ( ! this->Renderer )
    {
    return;
    }

  vtkInformation *info = this->GetPropertyKeys();

  vtkIdType numControlActors = this->ControlPointsActors->GetNumberOfItems();
  for(vtkIdType i=0; i<numControlActors; i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);
      actor->SetPropertyKeys(info);
    }

  this->ControlPolygonActor->SetPropertyKeys(info);

  if ( this->GetMTime() > this->BuildTime)
    {
    this->SizeHandles();
    this->BuildTime.Modified();
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::SizeHandles()
{
  vtkObject *object = this->ControlPoints->GetItemAsObject(0);
  vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);
  double radius =
    this->vtkWidgetRepresentation::SizeHandlesInPixels(this->HandleSize,
                                                       sphere->GetCenter());

  for(vtkIdType i=0; i<this->ControlPoints->GetNumberOfItems(); i++)
    {
      vtkObject *object = this->ControlPoints->GetItemAsObject(i);
      vtkSphereSource *sphere = vtkSphereSource::SafeDownCast(object);
      sphere->SetRadius(radius);
    }

  this->Tuber->SetRadius(this->TubeSizeFactor*radius);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::RegisterPickers()
{
  this->Renderer->GetRenderWindow()->GetInteractor()->GetPickingManager()
    ->AddPicker(this->Picker.GetPointer(), this);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::ShowControlPoints()
{
  for(vtkIdType i=0; i<this->ControlPointsActors->GetNumberOfItems(); i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);

      actor->SetVisibility(1);
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::HideControlPoints()
{
  for(vtkIdType i=0; i<this->ControlPointsActors->GetNumberOfItems(); i++)
    {
      vtkObject *object = this->ControlPointsActors->GetItemAsObject(i);
      vtkActor *actor = vtkActor::SafeDownCast(object);

      actor->SetVisibility(0);
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::ShowControlPolygon()
{
  this->ControlPolygonActor->SetVisibility(1);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidgetRepresentation::HideControlPolygon()
{
  this->ControlPolygonActor->SetVisibility(0);
}

//------------------------------------------------------------------------------
void  vtkBezierSurfaceWidgetRepresentation::
SetBezierSurfaceSource(vtkBezierSurfaceSource *source)
{
  this->BezierSource = source;
  this->ControlPointsActors->RemoveAllItems();
  this->ControlPointsMappers->RemoveAllItems();
  this->ControlPoints->RemoveAllItems();

  this->InitializeWidget();
  this->Modified();
  this->BuildRepresentation();
  this->SizeHandles();
}
