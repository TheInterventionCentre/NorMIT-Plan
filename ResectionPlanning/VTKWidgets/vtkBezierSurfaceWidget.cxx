/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkBezierSurface.cxx

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
#include "vtkBezierSurfaceWidget.h"
#include "vtkBezierSurfaceSource.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkSphereSource.h>
#include <vtkTubeFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkExtractEdges.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyLine.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCollection.h>
#include <vtkCellPicker.h>
#include <vtkPickingManager.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPolyDataNormals.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkBezierSurfaceWidget);


//-------------------------------------------------------------------------------
vtkBezierSurfaceWidget::vtkBezierSurfaceWidget()
  :State(vtkBezierSurfaceWidget::Start),
   MultiPointInteraction(1),
   TranslationInteraction(1),
   ContinuousBezierUpdate(1),
   SurfaceResolutionX(40),
   SurfaceResolutionY(40),
   NumberOfControlPointsX(4),
   NumberOfControlPointsY(4),
   HandleSizeFactor(1.1),
   ControlPolygonSizeFactor(0.2),
   ComputeNormalsFlag(false),
   AutoSize(false),
   HandleSize(3.0),
   TubeRadius(1.0)
{
  // Set the event callback to our process events function
  this->EventCallbackCommand->SetCallback(vtkBezierSurfaceWidget::ProcessEvents);

  double startX = -0.5;
  double startY = -0.5;
  double endX = 0.5;
  double endY = 0.5;
  double incX = (endX - startX)/4.0;
  double incY = (endY - startY)/4.0;

  vtkSmartPointer<vtkPoints> planePoints =
    vtkSmartPointer<vtkPoints>::New();

  //Generate geometry;
  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      double point[3] = {startX+i*incX, startY+j*incY, 0.0};
      planePoints->InsertNextPoint(point);
      }
    }

  //Generate topology;
  vtkSmartPointer<vtkCellArray> planeCells =
    vtkSmartPointer<vtkCellArray>::New();
  for(int i=0; i<3; ++i)
    {
    for(int j=0; j<3; ++j)
      {
      vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
      polyLine->GetPointIds()->SetNumberOfIds(5);
      polyLine->GetPointIds()->SetId(0,i*4+j);
      polyLine->GetPointIds()->SetId(1,i*4+j+1);
      polyLine->GetPointIds()->SetId(2,(i+1)*4+j+1);
      polyLine->GetPointIds()->SetId(3,(i+1)*4+j);
      polyLine->GetPointIds()->SetId(4,i*4+j);
      planeCells->InsertNextCell(polyLine);
      }
    }

  this->ControlPolygonPolyData->SetPoints(planePoints);
  this->ControlPolygonPolyData->SetLines(planeCells);

  this->TubeFilter->SetInputData(this->ControlPolygonPolyData.GetPointer());
  this->TubeFilter->SetRadius(0.002);
  this->TubeFilter->SetNumberOfSides(20);
  this->TubeFilter->Update();

  // Create control polygon and related objects
  this->ControlPolygonMapper->SetInputConnection(this->TubeFilter->GetOutputPort());
  this->ControlPolygonActor->SetMapper(this->ControlPolygonMapper.GetPointer());

  // Create control points and related objects
  for(int i=0; i<16; i++)
    {
    vtkSmartPointer<vtkSphereSource> handle =
      vtkSmartPointer<vtkSphereSource>::New();
    handle->SetThetaResolution(32);
    handle->SetPhiResolution(16);
    handle->SetRadius(3);
    handle->SetCenter(planePoints->GetPoint(i));
    handle->Update();
    this->HandlePolyDataCollection->AddItem(handle);

    vtkSmartPointer<vtkPolyDataMapper> handleMapper =
      vtkSmartPointer<vtkPolyDataMapper>::New();
    handleMapper->SetInputData(handle->GetOutput());
    this->HandleMapperCollection->AddItem(handleMapper);

    vtkSmartPointer<vtkActor> handleActor =
      vtkSmartPointer<vtkActor>::New();
    handleActor->SetMapper(handleMapper);
    this->HandleActorCollection->AddItem(handleActor);
    }

  // Create the default visualization properties
  this->CreateDefaultProperties();

  // Manage handles picking
  this->HandlePicker->SetTolerance(0.001);
  for (int i=0; i<this->HandleActorCollection->GetNumberOfItems(); ++i)
    {
    vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
    vtkActor *handleActor = vtkActor::SafeDownCast(object);
    if (handleActor)
      {
      this->HandlePicker->AddPickList(handleActor);
      }
    }
  this->HandlePicker->PickFromListOn();

  // Manage control polygon picking
  this->ControlPolygonPicker->SetTolerance(0.001);
  this->ControlPolygonPicker->AddPickList(this->ControlPolygonActor.GetPointer());
  this->ControlPolygonPicker->PickFromListOn();

  this->CurrentHandle = NULL;

  // Crate default Bézier surface and associated elements
  this->BezierSurfaceSource->SetNumberOfControlPoints(this->NumberOfControlPointsX,
                                                      this->NumberOfControlPointsY);
  this->BezierSurfaceSource->SetResolution(this->SurfaceResolutionX,
                                           this->SurfaceResolutionY);

  this->BezierSurfaceSource->SetControlPoints(this->ControlPolygonPolyData->GetPoints());

  this->BezierSurfaceMapper->
    SetInputConnection(this->BezierSurfaceSource->GetOutputPort());

  this->BezierSurfaceActor->SetMapper(this->BezierSurfaceMapper.GetPointer());

  // Place the widget
  double bounds[6];
  bounds[0] = bounds[2] = bounds[4] = -0.5;
  bounds[1] = bounds[3] = bounds[5] =  0.5;
  this->PlaceWidget(bounds);
}

