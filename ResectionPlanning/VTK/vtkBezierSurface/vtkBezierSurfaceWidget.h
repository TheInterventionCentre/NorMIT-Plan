/*===============================================================================

  Project: vtkBezierSurface
  Module: vtkBezierSurfaceWidget.h

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is propietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/
// .NAME vtkBezierSurfaceWidget - 3D widget for manipulating an infinite plane
// .SECTION Description
// This 3D widget defines an infinite plane that can be interactively placed
// in a scene. The widget is assumed to consist of four parts: 1) a plane
// contained in a 2) bounding box, with a 3) plane normal, which is rooted
// at a 4) point on the plane. (The representation paired with this widget
// determines the actual geometry of the widget.)
//
// To use this widget, you generally pair it with a vtkBezierSurfaceWidgetRepresentation
// (or a subclass). Variuos options are available for controlling how the
// representation appears, and how the widget functions.

// .SECTION Event Bindings
// By default, the widget responds to the following VTK events (i.e., it
// watches the vtkRenderWindowInteractor for these events):
// <pre>
// If the plane normal is selected:
//   LeftButtonPressEvent - select normal
//   LeftButtonReleaseEvent - release normal
//   MouseMoveEvent - orient the normal vector
// If the origin point is selected:
//   LeftButtonPressEvent - select slider (if on slider)
//   LeftButtonReleaseEvent - release slider (if selected)
//   MouseMoveEvent - move the origin point (constrained to the plane)
// If the plane is selected:
//   LeftButtonPressEvent - select slider (if on slider)
//   LeftButtonReleaseEvent - release slider (if selected)
//   MouseMoveEvent - move the plane
// If the outline is selected:
//   LeftButtonPressEvent - select slider (if on slider)
//   LeftButtonReleaseEvent - release slider (if selected)
//   MouseMoveEvent - move the outline
// If the keypress characters are used
//   'Down/Left' Move plane down
//   'Up/Right' Move plane up
// In all the cases, independent of what is picked, the widget responds to the
// following VTK events:
//   MiddleButtonPressEvent - move the plane
//   MiddleButtonReleaseEvent - release the plane
//   RightButtonPressEvent - scale the widget's representation
//   RightButtonReleaseEvent - stop scaling the widget
//   MouseMoveEvent - scale (if right button) or move (if middle button) the widget
// </pre>
//
// Note that the event bindings described above can be changed using this
// class's vtkWidgetEventTranslator. This class translates VTK events
// into the vtkBezierSurfaceWidget's widget events:
// <pre>
//   vtkWidgetEvent::Select -- some part of the widget has been selected
//   vtkWidgetEvent::EndSelect -- the selection process has completed
//   vtkWidgetEvent::Move -- a request for slider motion has been invoked
//   vtkWidgetEvent::Up and vtkWidgetEvent::Down -- MovePlaneAction
// </pre>
//
// In turn, when these widget events are processed, the vtkBezierSurfaceWidget
// invokes the following VTK events on itself (which observers can listen for):
// <pre>
//   vtkCommand::StartInteractionEvent (on vtkWidgetEvent::Select)
//   vtkCommand::EndInteractionEvent (on vtkWidgetEvent::EndSelect)
//   vtkCommand::InteractionEvent (on vtkWidgetEvent::Move)
// </pre>
//

// This class, and vtkBezierSurfaceWidgetRepresentation, are next generation VTK
// widgets. An earlier version of this functionality was defined in the class
// vtkImplicitPlaneWidget.

// .SECTION See Also
// vtk3DWidget vtkBoxWidget vtkPlaneWidget vtkLineWidget vtkPointWidget
// vtkSphereWidget vtkImagePlaneWidget

#ifndef vtkBezierSurfaceWidget_h
#define vtkBezierSurfaceWidget_h

#include "vtkBezierSurfaceSource.h"

// VTK includes
#include <vtkAbstractWidget.h>
#include <vtkSmartPointer.h>
#include <vtkCommand.h>

//----------------------------------------------------------------------------
class vtkBezierSurfaceWidgetRepresentation;
class vtkInteractionCallback;
class vtkPoints;
class vtkInteractionCallback;

//----------------------------------------------------------------------------
class vtkBezierSurfaceWidget : public vtkAbstractWidget
{
  friend class vtkInteractionCallback;

public:
  // Description:
  // Instantiate the object.
  static vtkBezierSurfaceWidget *New();

  // Description:
  // Standard vtkObject methods
  vtkTypeMacro(vtkBezierSurfaceWidget,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation( vtkBezierSurfaceWidgetRepresentation *rep );

  // Descritpion:
  // Disable/Enable the widget if needed.
  // Unobserved the camera if the widget is disabled.
  void SetEnabled(int enabling);

  // Description:
  // Return the representation as a vtkBezierSurfaceWidgetRepresentation.
  vtkBezierSurfaceWidgetRepresentation *GetBezierSurfaceRepresentation()
    {return reinterpret_cast<vtkBezierSurfaceWidgetRepresentation*>(this->WidgetRep);}


  // Description:
  // Create the default widget representation if one is not set.
  void CreateDefaultRepresentation();

  // Description:
  // Mutltinteraction related methods
  vtkGetMacro(MultiInteraction, bool);
  vtkSetMacro(MultiInteraction, bool);
  vtkBooleanMacro(MultiInteraction, bool);

  // Description:
  // Moveinteraction related methods
  vtkGetMacro(MoveInteraction, bool);
  vtkSetMacro(MoveInteraction, bool);
  vtkBooleanMacro(MoveInteraction, bool);

  // Description:
  // Moveinteraction related methods
  vtkGetMacro(TranslationInteraction, bool);
  vtkSetMacro(TranslationInteraction, bool);
  vtkBooleanMacro(TranslationInteraction, bool);

 protected:
  vtkBezierSurfaceWidget();
  ~vtkBezierSurfaceWidget();

//BTX - manage the state of the widget
  int WidgetState;
  enum _WidgetState {Start=0,Active};
//ETX

  // These methods handle events
  static void SelectAction(vtkAbstractWidget*);
  static void MultiSelectAction(vtkAbstractWidget*);
  static void TranslateAction(vtkAbstractWidget*);
  static void ScaleAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);
  static void EndMultiSelectAction(vtkAbstractWidget*);
  static void MoveAction(vtkAbstractWidget*);
  static void MovePlaneAction(vtkAbstractWidget*);

  // Description:
  // Update the cursor shape based on the interaction state. Returns 1
  // if the cursor shape requested is different from the existing one.
  int UpdateCursorShape( int interactionState );

  bool MoveInteraction;
  bool ShowControlPoints;
  bool ShowControlPolygon;
  bool ShowBezierSurface;
  bool MultiInteraction;
  bool TranslationInteraction;

protected:
  vtkBezierSurfaceWidget(const vtkBezierSurfaceWidget&);  //Not implemented
  void operator=(const vtkBezierSurfaceWidget&);  //Not implemented

  // Description:
  // Handle the interaction callback that may come from the representation
  vtkSmartPointer<vtkInteractionCallback> InteractionCallback;
  void InvokeInteractionCallback();

};

//----------------------------------------------------------------------------
class vtkInteractionCallback : public vtkCommand
{
public:
  static vtkInteractionCallback *New()
    { return new vtkInteractionCallback; }
  virtual void Execute(vtkObject*, unsigned long eventId, void*)
    {
      switch (eventId)
        {
        case vtkCommand::ModifiedEvent:
          this->BezierSurfaceWidget->InvokeInteractionCallback();
          break;
        }
    }
  vtkBezierSurfaceWidget *BezierSurfaceWidget;
};

#endif
