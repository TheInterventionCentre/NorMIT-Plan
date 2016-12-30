/*===============================================================================

  Project: Slicer-LiverResectionPlanning
  Module: vtkLineWidget3.cxx

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is proprietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/

#include "vtkLineWidget3.h"
#include "vtkLineRepresentation3.h"

// VTK includes
#include <vtkPointHandleRepresentation3D.h>
#include <vtkHandleWidget.h>
#include <vtkCommand.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkWidgetEventTranslator.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkEvent.h>
#include <vtkWidgetEvent.h>
#include <vtkRenderWindow.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkLineWidget3);

//------------------------------------------------------------------------------
vtkLineWidget3::vtkLineWidget3()
{ 
  this->WidgetState = vtkLineWidget3::Start;
  this->ManagesCursor = 1;
  this->CurrentHandle = 0;
  this->Point1Interaction = false;
  this->Point2Interaction = true;
  this->LineInteraction = true;
  this->ScaleInteraction = false;
  this->TranslateInteraction = true;

  // The widgets for moving the end points. They observe this widget (i.e.,
  // this widget is the parent to the handles).

  this->Point1Widget = vtkHandleWidget::New();
  this->Point1Widget->SetPriority(this->Priority-0.01);
  this->Point1Widget->SetParent(this);
  this->Point1Widget->ManagesCursorOff();
  this->Point1Widget->ProcessEventsOff(); // do not handle events

  this->Point2Widget = vtkHandleWidget::New();
  this->Point2Widget->SetPriority(this->Priority-0.01);
  this->Point2Widget->SetParent(this);
  this->Point2Widget->ManagesCursorOff();

  this->LineHandle = vtkHandleWidget::New();
  this->LineHandle->SetPriority(this->Priority-0.01);
  this->LineHandle->SetParent(this);
  this->LineHandle->ManagesCursorOff();

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkWidgetEvent::Select,
                                          this, vtkLineWidget3::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                          vtkWidgetEvent::EndSelect,
                                          this, vtkLineWidget3::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
                                          vtkWidgetEvent::Translate,
                                          this, vtkLineWidget3::TranslateAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent,
                                          vtkWidgetEvent::EndTranslate,
                                          this, vtkLineWidget3::EndSelectAction);
  // disable scaling with right click
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
                                          vtkWidgetEvent::Scale,
                                          this, vtkLineWidget3::ScaleAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
                                          vtkWidgetEvent::EndScale,
                                          this, vtkLineWidget3::EndSelectAction);

  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this, vtkLineWidget3::MoveAction);
}

//------------------------------------------------------------------------------
vtkLineWidget3::~vtkLineWidget3()
{
  this->Point1Widget->Delete();
  this->Point2Widget->Delete();
  this->LineHandle->Delete();
}

//------------------------------------------------------------------------------
void vtkLineWidget3::SetEnabled(int enabling)
{
  int enabled = this->Enabled;

  // We do this step first because it sets the CurrentRenderer
  this->Superclass::SetEnabled(enabling);

  // We defer enabling the handles until the selection process begins
  if ( enabling && !enabled )
  {
    // Don't actually turn these on until cursor is near the end points or the line.
    this->CreateDefaultRepresentation();

    this->Point1Widget->SetRepresentation(reinterpret_cast<vtkLineRepresentation3*>
                                         (this->WidgetRep)->GetPoint1Representation());
    this->Point1Widget->SetInteractor(this->Interactor);
    this->Point1Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

    //vtkLineRepresentation* lineRep = vtkLineRepresentation::SafeDownCast(this->WidgetRep);
    //lineRep->GetLineProperty()->SetColor(255.0,0.0.,0.0);
    //lineRep->GetEndPointProperty()->SetColor(255,0,0);

    //this->Point1Widget->GetRepresentation()->SetVisibility(false);
    //this->Point1Widget->EnabledOff();
    //this->Point1Widget->Off();
    //this->Point1Widget->GetHandleRepresentation()->SetHandleSize(0.0);
    //this->Point1Widget->GetHandleRepresentation()->SetVisibility(0);
    //this->Point1Widget->Render();

    std::cout << "Handle size " << this->Point1Widget->GetHandleRepresentation()->GetHandleSize() << std::endl;
    std::cout << "Handle visibility " << this->Point1Widget->GetHandleRepresentation()->GetVisibility() << std::endl;

    this->Point2Widget->SetRepresentation(reinterpret_cast<vtkLineRepresentation3*>
                                          (this->WidgetRep)->GetPoint2Representation());
    this->Point2Widget->SetInteractor(this->Interactor);
    this->Point2Widget->GetRepresentation()->SetRenderer(this->CurrentRenderer);

    this->LineHandle->SetRepresentation(reinterpret_cast<vtkLineRepresentation3*>
                                          (this->WidgetRep)->GetLineHandleRepresentation());
    this->LineHandle->SetInteractor(this->Interactor);
    this->LineHandle->GetRepresentation()->SetRenderer(this->CurrentRenderer);

  }
  else if ( !enabling && enabled )
  {
    this->Point1Widget->SetEnabled(0);
    this->Point2Widget->SetEnabled(0);
    this->LineHandle->SetEnabled(0);
  }
}

//------------------------------------------------------------------------------
void vtkLineWidget3::SelectAction(vtkAbstractWidget *widget)
{
  vtkLineWidget3 *self = reinterpret_cast<vtkLineWidget3*>(widget);
  if ( self->WidgetRep->GetInteractionState() == vtkLineRepresentation3::Outside )
  {
    return;
  }

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We are definitely selected
  self->WidgetState = vtkLineWidget3::Active;
  self->GrabFocus(self->EventCallbackCommand);
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
    StartWidgetInteraction(e);
  self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL); //for the handles
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
  self->EventCallbackCommand->SetAbortFlag(1);
}

//------------------------------------------------------------------------------
void vtkLineWidget3::TranslateAction(vtkAbstractWidget *widget)
{
  vtkLineWidget3 *self = reinterpret_cast<vtkLineWidget3*>(widget);
  if ( self->WidgetRep->GetInteractionState() == vtkLineRepresentation3::Outside )
  {
    return;
  }

  // if no translating interaction then return
  if (self->TranslateInteraction == false)
  {
      return;
  }

  // Modify the state, we are selected
  int state = self->WidgetRep->GetInteractionState();
  if ( state == vtkLineRepresentation3::OnP1 )
  {
    //reinterpret_cast<vtkLineRepresentation*>(self->WidgetRep)->
      //SetInteractionState(vtkLineRepresentation::TranslatingP1);
    reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
          SetInteractionState(vtkLineRepresentation3::Outside);
  }
  if ( state == vtkLineRepresentation3::OnP2 )
  {
    reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
      SetInteractionState(vtkLineRepresentation3::TranslatingP2);
  }
  else
  {
    reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
      SetInteractionState(vtkLineRepresentation3::OnLine);
  }

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We are definitely selected
  self->WidgetState = vtkLineWidget3::Active;
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
    StartWidgetInteraction(eventPos);
  self->InvokeEvent(vtkCommand::LeftButtonPressEvent,NULL); //for the handles
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
}

//------------------------------------------------------------------------------
void vtkLineWidget3::ScaleAction(vtkAbstractWidget *widget)
{

  vtkLineWidget3 *self = reinterpret_cast<vtkLineWidget3*>(widget);
  if ( self->WidgetRep->GetInteractionState() == vtkLineRepresentation3::Outside )
  {
    return;
  }

  // return if no scaling interaction
  if (self->ScaleInteraction == false)
  {
      return;
  }

  reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
    SetInteractionState(vtkLineRepresentation3::Scaling);
  self->Interactor->Disable();
  self->LineHandle->SetEnabled(0);
  self->Interactor->Enable();

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We are definitely selected
  self->WidgetState = vtkLineWidget3::Active;
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->
    StartWidgetInteraction(eventPos);
  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,NULL);
}

//------------------------------------------------------------------------------
void vtkLineWidget3::MoveAction(vtkAbstractWidget *widget)
{
  vtkLineWidget3 *self = reinterpret_cast<vtkLineWidget3*>(widget);
  // compute some info we need for all cases
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // See whether we're active
  if ( self->WidgetState == vtkLineWidget3::Start )
    {
    self->Interactor->Disable(); //avoid extra renders
    //self->Point1Widget->SetEnabled(0);
    self->Point2Widget->SetEnabled(0);
    self->LineHandle->SetEnabled(0);

    int oldState = self->WidgetRep->GetInteractionState();
    int state = self->WidgetRep->ComputeInteractionState(X,Y);
    int changed;
    // Determine if we are near the end points or the line
    if ( state == vtkLineRepresentation3::Outside )
    {
      changed = self->RequestCursorShape(VTK_CURSOR_DEFAULT);
    }
    else //must be near something
    {
      changed = self->RequestCursorShape(VTK_CURSOR_HAND);
      if ( state == vtkLineRepresentation3::OnP1 && self->Point1Interaction ) // if can interact with point1
      {
        self->Point1Widget->SetEnabled(0);
      }
      if ( state == vtkLineRepresentation3::OnP2 && self->Point2Interaction ) // if can interact with point2
      {
        self->Point2Widget->SetEnabled(1);
      }
      else if( self->LineInteraction ) //if ( state == vtkLineRepresentation::OnLine ) // if can interact with line
      {
        self->LineHandle->SetEnabled(1);
        changed = 1; //movement along the line always needs render
      }
    }
    self->Interactor->Enable(); //avoid extra renders
    if ( changed || oldState != state )
    {
      self->Render();
    }
  }
  else //if ( self->WidgetState == vtkLineWidget3::Active )
    {
    // moving something
    double e[2];
    e[0] = static_cast<double>(X);
    e[1] = static_cast<double>(Y);
    self->InvokeEvent(vtkCommand::MouseMoveEvent,NULL); //handles observe this
    reinterpret_cast<vtkLineRepresentation3*>(self->WidgetRep)->WidgetInteraction(e);
    self->InvokeEvent(vtkCommand::InteractionEvent,NULL);
    self->EventCallbackCommand->SetAbortFlag(1);
    self->Render();
    }
}

//------------------------------------------------------------------------------
void vtkLineWidget3::EndSelectAction(vtkAbstractWidget *widget)
{
  vtkLineWidget3 *self = reinterpret_cast<vtkLineWidget3*>(widget);
  if ( self->WidgetState == vtkLineWidget3::Start )
  {
    return;
  }

  // Return state to not active
  self->WidgetState = vtkLineWidget3::Start;
  self->ReleaseFocus();
  self->InvokeEvent(vtkCommand::LeftButtonReleaseEvent,NULL); //handles observe this
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::EndInteractionEvent,NULL);
  self->Superclass::EndInteraction();
  self->Render();
}

//----------------------------------------------------------------------
void vtkLineWidget3::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
  {
    this->WidgetRep = vtkLineRepresentation3::New();
  }
}

//--------------------------------------------------------------------------------
void vtkLineWidget3::SetProcessEvents(int pe)
{
  this->Superclass::SetProcessEvents(pe);

  //this->Point1Widget->SetProcessEvents(pe);
  this->Point2Widget->SetProcessEvents(pe);
  this->LineHandle->SetProcessEvents(pe);
}

//--------------------------------------------------------------------------------
void vtkLineWidget3::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