//-------------------------------------------------------------------------------
vtkBezierSurfaceWidget::~vtkBezierSurfaceWidget()
{

}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SetControlPoints(vtkPoints *points)
{
  this->ControlPoints->DeepCopy(points);
  this->BezierSurfaceSource->SetControlPoints(points);
  this->ControlPolygonPolyData->SetPoints(points);

  for(unsigned int i = 0; i<this->NumberOfControlPointsX; ++i)
    {
    for(unsigned int j = 0; j<this->NumberOfControlPointsY; ++j)
      {
      vtkSphereSource *source =
        vtkSphereSource::SafeDownCast(
          this->HandlePolyDataCollection->
          GetItemAsObject(i*NumberOfControlPointsX+j));

      source->SetCenter(points->GetPoint(i*this->NumberOfControlPointsX+j));
      source->Update();
      }
    }
  this->ValidPick = 1;
  this->SizeHandles();
  this->SizeControlPolygon();
  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SetEnabled(int enabling)
{
  if (!this->Interactor)
    {
    vtkErrorMacro(<<"The interactor must be set prior to "
                  <<"enabling/disabling widget");
    return;
    }

  if (enabling)
    {

    vtkDebugMacro(<<"Enabling widget");

    if (this->Enabled)
      {
      return;
      }

    if (!this->CurrentRenderer)
      {
      vtkRenderer *renderer =
        this->Interactor->FindPokedRenderer(
          this->Interactor->GetLastEventPosition()[0],
          this->Interactor->GetLastEventPosition()[1]);

      this->SetCurrentRenderer(renderer);
      if (this->CurrentRenderer == NULL)
        {
        return;
        }
      }

    this->Enabled = 1;

    // Interaction observers
    this->Interactor->AddObserver(vtkCommand::MouseMoveEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::LeftButtonPressEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::RightButtonPressEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::RightButtonReleaseEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    // Add Control Polygon
    this->ControlPolygonActor->SetProperty(this->ControlPolygonProperty.GetPointer());
    this->CurrentRenderer->AddActor(this->ControlPolygonActor.GetPointer());

    // Add Control Polygon Handles
    for(int i=0; i<this->HandleActorCollection->GetNumberOfItems(); i++)
      {
      vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
      vtkActor *handleActor = vtkActor::SafeDownCast(object);
      if (handleActor)
        {
        handleActor->SetProperty(this->HandleProperty.GetPointer());
        this->CurrentRenderer->AddActor(handleActor);
        }
      }

    // Add Bézier Surface
    this->BezierSurfaceActor->SetProperty(this->BezierSurfaceProperty.GetPointer());
    this->CurrentRenderer->AddActor(this->BezierSurfaceActor.GetPointer());

    // Enable event
    this->InvokeEvent(vtkCommand::EnableEvent, NULL);
    }
  else // Disabling
    {
    vtkDebugMacro(<<"Disabling widget");

    if (!this->Enabled)
      {
      return;
      }

    this->Enabled = 0;

    // Remove interaction callback
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // Remove control polygon
    this->CurrentRenderer->RemoveActor(this->ControlPolygonActor.GetPointer());

    // Remove handles
    for(int i =0; i<this->HandleActorCollection->GetNumberOfItems();++i)
      {
      vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
      vtkActor *handleActor = vtkActor::SafeDownCast(object);
      if (handleActor)
        {
        this->CurrentRenderer->RemoveActor(handleActor);
        }
      }

    // Remove Bézier surface
    this->CurrentRenderer->RemoveActor(this->BezierSurfaceActor.GetPointer());

    this->CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent, NULL);
    this->SetCurrentRenderer(NULL);
    }

  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ProcessEvents(vtkObject *vtkNotUsed(object),
                                           unsigned long event,
                                           void *clientData,
                                           void *vtkNotUsed(callData))
{
  vtkBezierSurfaceWidget *self =
    reinterpret_cast<vtkBezierSurfaceWidget*>(clientData);

  if (self == NULL)
    {
    return;
    }

  switch(event)
    {
    case vtkCommand::LeftButtonPressEvent:
      self->OnLeftButtonDown();
      break;

    case vtkCommand::LeftButtonReleaseEvent:
      self->OnLeftButtonUp();
      break;

    case vtkCommand::RightButtonPressEvent:
      self->OnRightButtonDown();
      break;

    case vtkCommand::RightButtonReleaseEvent:
      self->OnRightButtonUp();
      break;

    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  // Initial bounds
  double *bounds=this->InitialBounds;
  os << indent << "Initial Bounds: "
     << "(" << bounds[0] << "," << bounds[1] << ") "
     << "(" << bounds[2] << "," << bounds[3] << ") "
     << "(" << bounds[4] << "," << bounds[5] << ")\n";

  // Properties
  os << indent << "Handle property: " << this->HandleProperty << "\n";
  os << indent << "Selected handle property: " << this->SelectedHandleProperty << "\n";
  os << indent << "Control polygon property: " << this->ControlPolygonProperty << "\n";
  os << indent << "Selected control polygon property: "
     << this->SelectedControlPolygonProperty << "\n";
  os << indent << "Bézier surface property: " << this->BezierSurfaceProperty << "\n";

  // Interaction flags
  os << indent << "Multi-point interaction: "
     << (this->MultiPointInteraction ? "On\n" : "Off\n");
  os << indent << "Translation interaction: "
     << (this->TranslationInteraction ? "On\n" : "Off\n");
  os << indent << "Continuous Bézier update: "
     << (this->ContinuousBezierUpdate ? "On\n" : "Off\n");

  // Bézie surface
  os << indent << "Bézier surface: "
     << *this->BezierSurfaceSource.GetPointer() << "\n";
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::PositionHandles()
{
  for(int i=0; i<this->ControlPolygonPolyData->GetNumberOfPoints(); ++i)
    {
    vtkObject *object = this->HandlePolyDataCollection->GetItemAsObject(i);
    vtkSphereSource *handle = vtkSphereSource::SafeDownCast(object);
    if (handle)
      {
      handle->SetCenter(this->ControlPolygonPolyData->GetPoints()->GetPoint(i));
      handle->Update();
      }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::HandlesOn()
{
  for(int i=0; i<16; i++)
    {
    vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
    vtkActor *handleActor = vtkActor::SafeDownCast(object);
    if (handleActor)
      {
      handleActor->VisibilityOn();
      }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::HandlesOff()
{
  for(int i=0; i<16; i++)
    {
    vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
    vtkActor *handleActor = vtkActor::SafeDownCast(object);
    if (handleActor)
      {
      handleActor->VisibilityOff();
      }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ControlPolygonOn()
{
this->ControlPolygonActor->VisibilityOn();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ControlPolygonOff()
{
  this->ControlPolygonActor->VisibilityOff();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::BezierSurfaceOn()
{
  this->BezierSurfaceActor->VisibilityOn();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::BezierSurfaceOff()
{
  this->BezierSurfaceActor->VisibilityOff();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SizeHandles()
{
  double radius = this->AutoSize?
    this->vtk3DWidget::SizeHandles(this->HandleSizeFactor):
    this->HandleSize;

  for(int i=0; i<this->HandlePolyDataCollection->GetNumberOfItems(); ++i)
    {
    vtkObject *object = this->HandlePolyDataCollection->GetItemAsObject(i);
    vtkSphereSource *handle = vtkSphereSource::SafeDownCast(object);
    if (handle)
      {
      handle->SetRadius(radius);
      handle->Update();
      }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SizeControlPolygon()
{
  double radius = this->AutoSize?
    this->vtk3DWidget::SizeHandles(this->ControlPolygonSizeFactor):
    this->TubeRadius;
  this->TubeFilter->SetRadius(radius);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::HighlightHandle(vtkProp *prop)
{
  // Un-highlight control polygon
  this->HighlightControlPolygon(0);

  for(int i=0; i<this->HandleActorCollection->GetNumberOfItems(); ++i)
    {
    vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
    vtkActor *actor = vtkActor::SafeDownCast(object);
    if(actor)
      {
      actor->SetProperty(this->HandleProperty.GetPointer());
      }
    }

  // Set the new selected handle to highlight
  this->CurrentHandle = vtkActor::SafeDownCast(prop);
  if (this->CurrentHandle)
    {
    this->CurrentHandle->SetProperty(this->SelectedHandleProperty.GetPointer());
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::MultiHighlightHandle(vtkProp *prop)
{
  if (prop == NULL)
    {
    return;
    }

  int index = this->HandleActorCollection->IsItemPresent(prop);
  if (!index)
    {
    return;
    }

  // Un-highlight control polygon
  this->HighlightControlPolygon(0);

  // Highlight the correspondent group of handles
  int row = (index -1) / 4;
  int column = (index -1) - row * 4;

  bool externalMarking = row == 0 || row == 3 || column == 0 || column == 3;

  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      bool externalPoint = i == 0 || j == 0 || i == 3 || j == 3;

      vtkObject *object =
        this->HandleActorCollection->GetItemAsObject(i*4+j);
      vtkActor *handleActor = vtkActor::SafeDownCast(object);
      if (handleActor)
        {
        // External marking and external point --> highlight
        if(externalMarking && externalPoint)
          {
          handleActor->SetProperty(this->SelectedHandleProperty.GetPointer());
          }
        // External marking and internal point --> un-highlight
        else if (externalMarking && !externalPoint)
          {
          handleActor->SetProperty(this->HandleProperty.GetPointer());
          }
        // Non-External marking and non-external point --> highlight
        else if (!externalMarking && !externalPoint)
          {
          handleActor->SetProperty(this->SelectedHandleProperty.GetPointer());
          }
        // Non-External marking and external point --> un-highlight
        else
          {
          handleActor->SetProperty(this->HandleProperty.GetPointer());
          }
        }
      }
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::HighlightControlPolygon(int highlight)
{
  if (highlight)
    {
    // // Un-highlight handles
    // for(int i=0; i<this->HandleActorCollection->GetNumberOfItems(); ++i)
    //   {
    //   vtkObject *object = this->HandleActorCollection->GetItemAsObject(i);
    //   vtkActor *handleActor = vtkActor::SafeDownCast(object);
    //   if (handleActor)
    //     {
    //     handleActor->SetProperty(this->HandleProperty);
    //     }
    //   }

    // Highlight control polygon
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
void vtkBezierSurfaceWidget::OnLeftButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  if (!this->CurrentRenderer ||
      !this->CurrentRenderer->IsInViewport(X,Y))
    {
    this->State = vtkBezierSurfaceWidget::Outside;
    return;
    }

  vtkAssemblyPath *path = this->GetAssemblyPath(X, Y, 0.0,
                                                this->HandlePicker.GetPointer());

  if (path)
    {
    this->State = vtkBezierSurfaceWidget::Deforming;
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    this->ValidPick = 1;
    this->CurrentHandle = this->HandlePicker->GetActor();
    this->HighlightHandle(this->CurrentHandle);
    }
  else
    {

    path = this->GetAssemblyPath(X, Y, 0.,
                                 this->ControlPolygonPicker.GetPointer());

    if (path)
      {
      this->State = vtkBezierSurfaceWidget::Moving;
      this->ControlPolygonPicker->GetPickPosition(this->LastPickPosition);
      this->ValidPick = 1;
      this->CurrentHandle = NULL;
      this->HighlightHandle(NULL);
      this->HighlightControlPolygon(1);
      }
    else
      {
      this->HighlightHandle(NULL);
      this->HighlightControlPolygon(0);
      this->State = vtkBezierSurfaceWidget::Outside;
      return;
      }
    }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
  this->Interactor->Render();
}

//-------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::OnLeftButtonUp()
{
  if (this->State == vtkBezierSurfaceWidget::Outside ||
      this->State == vtkBezierSurfaceWidget::Start )
    {
    return;
    }

  this->State = vtkBezierSurfaceWidget::Start;
  this->SizeHandles();
  this->SizeControlPolygon();

  this->HighlightHandle(NULL);
  this->HighlightControlPolygon(0);
  this->CurrentHandle = NULL;

  this->BezierSurfaceSource->SetControlPoints(this->ControlPolygonPolyData->GetPoints());

  this->ControlPoints->DeepCopy(this->ControlPolygonPolyData->GetPoints());

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::OnRightButtonDown()
{
  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  if (!this->CurrentRenderer ||
      !this->CurrentRenderer->IsInViewport(X,Y))
    {
    this->State = vtkBezierSurfaceWidget::Outside;
    return;
    }

  vtkAssemblyPath *path = this->GetAssemblyPath(X, Y, 0.0,
                                                this->HandlePicker.GetPointer());

  if (path)
    {
    this->State = vtkBezierSurfaceWidget::MultiDeforming;
    this->CurrentHandle = this->HandlePicker->GetActor();
    this->ValidPick = 1;
    this->HandlePicker->GetPickPosition(this->LastPickPosition);
    this->MultiHighlightHandle(this->CurrentHandle);
    }
  else
    {
    path = this->GetAssemblyPath(X, Y, 0.,
                                 this->ControlPolygonPicker.GetPointer());

    if (path)
      {
      this->State = vtkBezierSurfaceWidget::Moving;
      this->ControlPolygonPicker->GetPickPosition(this->LastPickPosition);
      this->ValidPick = 1;
      this->CurrentHandle = NULL;
      this->HighlightHandle(NULL);
      this->HighlightControlPolygon(1);
      }
    else
      {
      this->HighlightHandle(NULL);
      this->HighlightControlPolygon(0);
      this->State = vtkBezierSurfaceWidget::Outside;
      return;
      }
    }

  this->EventCallbackCommand->SetAbortFlag(1);
  this->StartInteraction();
  this->InvokeEvent(vtkCommand::StartInteractionEvent, NULL);
  this->Interactor->Render();
}

//-------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::OnRightButtonUp()
{
  if (this->State == vtkBezierSurfaceWidget::Outside ||
      this->State == vtkBezierSurfaceWidget::Start)
    {
    return;
    }

  this->State = vtkBezierSurfaceWidget::Start;
  this->SizeHandles();
  this->SizeControlPolygon();
  this->HighlightHandle(NULL);
  this->HighlightControlPolygon(0);
  this->CurrentHandle = NULL;

  this->BezierSurfaceSource
    ->SetControlPoints(this->ControlPolygonPolyData->GetPoints());

  this->ControlPoints->DeepCopy(this->ControlPolygonPolyData->GetPoints());

  this->EventCallbackCommand->SetAbortFlag(1);
  this->EndInteraction();
  this->InvokeEvent(vtkCommand::EndInteractionEvent, NULL);
  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::OnMouseMove()
{
  if (this->State == vtkBezierSurfaceWidget::Outside ||
      this->State == vtkBezierSurfaceWidget::Start)
    {
    return;
    }

  int X = this->Interactor->GetEventPosition()[0];
  int Y = this->Interactor->GetEventPosition()[1];

  vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
  if (!camera)
    {
    return;
    }

  double focalPoint[4];
  this->ComputeWorldToDisplay(this->LastPickPosition[0],
                              this->LastPickPosition[1],
                              this->LastPickPosition[2],
                              focalPoint);

  double z = focalPoint[2];
  double lastX = static_cast<double>(this->Interactor->GetLastEventPosition()[0]);
  double lastY = static_cast<double>(this->Interactor->GetLastEventPosition()[1]);
  double prevPickPoint[4], pickPoint[4];
  this->ComputeDisplayToWorld(lastX, lastY, z, prevPickPoint);
  this->ComputeDisplayToWorld(static_cast<double>(X),
                              static_cast<double>(Y),
                              z, pickPoint);

  if (this->State == vtkBezierSurfaceWidget::Deforming)
    {
    int index =
      this->HandleActorCollection->IsItemPresent(this->CurrentHandle)-1;
    //this->HighlightHandle(this->CurrentHandle);
    this->MoveControlPoint(index, prevPickPoint, pickPoint);
    }
  else if (this->State == vtkBezierSurfaceWidget::MultiDeforming)
    {
    int index =
      this->HandleActorCollection->IsItemPresent(this->CurrentHandle)-1;
    this->MultiMoveControlPoint(index, prevPickPoint, pickPoint);
    }
  else if (this->State == vtkBezierSurfaceWidget::Moving)
    {
    this->MoveControlPolygon(prevPickPoint, pickPoint);
    }


  this->EventCallbackCommand->SetAbortFlag(1);
  this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
  this->Interactor->Render();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::MoveControlPolygon(double *p1, double*p2)
{

  double motionVector[3];
  motionVector[0] = p2[0] - p1[0];
  motionVector[1] = p2[1] - p1[1];
  motionVector[2] = p2[2] - p1[2];

  // Move the handles
  for(int i =0; i<this->HandlePolyDataCollection->GetNumberOfItems(); ++i)
    {
    vtkObject *object = this->HandlePolyDataCollection->GetItemAsObject(i);
    vtkSphereSource *handle = vtkSphereSource::SafeDownCast(object);

    if (handle == NULL)
      {
      continue;
      }

    double *handlePoint = handle->GetCenter();
    double destinationPoint[3];
    destinationPoint[0] = handlePoint[0] + motionVector[0];
    destinationPoint[1] = handlePoint[1] + motionVector[1];
    destinationPoint[2] = handlePoint[2] + motionVector[2];

    handle->SetCenter(destinationPoint);
    handle->Update();
    }

  // Move the control polygon
  for(int i=0; i<this->ControlPolygonPolyData->GetNumberOfPoints(); ++i)
    {
    double *point = this->ControlPolygonPolyData->GetPoint(i);
    double destinationPoint[3];
    destinationPoint[0] = point[0] + motionVector[0];
    destinationPoint[1] = point[1] + motionVector[1];
    destinationPoint[2] = point[2] + motionVector[2];

    this->ControlPolygonPolyData->GetPoints()->SetPoint(i, destinationPoint);
    }
  this->ControlPolygonPolyData->Modified();

  if (this->ContinuousBezierUpdate)
    {
    this->BezierSurfaceSource->
      SetControlPoints(this->ControlPolygonPolyData->GetPoints());
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::MoveControlPoint(int cp, double *p1, double *p2)
{
  if (cp<0)
    {
    return;
    }

  double motionVector[3];
  motionVector[0] = p2[0] - p1[0];
  motionVector[1] = p2[1] - p1[1];
  motionVector[2] = p2[2] - p1[2];

  vtkObject *object = this->HandlePolyDataCollection->GetItemAsObject(cp);
  vtkSphereSource *handle = vtkSphereSource::SafeDownCast(object);

  if (handle == NULL)
    {
    return;
    }

  double *handlePoint = handle->GetCenter();
  double destinationPoint[3];
  destinationPoint[0] = handlePoint[0] + motionVector[0];
  destinationPoint[1] = handlePoint[1] + motionVector[1];
  destinationPoint[2] = handlePoint[2] + motionVector[2];

  handle->SetCenter(destinationPoint);
  handle->Update();

  this->ControlPolygonPolyData->GetPoints()->SetPoint(cp, destinationPoint);
  this->ControlPolygonPolyData->Modified();

  if (this->ContinuousBezierUpdate)
    {
    this->BezierSurfaceSource->
      SetControlPoints(this->ControlPolygonPolyData->GetPoints());
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::MultiMoveControlPoint(int cp, double *p1, double *p2)
{
  if (cp<0)
    {
    return;
    }

  double motionVector[3];
  motionVector[0] = p2[0] - p1[0];
  motionVector[1] = p2[1] - p1[1];
  motionVector[2] = p2[2] - p1[2];

  int row = cp / 4;
  int column = cp - row * 4;

  bool externalMarking = row == 0 || row == 3 || column == 0 || column == 3;


  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      bool externalPoint = i == 0 || j == 0 || i == 3 || j == 3;

      vtkObject *object =
        this->HandlePolyDataCollection->GetItemAsObject(i*4+j);
      vtkSphereSource *handle = vtkSphereSource::SafeDownCast(object);

      if (handle)
        {
        // External marking and external point --> move
        // Internal marking and internal point --> move
        if ( (externalMarking && externalPoint) ||
             (!externalMarking && !externalPoint) )
          {
          double *handlePoint = handle->GetCenter();
          double destinationPoint[3];
          destinationPoint[0] = handlePoint[0] + motionVector[0];
          destinationPoint[1] = handlePoint[1] + motionVector[1];
          destinationPoint[2] = handlePoint[2] + motionVector[2];

          handle->SetCenter(destinationPoint);
          handle->Update();

          this->ControlPolygonPolyData->GetPoints()->SetPoint(i*4+j,
                                                              destinationPoint);
          }

        }

      }
    }

  this->ControlPolygonPolyData->Modified();

  if (this->ContinuousBezierUpdate)
    {
    this->BezierSurfaceSource->
      SetControlPoints(this->ControlPolygonPolyData->GetPoints());
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::CreateDefaultProperties()
{
  this->HandleProperty->SetColor(1.0, 1.0, 1.0);

  this->SelectedHandleProperty->SetColor(1.0, 0.0, 0.0);

  this->ControlPolygonProperty->SetColor(1.0, 1.0, 1.0);
  this->ControlPolygonProperty->LightingOff();

  this->SelectedControlPolygonProperty->SetColor(0.0, 1.0, 0.0);
  this->SelectedControlPolygonProperty->LightingOff();

  this->BezierSurfaceProperty->SetColor(1.0, 1.0, 1.0);
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::PlaceWidget(double bds[6])
{
  double bounds[6], center[3];
  this->AdjustBounds(bds, bounds, center);

  double startX = bounds[0];
  double startY = bounds[2];
  double incX = (bounds[1] - bounds[0]) / 4.0;
  double incY = (bounds[3] - bounds[2]) / 4.0;

  //Generate geometry;
  vtkSmartPointer<vtkPoints> planePoints =
    vtkSmartPointer<vtkPoints>::New();
  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      double point[3] = {startX+i*incX, startY+j*incY, 0.0};
      planePoints->InsertNextPoint(point);
      }
    }

  //Generate topology;
  vtkSmartPointer<vtkCellArray> planeCells =
    vtkSmartPointer<vtkCellArray>::New();
  for(int i=0; i<1; ++i)
    {
    for(int j=0; j<1; ++j)
      {
      vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
      polyLine->GetPointIds()->SetNumberOfIds(5);
      polyLine->GetPointIds()->SetId(0,i*4+j);
      polyLine->GetPointIds()->SetId(1,i*4+j+1);
      polyLine->GetPointIds()->SetId(2,(i+1)*4+j+1);
      polyLine->GetPointIds()->SetId(3,(i+1)*4+j);
      polyLine->GetPointIds()->SetId(4,i*4+j);
      planeCells->InsertNextCell(polyLine);
      }
    }

  this->ControlPolygonPolyData->SetPoints(planePoints);
  this->ControlPolygonPolyData->SetPolys(planeCells);

  for(int i=0;i<6; i++)
    {
    this->InitialBounds[i] = bounds[i];
    }

  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  this->BezierSurfaceSource->SetControlPoints(this->ControlPolygonPolyData->GetPoints());

  this->PositionHandles();
  this->SizeHandles();
  this->SizeControlPolygon();

  if (this->Interactor)
    {
    this->Interactor->Render();
    }
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::RegisterPickers()
{
  this->Interactor->GetPickingManager()
    ->AddPicker(this->HandlePicker.GetPointer(), this);

  this->Interactor->GetPickingManager()
    ->AddPicker(this->ControlPolygonPicker.GetPointer(), this);
}

//------------------------------------------------------------------------------
unsigned int vtkBezierSurfaceWidget::GetSurfaceResolutionX() const
{
  return this->SurfaceResolutionX;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SetSurfaceResolutionX(unsigned int resolutionX)
{
  this->SurfaceResolutionX = resolutionX;
  this->BezierSurfaceSource->SetResolution(this->SurfaceResolutionX,
                                           this->SurfaceResolutionY);
  this->BezierSurfaceSource->Update();
}

//------------------------------------------------------------------------------
unsigned int vtkBezierSurfaceWidget::GetSurfaceResolutionY() const
{
  return this->SurfaceResolutionY;
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::SetSurfaceResolutionY(unsigned int resolutionY)
{
  this->SurfaceResolutionY = resolutionY;
  this->BezierSurfaceSource->SetResolution(this->SurfaceResolutionX,
                                           this->SurfaceResolutionY);
  this->BezierSurfaceSource->Update();
}

//------------------------------------------------------------------------------
vtkPolyData* vtkBezierSurfaceWidget::GetTubedWidgetPolyData() const
{
  this->TubeFilter->Update();
  return this->TubeFilter->GetOutput();
}

//------------------------------------------------------------------------------
vtkPoints* vtkBezierSurfaceWidget::GetControlPoints() const
{
  return this->ControlPolygonPolyData->GetPoints();
}

//------------------------------------------------------------------------------
vtkPolyData *
vtkBezierSurfaceWidget::GetBezierSurfacePolyData() const
{
  this->BezierSurfaceSource->Update();
  return this->BezierSurfaceSource->GetOutput();
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ComputeNormalsOn()
{
  this->ComputeNormalsFlag = true;
  this->Normals->SetInputConnection(this->BezierSurfaceSource->GetOutputPort());
  this->BezierSurfaceMapper->SetInputConnection(this->Normals->GetOutputPort());
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ComputeNormalsOff()
{
  this->ComputeNormalsFlag = false;
  this->BezierSurfaceMapper->
    SetInputConnection(this->BezierSurfaceSource->GetOutputPort());
}

//------------------------------------------------------------------------------
void vtkBezierSurfaceWidget::ComputeNormals(bool computeNormals)
{
  if (computeNormals)
    {
    this->ComputeNormalsOn();
    }
  else
    {
    this->ComputeNormalsOff();
    }
}
