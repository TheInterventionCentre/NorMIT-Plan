/*=========================================================================

Program: NorMIT-Nav
Module: vtkBezierSurfaceWidget.h

Copyright (c) 2017 The Intervention Centre, Oslo University Hospital

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

// .NAME vtkBezierSurfaceWidget - Interactive (deformable) Bézier Surface.
// .SECTION Description
// This 3D widget generates a Bézier surface (4x4 control points) which can be
// deformed an translated by interaction on a net of control
// points. Manipulation of control points occurs by direct interaction with
// handles ether individually or by groups (right click); groups are defined so
// any right-click manipulation of any of the interior points will produce
// manipulation of all interior points (the same holds for exterior
// points). Additionally, the whole widget/surface can be moved by left-clicking
// and dragging the control polygon. Initially, the Bézier surface lies in a
// plane of (1x1 units) centered at (0,0,0).
//
// As in other 3D widgets, this widgets requres invoking the SetInteractor()
// method where the vtkRenderWindowInteractor must be provided. It is also
// possible to call the PlaceWidget() method in order to place the widget by
// defining a bounding box.
//
// Visualization of the different elements is possible through the
// HandleOn/Off(), ControlPolygonOn/Off(), and BezierSurfaceOn/Off(). In
// addition, visualization properties of these geometric elements can be set by
// using SetHandleProperty(), SetSelectedHandleProperty(),
// SetControlPolygonProperty(), SetSelectedControlPolygonProperty() and
// SetBezierSurfaceProperty().
//
// The implementation of this widget is based on vtkBoxWidget.
//
// .SECTION See also
// vtk3DWidget vtkBoxWidget

#ifndef vtkBezierSurfaceWidget_h
#define vtkBezierSurfaceWidget_h

#include <vtk3DWidget.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>
#include <vtkNew.h>
#include <vtkProperty.h>
#include <vtkBezierSurfaceSource.h>


//------------------------------------------------------------------------------
class vtkActor;
class vtkPolyData;
class vtkCollection;
class vtkProp;
class vtkPolyDataMapper;
class vtkCellPicker;
class vtkTubeFilter;

//------------------------------------------------------------------------------
class vtkBezierSurfaceWidget: public vtk3DWidget
{
 public:
  // Description:
  // Instantiation of object.
  static vtkBezierSurfaceWidget *New();

  vtkTypeMacro(vtkBezierSurfaceWidget, vtk3DWidget);

  // Description:
  // Print properties of the object.
  void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Enable/Disable the widget
  virtual void SetEnabled(int);

  // Description:
  // Methods for placing the widget.
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
  {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax,
                   double ymin, double ymax,
                   double zmin, double zmax)
  {this->Superclass::PlaceWidget(xmin, xmax, ymin, ymax, zmin, zmax);}

  // Description:
  // Set/Get the MultiPointInteraction flag. If the flag is != 0 (enabled) then
  // multi-point interaction is enabled. MultiPointInteraction is enabled by
  // default.
  vtkGetMacro(MultiPointInteraction, int);
  vtkSetMacro(MultiPointInteraction, int);
  vtkBooleanMacro(MultiPointInteraction, int);

  // Description:
  // Set/Get the TranslationInteraction flag. If the flag is != 0 (enabled) then
  // the global translation of the control polygon and the surface is
  // enabled. Translation interaction is enabled by default.
  vtkGetMacro(TranslationInteraction, int);
  vtkSetMacro(TranslationInteraction, int);
  vtkBooleanMacro(TranslationInteraction, int);

  // Description:
  // Set/Get the ContinuousBezierUpdate flag. If the flag is !=0 (enabled) then
  // the update of the BézierSurface happens in a continuous way, this is,
  // while the mouse is moving, rather than when the user releases the mouse (at
  // the end of the interaction). Continuous Bézier interaction is enabled by default.
  vtkGetMacro(ContinuousBezierUpdate, int);
  vtkSetMacro(ContinuousBezierUpdate, int);
  vtkBooleanMacro(ContinuousBezierUpdate, int);

  // Description:
  // Set/Get the visual properties of the handle.
  vtkGetObjectMacro(HandleProperty, vtkProperty);
  vtkSetObjectMacro(HandleProperty, vtkProperty);

  // Description:
  // Set/Get the visual properties of the handle while selected.
  vtkGetObjectMacro(SelectedHandleProperty, vtkProperty);
  vtkSetObjectMacro(SelectedHandleProperty, vtkProperty);

  // Description:
  // Set/Get the visual properties of the control polygon.
  vtkGetObjectMacro(ControlPolygonProperty, vtkProperty);
  vtkSetObjectMacro(ControlPolygonProperty, vtkProperty);

  // Descripion:
  // Set/Get the visual properties of the control polygon while selected.
  vtkGetObjectMacro(SelectedControlPolygonProperty, vtkProperty);
  vtkSetObjectMacro(SelectedControlPolygonProperty, vtkProperty);

  // Description:
  // Set/Get the visual properties of the Bézier surface.
  vtkGetObjectMacro(BezierSurfaceProperty, vtkProperty);
  vtkSetObjectMacro(BezierSurfaceProperty, vtkProperty);

  // Description:
  // Set/Get resizing factor for handles
  vtkGetMacro(HandleSizeFactor, double);
  vtkSetMacro(HandleSizeFactor, double);

  // Description:
  // Set/Get resizing factor for control polygon (thickness factor).
  vtkGetMacro(ControlPolygonSizeFactor, double);
  vtkSetMacro(ControlPolygonSizeFactor, double);

  // Description:
  // Set/Get resolution of the Bézier surface. Default is 40x40.
  unsigned int GetSurfaceResolutionX() const;
  void SetSurfaceResolutionX(unsigned int resolutionX);
  unsigned int GetSurfaceResolutionY() const;
  void SetSurfaceResolutionY(unsigned int resolutionY);

  // Description:
  // Get the widget polydata, this is, the connected grid of control points.
  void GetWidgetPolyData(vtkPolyData const *pd) const;

  // Description:
  // Get the tubed polydata, this is, the connected grid of points as tubes.
  void GetTubedWidgetPolyData(vtkPolyData const *pd) const;

  // Description:
  // Get the control points.
  void GetControlPoints(vtkPoints const *points) const;

  // Description:
  // Get the Bézier surface as polydata.
  void GetBezierSurfacePolyData(vtkPolyData const *pd) const;

  // Description:
  // Enable/Disable the handles.
  void HandlesOn();
  void HandlesOff();

  // Description:
  // Enable/Disable the control polygon.
  void ControlPolygonOn();
  void ControlPolygonOff();

  // Description:
  // Enable/Disable the Bézier Surface.
  void BezierSurfaceOn();
  void BezierSurfaceOff();

 protected:

  vtkBezierSurfaceWidget();
  ~vtkBezierSurfaceWidget();

  //BTX - manage the state of the widget.
  int State;
  enum WidgetState
  {
    Start=0,
    Deforming,
    MultiDeforming,
    Moving,
    Outside
  };
  //ETX

  // Description:
  // Processes the events related to the mouse movement for the widget.
  static void ProcessEvents(vtkObject *object,
                            unsigned long event,
                            void *clientdata,
                            void *calldata);


  // Description:
  // Methods defining actions to be performed on user interaction. These methods
  // are dispatched by ProcessEvents()
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnRightButtonDown();

  // Description:
  // Elements defining the geometry and visualization of the control polygon.
  vtkSmartPointer<vtkActor> ControlPolygonActor;
  vtkSmartPointer<vtkPolyData> ControlPolygonPolyData;
  vtkSmartPointer<vtkPolyDataMapper> ControlPolygonMapper;

  // Description:
  // Collections of elments defining the geometry and visualization of the handles.
  vtkSmartPointer<vtkCollection> HandleActorCollection;
  vtkSmartPointer<vtkCollection> HandleMapperCollection;
  vtkSmartPointer<vtkCollection> HandlePolyDataCollection;

  // Description:
  // Tube filter employed to beautify the control polygon.
  vtkSmartPointer<vtkTubeFilter> TubeFilter;

  // Description:
  // Position the handles based on the coordinates of the control points in the
  // control polygon.
  virtual void PositionHandles();

  // Description:
  // Resize the handles attending to the camera properties.
  virtual void SizeHandles();

  // Description:
  // Resize (thickness) the control polygon attending to the camera properties.
  virtual void SizeControlPolygon();

  // Description:
  // If a handle is clicked, this points to the actor that holds that handle.
  vtkWeakPointer<vtkActor> CurrentHandle;

  // Description:
  // Highlight the specified handle. If NULL is provided, no handle will be
  // highlighted, this is, all handles will be set to the normal vtkProperty.
  void HighlightHandle(vtkProp *prop);

  // Description:
  // Highlight groups of handles based on the specified handle (either internal
  // handle or external handle). This un-highlighgs the other group of handles.
  void MultiHighlightHandle(vtkProp *prop);

  // Description:
  // Highlights the control polygon based on the specified value (0=
  // not-highlighted, 1=highlighted). If the control polygon is highlighted, the
  // rest of the properties will be automatically un-highlighted.
  void HighlightControlPolygon(int highlight);

  // Description:
  // Picker associated to the handles-
  vtkSmartPointer<vtkCellPicker> HandlePicker;

  // Description:
  // Picker associated to the control polygon.
  vtkSmartPointer<vtkCellPicker> ControlPolygonPicker;

  // Description:
  // Register the handle picker and the control polygon picker.
  virtual void RegisterPickers();

  // Description:
  // Moves the control polygon according to the motion vector
  virtual void MoveControlPolygon(double *p1, double *p2);
  void MoveControlPoint(int cp, double *p1, double *p2);
  void MultiMoveControlPoint(int cp, double *p1, double *p2);

  // Description:
  // Handles visual properties.
  vtkSmartPointer<vtkProperty> HandleProperty;
  vtkSmartPointer<vtkProperty> SelectedHandleProperty;

  // Description:
  // Control polygon properties.
  vtkSmartPointer<vtkProperty> ControlPolygonProperty;
  vtkSmartPointer<vtkProperty> SelectedControlPolygonProperty;

  // Description:
  // Bézier surface visual properties.
  vtkSmartPointer<vtkProperty> BezierSurfaceProperty;

  // Description:
  // Create default properties.
  void CreateDefaultProperties();

  // Description:
  // Interaction flags.
  int MultiPointInteraction;
  int TranslationInteraction;
  int ContinuousBezierUpdate;

  // Description:
  // Parameters of Bézier surfaces. The reader should note that the number of
  // control points is fixed to 4 to facilitate the interaction (internal,
  // external). Therefore only resolution is availabe through Get/Set methods.
  unsigned int SurfaceResolutionX;
  unsigned int SurfaceResolutionY;
  unsigned int NumberOfControlPointsX;
  unsigned int NumberOfControlPointsY;

  // Description:
  // Resizing factor for handles and control polygons
  double HandleSizeFactor;
  double ControlPolygonSizeFactor;

  // Description:
  // Bézier surface elements
  vtkSmartPointer<vtkBezierSurfaceSource> BezierSurfaceSource;
  vtkSmartPointer<vtkPolyDataMapper> BezierSurfaceMapper;
  vtkSmartPointer<vtkActor> BezierSurfaceActor;

 private:
  vtkBezierSurfaceWidget( const vtkBezierSurfaceWidget &) VTK_DELETE_FUNCTION;
  void operator=(const vtkBezierSurfaceWidget&) VTK_DELETE_FUNCTION;
};

#endif
