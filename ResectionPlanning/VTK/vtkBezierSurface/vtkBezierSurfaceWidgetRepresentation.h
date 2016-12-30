/*===============================================================================

  Project: vtkBezierSurface
  Module: vtkBezierSurfaceWidgetRepresentation.h

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is propietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/
// .NAME vtkBezierSurfaceWidgetRepresentation - a class defining the representation for a vtkImplicitPlaneWidget2
// .SECTION Description
// This class is a concrete representation for the
// vtkImplicitPlaneWidget2. It represents an infinite plane defined by a
// normal and point in the context of a bounding box. Through interaction
// with the widget, the plane can be manipulated by adjusting the plane
// normal or moving the origin point.
//
// To use this representation, you normally define a (plane) origin and (plane)
// normal. The PlaceWidget() method is also used to initially position the
// representation.

// .SECTION Caveats
// This class, and vtkImplicitPlaneWidget2, are next generation VTK
// widgets. An earlier version of this functionality was defined in the
// class vtkImplicitPlaneWidget.

// .SECTION See Also
// vtkImplicitPlaneWidget2 vtkImplicitPlaneWidget

#ifndef vtkBezierSurfaceWidgetRepresentation_h
#define vtkBezierSurfaceWidgetRepresentation_h

// This module includes
#include "vtkWidgetRepresentation.h"
#include "vtkBezierSurfaceSource.h"

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

//-------------------------------------------------------------------------------
class vtkActor;
class vtkPolyDataMapper;
class vtkCellPicker;
class vtkConeSource;
class vtkLineSource;
class vtkSphereSource;
class vtkTubeFilter;
class vtkPlane;
class vtkCutter;
class vtkProperty;
class vtkImageData;
class vtkOutlineFilter;
class vtkFeatureEdges;
class vtkPolyData;
class vtkPolyDataAlgorithm;
class vtkTransform;
class vtkBox;
class vtkLookupTable;
class vtkBezierSurfaceSource;
class vtkCollection;
class vtkPoints;

//-------------------------------------------------------------------------------
class vtkBezierSurfaceWidgetRepresentation : public vtkWidgetRepresentation
{
 public:
  // Description:
  // Instantiate the class.
  static vtkBezierSurfaceWidgetRepresentation *New();

  // Description:
  // Standard methods for the class.
  vtkTypeMacro(vtkBezierSurfaceWidgetRepresentation,vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Set/Get Bezier surface
  void SetBezierSurfaceSource(vtkBezierSurfaceSource *source);
  vtkSmartPointer<vtkBezierSurfaceSource> GetBezierSurfaceSource() const
    {return this->BezierSource;}

  // Description:
  // Turn on/off the ability to scale the widget with the mouse.
  vtkSetMacro(ScaleEnabled,int);
  vtkGetMacro(ScaleEnabled,int);
  vtkBooleanMacro(ScaleEnabled,int);

  // Description:
  // Methods related to interaction
  virtual int ComputeInteractionState(int X, int Y, int modify=0);
  virtual int ComputeMultiInteractionState(int X, int Y, int modify=0);
  virtual void StartWidgetInteraction(double eventPos[2]);
  virtual void WidgetInteraction(double newEventPos[2]);
  virtual void EndWidgetInteraction(double newEventPos[2]);

  // Description:
  // Methods related to widget construction and placement
  virtual void PlaceWidget(double bounds[6]);
  virtual void BuildRepresentation();
  virtual double *GetBounds();

  // Decsription:
  // Methods supporting the rendering process.
  virtual void GetActors(vtkPropCollection *pc);
  virtual void ReleaseGraphicsResources(vtkWindow*);
  virtual int RenderOpaqueGeometry(vtkViewport*);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport*);
  virtual int HasTranslucentPolygonalGeometry();

  // Description:
  // Methods enabling/disabling visibility
  void ShowControlPoints();
  void HideControlPoints();
  void ShowControlPolygon();
  void HideControlPolygon();

  // Description:
  // Methods for enabling/disabling moving interaction
  vtkGetMacro(MovingInteraction, bool);
  vtkSetMacro(MovingInteraction, bool);
  vtkBooleanMacro(MovingInteraction, bool);

  //BTX - manage the state of the widget
  enum _InteractionState
  {
    Outside=0,
    Moving,
    MovingOutline,
    MovingOrigin,
    Rotating,
    Pushing,
    Scaling,
    Deforming,
    MultiDeforming
  };
  //ETX

  // Description:
  // The interaction state may be set from a widget (e.g.,
  // vtkImplicitPlaneWidget2) or other object. This controls how the
  // interaction with the widget proceeds. Normally this method is used as
  // part of a handshaking process with the widget: First
  // ComputeInteractionState() is invoked that returns a state based on
  // geometric considerations (i.e., cursor near a widget feature), then
  // based on events, the widget may modify this further.
  vtkSetClampMacro(InteractionState,int,Outside,MultiDeforming);

  // Description:
  // Sets the visual appearance of the representation based on the
  // state it is in. This state is usually the same as InteractionState.
  virtual void SetRepresentationState(int);
  vtkGetMacro(RepresentationState, int);

  // Description:
  // Sets/Gets the HandleSize-to-TubeRadius factor
  vtkSetMacro(TubeSizeFactor, double);
  vtkGetMacro(TubeSizeFactor, double);

 protected:
  vtkBezierSurfaceWidgetRepresentation();
  ~vtkBezierSurfaceWidgetRepresentation();

  void InitializeWidget();

  int RepresentationState;

  // Keep track of event positions
  double LastEventPosition[3];

  double TubeSizeFactor;

  vtkNew<vtkCollection> ControlPoints;
  vtkNew<vtkCollection> ControlPointsMappers;
  vtkNew<vtkCollection> ControlPointsActors;
  vtkNew<vtkPolyData>   ControlPolygon;
  vtkNew<vtkPolyDataMapper> ControlPolygonMapper;
  vtkNew<vtkActor> ControlPolygonActor;
  vtkIdType LastHandleInteracted;
  void HighlightHandle(int highlight, vtkIdType id);
  void HighlightControlPolygon(int highlight);

  int  OutlineTranslation; //whether the outline can be moved
  int  ScaleEnabled; //whether the widget can be scaled
  int  OutsideBounds; //whether the widget can be moved outside input's bounds
  float HandlesScaleFactor;
  // Do the picking
  vtkNew<vtkCellPicker> Picker;

  // Register internal Pickers within PickingManager
  virtual void RegisterPickers();

  void CreateDefaultProperties();
  void SizeHandles();
  void Deform(double *endPoint);
  void MultiDeform(double *originPoint, double *endPoint);
  void Move(double *originPoint, double *endPoints);

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkNew<vtkProperty> HandlesProperty;
  vtkNew<vtkProperty> SelectedHandlesProperty;
  vtkNew<vtkProperty> ControlPolygonProperty;
  vtkNew<vtkProperty> SelectedControlPolygonProperty;

  // Support GetBounds() method
  vtkNew<vtkBox> BoundingBox;
  vtkNew<vtkTubeFilter> Tuber;

  // Description:
  // Bezier surface source
  vtkSmartPointer<vtkBezierSurfaceSource> BezierSource;

  bool MovingInteraction;

 private:
  vtkBezierSurfaceWidgetRepresentation(const vtkBezierSurfaceWidgetRepresentation&);  //Not implemented
  void operator=(const vtkBezierSurfaceWidgetRepresentation&);  //Not implemented
};

#endif
