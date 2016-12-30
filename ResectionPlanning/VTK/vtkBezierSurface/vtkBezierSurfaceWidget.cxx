/*===============================================================================

  Project: vtkBezierSurface
  Module: vtkBezierSurfaceWidget.cxx

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is propietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/

// This module includes
#include "vtkBezierSurfaceWidget.h"
#include "vtkBezierSurfaceWidgetRepresentation.h"

// VTK includes
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkStdString.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkEvent.h>
#include <vtkWidgetEvent.h>
#include <vtkRenderWindow.h>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkBezierSurfaceWidget);

//----------------------------------------------------------------------------
vtkBezierSurfaceWidget::vtkBezierSurfaceWidget()
  : MoveInteraction(true),
    ShowControlPoints(true),
    ShowControlPolygon(true),
    ShowBezierSurface(true),
    MultiInteraction(true),
    TranslationInteraction(true)
{
  this->WidgetState = vtkBezierSurfaceWidget::Start;

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkWidgetEvent::Select,
                                          this,
                                          vtkBezierSurfaceWidget::SelectAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                          vtkWidgetEvent::EndSelect,
                                          this,
                                          vtkBezierSurfaceWidget::EndSelectAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this,
                                          vtkBezierSurfaceWidget::MoveAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
                                          1000,
                                          this,
                                          vtkBezierSurfaceWidget::MultiSelectAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
                                          vtkWidgetEvent::EndSelect,
                                          this, vtkBezierSurfaceWidget::EndMultiSelectAction);


  this->InteractionCallback =
    vtkSmartPointer<vtkInteractionCallback>::New();
  this->InteractionCallback->BezierSurfaceWidget = this;
}

//----------------------------------------------------------------------------
vtkBezierSurfaceWidget::~vtkBezierSurfaceWidget()
{

}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::SelectAction(vtkAbstractWidget *widget)
{
  vtkBezierSurfaceWidget *self = vtkBezierSurfaceWidget::SafeDownCast(widget);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  vtkBezierSurfaceWidgetRepresentation *representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(self->WidgetRep);

  representation->SetInteractionState(vtkBezierSurfaceWidgetRepresentation::Moving);
  int interactionState = representation->ComputeInteractionState(X, Y);
  self->UpdateCursorShape(interactionState);

  if (self->WidgetRep->GetInteractionState() ==
       vtkBezierSurfaceWidgetRepresentation::Outside)
    {
    return;
    }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetState = vtkBezierSurfaceWidget::Active;
  representation->StartWidgetInteraction(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::MultiSelectAction(vtkAbstractWidget *widget)
{
  vtkBezierSurfaceWidget *self = vtkBezierSurfaceWidget::SafeDownCast(widget);

  if (self->MultiInteraction == false)
    {
      return;
    }

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  vtkBezierSurfaceWidgetRepresentation *representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(self->WidgetRep);
  representation->SetInteractionState(vtkBezierSurfaceWidgetRepresentation::Moving);
  int interactionState = representation->ComputeMultiInteractionState(X, Y);

  self->UpdateCursorShape(interactionState);

  if ( self->WidgetRep->GetInteractionState() ==
       vtkBezierSurfaceWidgetRepresentation::Outside )
    {
    return;
    }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetState = vtkBezierSurfaceWidget::Active;
  representation->StartWidgetInteraction(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::MoveAction(vtkAbstractWidget *w)
{
  vtkBezierSurfaceWidget *self = reinterpret_cast<vtkBezierSurfaceWidget*>(w);

  if (self->MoveInteraction == false)
    {
    return;
    }

// So as to change the cursor shape when the mouse is poised over
  // the widget. Unfortunately, this results in a few extra picks
  // due to the cell picker. However given that its picking planes
  // and the handles/arrows, this should be very quick
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  int changed = 0;

  vtkBezierSurfaceWidgetRepresentation *representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(self->WidgetRep);

  if (self->ManagesCursor && self->WidgetState != vtkBezierSurfaceWidget::Active)
    {
    int oldInteractionState = representation->GetInteractionState();

    representation->SetInteractionState(vtkBezierSurfaceWidgetRepresentation::Moving);

    int state = self->WidgetRep->ComputeInteractionState( X, Y );
    changed = self->UpdateCursorShape(state);

    representation->SetInteractionState(oldInteractionState);
    changed = (changed || state != oldInteractionState) ? 1 : 0;
    }

  // See whether we're active
  if ( self->WidgetState == vtkBezierSurfaceWidget::Start )
    {
    if (changed && self->ManagesCursor)
      {
      self->Render();
      }
    return;
    }

  // Okay, adjust the representation
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  representation->WidgetInteraction(e);

  // moving something
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::EndSelectAction(vtkAbstractWidget *w)
{
  vtkBezierSurfaceWidget *self = reinterpret_cast<vtkBezierSurfaceWidget*>(w);

  if ( self->WidgetState != vtkBezierSurfaceWidget::Active  ||
       self->WidgetRep->GetInteractionState() ==
       vtkBezierSurfaceWidgetRepresentation::Outside )
    {
    return;
    }

  vtkBezierSurfaceWidgetRepresentation *representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(self->WidgetRep);

  // Return state to not selected
  double e[2];
  representation->EndWidgetInteraction(e);
  self->WidgetState = vtkBezierSurfaceWidget::Start;
  self->ReleaseFocus();

  // Update cursor if managed
  self->UpdateCursorShape(representation->GetRepresentationState());

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  self->Render();
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::EndMultiSelectAction(vtkAbstractWidget *w)
{
  vtkBezierSurfaceWidget *self =
    vtkBezierSurfaceWidget::SafeDownCast(w);

  if ( self->WidgetState != vtkBezierSurfaceWidget::Active  ||
       self->WidgetRep->GetInteractionState() ==
       vtkBezierSurfaceWidgetRepresentation::Outside )
    {
    return;
    }

  vtkBezierSurfaceWidgetRepresentation *representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(self->WidgetRep);

  // Return state to not selected
  double e[2];
  representation->EndWidgetInteraction(e);
  self->WidgetState = vtkBezierSurfaceWidget::Start;
  self->ReleaseFocus();

  // Update cursor if managed
  self->UpdateCursorShape(representation->GetRepresentationState());

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  self->Render();
}


//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::SetEnabled(int enabling)
{
  if(this->Enabled == enabling)
    {
    return;
    }

  if(this->GetCurrentRenderer() && !enabling)
    {
    this->GetCurrentRenderer()->
      GetActiveCamera()->RemoveObserver(this->InteractionCallback);
    }

  Superclass::SetEnabled(enabling);
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
    {
    this->WidgetRep = vtkBezierSurfaceWidgetRepresentation::New();
    }
}

//----------------------------------------------------------------------
void vtkBezierSurfaceWidget::
SetRepresentation(vtkBezierSurfaceWidgetRepresentation*rep)
{
  this->Superclass::SetWidgetRepresentation(rep);
 //reinterpret_cast<vtkWidgetRepresentation*>(rep));

}

//----------------------------------------------------------------------
int vtkBezierSurfaceWidget::UpdateCursorShape( int state )
{
  // So as to change the cursor shape when the mouse is poised over
  // the widget.
  if (this->ManagesCursor)
    {
    if (state == vtkBezierSurfaceWidgetRepresentation::Outside)
      {
      return this->RequestCursorShape(VTK_CURSOR_DEFAULT);
      }
    else if (state == vtkBezierSurfaceWidgetRepresentation::MovingOutline)
      {
      return this->RequestCursorShape(VTK_CURSOR_SIZEALL);
      }
    else
      {
      return this->RequestCursorShape(VTK_CURSOR_HAND);
      }
    }

  return 0;
}

//----------------------------------------------------------------------------
void vtkBezierSurfaceWidget::InvokeInteractionCallback()
{
  unsigned long previousMtime;
  vtkBezierSurfaceWidgetRepresentation* representation =
    vtkBezierSurfaceWidgetRepresentation::SafeDownCast(this->WidgetRep);

  previousMtime = representation->GetMTime();


  if(representation->GetMTime() > previousMtime)
    {
    this->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    }
}

//----------------------------------------------------------------------------
void vtkBezierSurfaceWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
