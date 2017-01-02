
#ifndef vtkLineWidget3_h
#define vtkLineWidget3_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkAbstractWidget.h"

class vtkLineRepresentation;
class vtkHandleWidget;


class VTKINTERACTIONWIDGETS_EXPORT vtkLineWidget3 : public vtkAbstractWidget
{
public:
  /**
   * Instantiate the object.
   */
  static vtkLineWidget3 *New();

  //@{
  /**
   * Standard vtkObject methods
   */
  vtkTypeMacro(vtkLineWidget3,vtkAbstractWidget);
  void PrintSelf(ostream& os, vtkIndent indent);
  //@}

  /**
   * Override superclasses' SetEnabled() method because the line
   * widget must enable its internal handle widgets.
   */
  virtual void SetEnabled(int enabling);

  /**
   * Specify an instance of vtkWidgetRepresentation used to represent this
   * widget in the scene. Note that the representation is a subclass of vtkProp
   * so it can be added to the renderer independent of the widget.
   */
  void SetRepresentation(vtkLineRepresentation *r)
    {this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));}

  /**
   * Return the representation as a vtkLineRepresentation.
   */
  vtkLineRepresentation *GetLineRepresentation()
    {return reinterpret_cast<vtkLineRepresentation*>(this->WidgetRep);}

  /**
   * Create the default widget representation if one is not set.
   */
  void CreateDefaultRepresentation();

  /**
   * Methods to change the whether the widget responds to interaction.
   * Overridden to pass the state to component widgets.
   */
  virtual void SetProcessEvents(int);

protected:
  vtkLineWidget3();
  ~vtkLineWidget3();

  // Manage the state of the widget
  int WidgetState;
  enum _WidgetState {Start=0,Active};
  int CurrentHandle;

  // These methods handle events
  static void SelectAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);
  static void MoveAction(vtkAbstractWidget*);

  // The positioning handle widgets
  vtkHandleWidget *Point1Widget; //first end point (other end point not used in this version)
  vtkHandleWidget *LineHandle; //used when selecting the line

private:
  vtkLineWidget3(const vtkLineWidget3&) VTK_DELETE_FUNCTION;
  void operator=(const vtkLineWidget3&) VTK_DELETE_FUNCTION;
};

#endif
