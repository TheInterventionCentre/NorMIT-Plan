/* ====================================================================

  Program: NorMIT-Plan
  Module: vtkLineWidget3.cxx

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
#include "vtkLineWidget3.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCellPicker.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkLineWidget3);

//------------------------------------------------------------------------------
vtkLineWidget3::vtkLineWidget3()
  :HandleSizeFactor(1.1),
   LineSizeFactor(0.2)
{

  // Set the event callback to our process events function
  this->EventCallbackCommand->SetCallback(vtkLineWidget3::ProcessEvents);

  Point1[0] = -0.5; Point1[1] = 0.0; Point1[2] = 0.0;
  Point2[0] =  0.5; Point2[1] = 0.0; Point2[2] = 0.0;

  vtkSmartPointer<vtkLineSource> lineSource =
    vtkSmartPointer<vtkLineSource>::New();
  lineSource->SetPoint1(Point1);
  lineSource->SetPoint2(Point2);
  lineSource->Update();

  this->LinePolyData->SetPoints(lineSource->GetOutput()->GetPoints());
  this->LinePolyData->SetLines(lineSource->GetOutput()->GetLines());

  this->TubeFilter->SetInputData(this->LinePolyData.GetPointer());
  this->TubeFilter->SetRadius(0.002);
  this->TubeFilter->SetNumberOfSides(20);
  this->TubeFilter->Update();

  this->LineMapper->SetInputConnection(TubeFilter->GetOutputPort());
  this->LineActor->SetMapper(this->LineMapper.GetPointer());

  this->Handle1Source->SetCenter(Point1);
  this->Handle2Source->SetCenter(Point2);
  this->Handle1Mapper->SetInputConnection(this->Handle1Source->GetOutputPort());
  this->Handle2Mapper->SetInputConnection(this->Handle2Source->GetOutputPort());
  this->Handle1Actor->SetMapper(this->Handle1Mapper.GetPointer());
  this->Handle2Actor->SetMapper(this->Handle2Mapper.GetPointer());

  // Crate the default visualization properties.
  this->CreateDefaultProperties();

  // Manage the handles picking.
  this->Handle1Picker->AddPickList(this->Handle1Actor.GetPointer());
  this->Handle1Picker->PickFromListOn();
  this->Handle2Picker->AddPickList(this->Handle2Actor.GetPointer());
  this->Handle2Picker->PickFromListOn();

  this->CurrentHandle = NULL;



}

//------------------------------------------------------------------------------
vtkLineWidget3::~vtkLineWidget3()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::PrintSelf(ostream &vtkNotUsed(os),
                               vtkIndent vtkNotUsed(indent))
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::PlaceWidget(double bds[6])
{
  double bounds[6], center[3];
  this->AdjustBounds(bds, bounds, center);

  this->Point1[0] = bds[0];
  this->Point1[1] = (bds[3]+bds[2])/2.0;
  this->Point1[2] = (bds[5]-bds[4])/2.0;

  this->Handle1Source->SetCenter(Point1);
  this->Handle1Source->Update();

  this->Point2[0] = bds[2];
  this->Point2[1] = (bds[3]+bds[2])/2.0;
  this->Point2[2] = (bds[5]-bds[4])/2.0;

  this->Handle2Source->SetCenter(Point2);
  this->Handle2Source->Update();

  for(int i=0; i<6; ++i)
    {
    this->InitialBounds[i] = bounds[i];
    }

  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));

  this->SizeHandles();
  this->SizeLine();
}

//------------------------------------------------------------------------------
void vtkLineWidget3::PlaceWidget()
{
}

//------------------------------------------------------------------------------
void vtkLineWidget3::PlaceWidget(double xmin, double xmax,
                                 double ymin, double ymax,
                                 double zmin, double zmax)
{
  double bounds[6] = {xmin, xmax, ymin, ymax, zmin, zmax};
  this->PlaceWidget(bounds);
}

//------------------------------------------------------------------------------
void vtkLineWidget3::SizeHandles()
{
  double radius = this->vtk3DWidget::SizeHandles(this->HandleSizeFactor);

  this->Handle1Source->SetRadius(radius);
  this->Handle1Source->Update();
  this->Handle2Source->SetRadius(radius);
  this->Handle2Source->Update();
}

//------------------------------------------------------------------------------
void vtkLineWidget3::SizeLine()
{
  double radius = this->vtk3DWidget::SizeHandles(this->LineSizeFactor);
  this->TubeFilter->SetRadius(radius);
}


//------------------------------------------------------------------------------
void vtkLineWidget3::CreateDefaultProperties()
{
  this->Handle1Property->SetColor(1.0, 1.0, 1.0);
  this->SelectedHandle1Property->SetColor(1.0, 0.0, 0.0);

  this->Handle2Property->SetColor(1.0, 1.0, 1.0);
  this->SelectedHandle2Property->SetColor(1.0, 0.0, 0.0);

  this->LineProperty->SetColor(1.0, 0.0, 0.0);
  this->LineProperty->LightingOff();
}

//------------------------------------------------------------------------------
void vtkLineWidget3::SetEnabled(int enabling)
{

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

    this->Interactor->AddObserver(vtkCommand::MiddleButtonPressEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::RightButtonReleaseEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    this->Interactor->AddObserver(vtkCommand::MiddleButtonReleaseEvent,
                                  this->EventCallbackCommand,
                                  this->Priority);

    // Add handle1 to scene
    this->Handle1Actor->SetProperty(this->Handle1Property.GetPointer());
    this->CurrentRenderer->AddActor(this->Handle1Actor.GetPointer());

    // Add handle2 to scene
    this->Handle2Actor->SetProperty(this->Handle2Property.GetPointer());
    this->CurrentRenderer->AddActor(this->Handle2Actor.GetPointer());

    // Add line to scene
    this->LineActor->SetProperty(this->LineProperty.GetPointer());
    this->CurrentRenderer->AddActor(this->LineActor.GetPointer());

    //Enable event
    this->InvokeEvent(vtkCommand::EnableEvent, NULL);
    }
  else
    {
    vtkDebugMacro(<<"Disabling widget");

    if(!this->Enabled)
      {
      return;
      }

    this->Enabled = 0;

    // Remove interaction callback
    this->Interactor->RemoveObserver(this->EventCallbackCommand);

    // Remove handles from scene
    this->CurrentRenderer->RemoveActor(this->Handle1Actor.GetPointer());
    this->CurrentRenderer->RemoveActor(this->Handle1Actor.GetPointer());

    this->CurrentHandle = NULL;
    this->InvokeEvent(vtkCommand::DisableEvent, NULL);
    this->SetCurrentRenderer(NULL);
    }

  this->Interactor->Render();
 }
}

//------------------------------------------------------------------------------
void vtkLineWidget3::ProcessEvents(vtkObject *vtkNotUsed(object),
                                           unsigned long event,
                                           void *clientData,
                                           void *vtkNotUsed(callData))
{
  vtkLineWidget3 *self =
    reinterpret_cast<vtkLineWidget3*>(clientData);

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

    case vtkCommand::MiddleButtonPressEvent:
      self->OnMiddleButtonDown();
      break;

    case vtkCommand::MiddleButtonReleaseEvent:
      self->OnMiddleButtonUp();
      break;

    case vtkCommand::MouseMoveEvent:
      self->OnMouseMove();
      break;
    }
}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnMouseMove()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnLeftButtonDown()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnLeftButtonUp()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnRightButtonDown()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnRightButtonUp()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnMiddleButtonDown()
{

}

//------------------------------------------------------------------------------
void vtkLineWidget3::OnMiddleButtonUp()
{

}
