/*===============================================================================

  Project: Slicer-LiverResectionPlanning
  Module: vtkLineWidget3.h

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is proprietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/

#ifndef vtkLineWidget3_h
#define vtkLineWidget3_h

// VTK includes
#include <vtkInteractionWidgetsModule.h> // For export macro
#include <vtkAbstractWidget.h>
#include <vtkNew.h>

//-------------------------------------------------------------------------------
class vtkLineRepresentation3;
class vtkHandleWidget;

//-------------------------------------------------------------------------------
class VTKINTERACTIONWIDGETS_EXPORT vtkLineWidget3 : public vtkAbstractWidget
{
public:
  // Description:
  // Instantiate the object.
  static vtkLineWidget3 *New();

  // Description:
  // Standard vtkObject methods
  vtkTypeMacro(vtkLineWidget3,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Override superclasses' SetEnabled() method because the line
  // widget must enable its internal handle widgets.
  virtual void SetEnabled(int enabling);

  // Description:
  // Specify an instance of vtkWidgetRepresentation used to represent this
  // widget in the scene. Note that the representation is a subclass of vtkProp
  // so it can be added to the renderer independent of the widget.
  void SetRepresentation(vtkLineRepresentation3 *r)
    {this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));}

  // Description:
  // Return the representation as a vtkLineRepresentation.
  vtkLineRepresentation3 *GetLineRepresentation()
    {return reinterpret_cast<vtkLineRepresentation3*>(this->WidgetRep);}

  // Description:
  // Create the default widget representation if one is not set.
  void CreateDefaultRepresentation();

  // Description:
  // Methods to change the whether the widget responds to interaction.
  // Overridden to pass the state to component widgets.
  virtual void SetProcessEvents(int);

  //Description
  // Methods to change enable/disable the interaction elements
  vtkGetMacro(Point1Interaction, bool);
  vtkSetMacro(Point1Interaction, bool);
  vtkBooleanMacro(Point1Interaction, bool);

  vtkGetMacro(Point2Interaction, bool);
  vtkSetMacro(Point2Interaction, bool);
  vtkBooleanMacro(Point2Interaction, bool);

  vtkGetMacro(LineInteraction, bool);
  vtkSetMacro(LineInteraction, bool);
  vtkBooleanMacro(LineInteraction, bool);

  vtkGetMacro(ScaleInteraction, bool);
  vtkSetMacro(ScaleInteraction, bool);
  vtkBooleanMacro(ScaleInteraction, bool);

  vtkGetMacro(TranslateInteraction, bool);
  vtkSetMacro(TranslateInteraction, bool);
  vtkBooleanMacro(TranslateInteraction, bool);

protected:
  vtkLineWidget3();
  ~vtkLineWidget3();

//BTX - manage the state of the widget
  int WidgetState;
  enum _WidgetState {Start=0,Active};
//ETX
  int CurrentHandle;

  // These methods handle events
  static void SelectAction(vtkAbstractWidget*);
  static void TranslateAction(vtkAbstractWidget*);
  static void ScaleAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);
  static void MoveAction(vtkAbstractWidget*);

  // The positioning handle widgets
  vtkHandleWidget *Point1Widget; //first end point
  vtkHandleWidget *Point2Widget; //second end point
  vtkHandleWidget *LineHandle; //used when selecting the line

  //Interaction flags
  bool Point1Interaction;
  bool Point2Interaction;
  bool LineInteraction;
  bool ScaleInteraction;
  bool TranslateInteraction;

private:
  vtkLineWidget3(const vtkLineWidget3&) VTK_DELETE_FUNCTION;
  void operator=(const vtkLineWidget3&) VTK_DELETE_FUNCTION;
};

#endif
