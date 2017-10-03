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
class vtkPoints;
class vtkPolyDataNormals;

//------------------------------------------------------------------------------
/**
 * \ingroup ResectionPlanning
 *
 * \brief This class is used to create a network of control points associated to
 * a Bézier surface. The control points can be interactively modified, thus
 * producing a deformation of the Bézier surface.
 *
 * Visualization of the different elements is possible through the
 * HandleOn/Off(), ControlPolygonOn/Off(), and BezierSurfaceOn/Off(). In
 * addition, visualization properties of these geometric elements can be set by
 * using SetHandleProperty(), SetSelectedHandleProperty(),
 * SetControlPolygonProperty(), SetSelectedControlPolygonProperty() and
 * SetBezierSurfaceProperty().
 */
class vtkBezierSurfaceWidget: public vtk3DWidget
{
 public:

  /**
   * Instantiation of a new object
   */
  static vtkBezierSurfaceWidget *New();

  vtkTypeMacro(vtkBezierSurfaceWidget, vtk3DWidget);

  /**
   *  Print properties of the object.
   *
   * @param os output stream
   * @param indent indentation
   */
  void PrintSelf(ostream &os, vtkIndent indent);

  /**
   * Enable/Disable the widget
   *
   * @param enabled flag indicating whether the widget is enabled/disabled.
   */
  virtual void SetEnabled(int enabled);

  /**
   * Place the widget within the given bounding box.
   *
   * @param bounds min-max values defining the aligned boundin box.
   */
  virtual void PlaceWidget(double bounds[6]);

  /**
   * Place the widget within the given bounding box
   *
   */
  void PlaceWidget()
  {this->Superclass::PlaceWidget();}

  /**
   * Place the widget within the given bounding box
   *
   * @param xmin min coordinate in x axis.
   * @param xmax max coordinate in x axis.
   * @param ymin min coordinate in y axis.
   * @param ymax max coordinate in y axis.
   * @param zmin min coordinate in z axis.
   * @param zmax max coordinate in z axis.
   */
  void PlaceWidget(double xmin, double xmax,
                   double ymin, double ymax,
                   double zmin, double zmax)
  {this->Superclass::PlaceWidget(xmin, xmax, ymin, ymax, zmin, zmax);}

  /**
   * Get auto size flag
   *
   * @return autosize flag.
   */
  vtkGetMacro(AutoSize, bool);

  /**
   * Set auto size flag
   *
   * @return autosize flag.
   */
  vtkSetMacro(AutoSize, bool);

  /**
   * Boolean Auto size
   */
  vtkBooleanMacro(AutoSize, bool);

  /**
   * Get the MultiPointInteraction flag. If the flag is != 0 (enabled) then
   * multi-point interaction is enabled. MultiPointInteraction is enabled by
   * default.
   *
   * @return 0 if disabled, 1 otherwise.
   */
  vtkGetMacro(MultiPointInteraction, int);

  /**
   * Set the MultiPointInteraction flag. If the flag is != 0 (enabled) then
   * multi-point interaction is enabled. MultiPointInteraction is enabled by
   * default.
   */
  vtkSetMacro(MultiPointInteraction, int);
  vtkBooleanMacro(MultiPointInteraction, int);


 /**
  * Get the TranslationInteraction flag. If the flag is != 0 (enabled) then
  * the global translation of the control polygon and the surface is
  * enabled. Translation interaction is enabled by default.
  *
  * @return 0 if disabled, 1 otherwise.
  */
  vtkGetMacro(TranslationInteraction, int);

  /**
  * Set the TranslationInteraction flag. If the flag is != 0 (enabled) then
  * the global translation of the control polygon and the surface is
  * enabled. Translation interaction is enabled by default.
  */
  vtkSetMacro(TranslationInteraction, int);
  vtkBooleanMacro(TranslationInteraction, int);

 /**
  * Get the ContinuousBezierUpdate flag. If the flag is !=0 (enabled) then
  * the update of the BézierSurface happens in a continuous way, this is,
  * while the mouse is moving, rather than when the user releases the mouse (at
  * the end of the interaction). Continuous Bézier interaction is enabled by default.
  *
  * @return 0 if disabled, 1 otherwise.
  */
  vtkGetMacro(ContinuousBezierUpdate, int);
  /**
  * Get the ContinuousBezierUpdate flag. If the flag is !=0 (enabled) then
  * the update of the BézierSurface happens in a continuous way, this is,
  * while the mouse is moving, rather than when the user releases the mouse (at
  * the end of the interaction). Continuous Bézier interaction is enabled by
  * default.
  */
  vtkSetMacro(ContinuousBezierUpdate, int);
  vtkBooleanMacro(ContinuousBezierUpdate, int);

  /**
   * Get the visual properties of the handle.
   *
   * @return vtkProperty object containing the visual properties of the handle.
   */
  vtkGetNewMacro(HandleProperty,vtkProperty);
  /**
   * Set the visual properties of the handle.
   *
   * @param prop vtkProperty property object containing the visual properties of
   * the handle.
   */
  void SetHandleProperty(vtkProperty *prop)
  {this->HandleProperty->DeepCopy(prop);}

  /**
   * Get the visual properties of the handle while selected.
   *
   * @return vtkProperty object containing the visual properties of the handle
   * when this is selected.
   */
  vtkGetNewMacro(SelectedHandleProperty, vtkProperty);
  /**
   * Set the visual properties of the handle while selected.
   *
   * @param prop vtkProperty object containing the visual properties of the
   * handle when this is selected
   */
  void SetSelectedHandleProperty(vtkProperty *prop)
  {this->SelectedHandleProperty->DeepCopy(prop);}

  /**
   * Get the visual properties of the control polygon.
   *
   * @return vtkProperty object containing the visual properties of the control
   * polygon (net of control points).
   */
  vtkGetNewMacro(ControlPolygonProperty, vtkProperty);

  /**
   * Set the visual properties of the control polygon (net of control points).
   *
   * @param prop vtkProperty object containing the visual properties of the
   * control polygon (net of control points).
   */
  void SetControlPolygonProperty(vtkProperty *prop)
  {this->ControlPolygonProperty->DeepCopy(prop);}


  /**
   * Get the visual properties of the control polygon while selected.
   *
   * @return pointer to  vtkProperty containing the visual properties of the control
   * polygon while selected.
   */
  vtkGetNewMacro(SelectedControlPolygonProperty, vtkProperty);

  /**
   * Set the visual properties of the control polygon while selected.
   *
   * @param prop pointer to vtkProperty containing the visual properties of the
   * control polygon when this is selected.
   */
  void SetSelectedControlPolygonProperty(vtkProperty *prop)
  {this->SelectedControlPolygonProperty->DeepCopy(prop);}

  /**
   * Get the visual properties of the Bézier surface.
   *
   * @return pointer to vtkProperty containing the visual properties of the
   * Bézier surface.
   */
  vtkGetNewMacro(BezierSurfaceProperty, vtkProperty);

  /**
   * Set hte visual properties of the Bézier surface.
   *
   * @param prop pointer to a vtkProperty containing the visual properties of
   * the Bézier surface.
   */
  void SetBezierSurfaceProperty(vtkProperty *prop)
  {this->BezierSurfaceProperty->DeepCopy(prop);}

  /**
   *  Get the resizing factor for handles.
   *
   * @return resizing factor.
   */
  vtkGetMacro(HandleSizeFactor, double);

  /**
   * Set the resizing factor for handles.
   *
   * @param double resizing factor
   */
  vtkSetMacro(HandleSizeFactor, double);

  /**
   * Get resizing factor for control polygon (thickness factor).
   *
   * @return resizing factor for the control polygon.
   */
  vtkGetMacro(ControlPolygonSizeFactor, double);

  /**
   * Set resizing factor for control polkygon (thickness factor).
   *
   * @param double resizing factor.
   *
   */vtkSetMacro(ControlPolygonSizeFactor, double);

  /**
   * Get resolution of the Bézier surface (X).
   *
   * @return  X resolution of the Bézier surface in number of quads.
   */
  unsigned int GetSurfaceResolutionX() const;

  /**
   * Set resolution of the Bézier surface (X) in number of quads.
   *
   * @param resolutionX
   */
  void SetSurfaceResolutionX(unsigned int resolutionX);

  /**
   * Get resolution of the Bézier surface (Y) in number of quads.
   *
   * @return Y resolution of the Bézier surface (Y) in number of quads.
   */
  unsigned int GetSurfaceResolutionY() const;

  /**
   * Set resolution of Bézier surface (Y) in number of quads.
   *
   * @param resolutionY
   */
  void SetSurfaceResolutionY(unsigned int resolutionY);

  /**
   * Get the widget polydata.
   *
   * @return pointer to vtkPolyData containing the grid of connected control points.
   */
  vtkPolyData* GetWidgetPolyData() const
  {return this->ControlPolygonPolyData.GetPointer();}

  /**
   * Get the tubed widget polydata
   *
   * @return pointer to vtkPolyData containing the tubed grid of connected
   * control points.
   */
  vtkPolyData* GetTubedWidgetPolyData() const;

  /**
   * Get the control points.
   *
   * @return pointer to vtkPoints containing the coordinates of the points of
   * the grid of control points.
   */
  vtkPoints* GetControlPoints() const;

  /**
   * Get the Bézier surface as polydata.
   *
   * @return pointer to vtkPolyData containing the triangular representation of
   * the Bézier surface.
   */
  vtkPolyData* GetBezierSurfacePolyData() const;

  /**
   * Enable handles.
   */
  void HandlesOn();

  /**
   * Disable handles.
   */
  void HandlesOff();

  /**
   * Enable the control polygon
   */
  void ControlPolygonOn();

  /**
   * Disable the control polygon.
   */
  void ControlPolygonOff();

  /**
   * Enable the Bézier surface
   */
  void BezierSurfaceOn();

  /**
   * Disable the Bézier surface.
   */
  void BezierSurfaceOff();

  vtkGetNewMacro(ControlPoints, vtkPoints);

  /**
   * Set control points
   *
   * @param points pointer to vtkPoints.
   */
  void SetControlPoints(vtkPoints *points);

    /**
   * Enable surface normals computation
   *
   */
  void ComputeNormalsOn();

  /**
   * Disable surface normals computation
   *
   */
  void ComputeNormalsOff();

  /**
   * Check the compute normals internal flag
   *
   *
   * @return true if normals are to be computed, false otherwise.
   */
  bool ComputeNormals()
  {return this->ComputeNormalsFlag;}

  /**
   * Enable/Disable normals computation
   *
   * @param computeNormals flag indicating whether the normals should be computed.
   */
  void ComputeNormals(bool computeNormals);

 protected:

  vtkBezierSurfaceWidget();
  ~vtkBezierSurfaceWidget();

  //BTX - manage the state of the widget.
  int State;
  enum WidgetState
  {
    Start=0,          //!< Start interaction.
    Deforming,        //!< Deformation interaction.
    MultiDeforming,   //!< Multiple-point deformation interaction.
    Moving,           //!< Moving (translation) interaction.
    Outside           //!< Outside the widget.
  };
  //ETX


  /**
   * Processes the events related to the mouse movement for the widget.
   *
   * @param object pointer to vtkObject initiating the event
   * @param event event identifier
   * @param clientdata client data
   * @param calldata call data
   */
  static void ProcessEvents(vtkObject *object,
                            unsigned long event,
                            void *clientdata,
                            void *calldata);
  /**
   * Handling of mouse move interaction. This is dispatched by ProcessEvents().
   */
  virtual void OnMouseMove();

  /**
   * Handling of mouse left click (down). This is dispatched by ProcessEvents().
   */
  virtual void OnLeftButtonDown();

  /**
   * Handling of mouse left click (up). This is dispatched by ProcessEvents().
   */
  virtual void OnLeftButtonUp();

  /**
   * Handling of mouse right click (up). This is dispatched by ProcessEvents().
   */
  virtual void OnRightButtonDown();

  /**
   * Handling of mouse right click (down). This is dispatched by ProcessEvents().
   */
  virtual void OnRightButtonUp();

  // Elements defining the geometry and visualization of the control polygon.
  vtkNew<vtkActor> ControlPolygonActor;
  vtkNew<vtkPolyData> ControlPolygonPolyData;
  vtkNew<vtkPolyDataMapper> ControlPolygonMapper;

  // Collections of elments defining the geometry and visualization of the handles.
  vtkNew<vtkCollection> HandleActorCollection;
  vtkNew<vtkCollection> HandleMapperCollection;
  vtkNew<vtkCollection> HandlePolyDataCollection;

  // Tube filter employed to beautify the control polygon.
  vtkNew<vtkTubeFilter> TubeFilter;

  /**
   * Position the handles based on the coordinates of the control points in the
   * control polygon.
   */
  virtual void PositionHandles();

  /**
   * Resize the handles attending to the camera properties.
   */
  virtual void SizeHandles();

  /**
   * Resize (thickness) the control polygon attending to the camera properties.
   */
  virtual void SizeControlPolygon();

  // If a handle is clicked, this points to the actor that holds that handle.
  vtkWeakPointer<vtkActor> CurrentHandle;

  /**
   * Highlight the specified handle. If NULL is provided, no handle will be
   * highlighted, this is, all handles will be set to the normal vtkProperty.
   *
   * @param prop pointer to vtkProp (actor) to be highlighted.
   */
  void HighlightHandle(vtkProp *prop);

  /**
   * Highlight groups of handles based on the specified handle (either internal
   * handle or external handle). This un-highlighgs the other group of handles.
   *
   * @param prop pointer to vtkProp (actor) in the group of actors  to be
   * highlighted.
   */
  void MultiHighlightHandle(vtkProp *prop);

  /**
   *   Highlights the control polygon based on the specified value (0=
   *   not-highlighted, 1=highlighted). If the control polygon is highlighted,
   *   the rest of the properties will be automatically un-highlighted.
   *
   * @param highlight flag indicating whether the control polygon should be
   * highlighted or not.
   */
  void HighlightControlPolygon(int highlight);

  // Picker associated to the handles.
  vtkNew<vtkCellPicker> HandlePicker;

  // Picker associated to the control polygon.
  vtkNew<vtkCellPicker> ControlPolygonPicker;

  /**
   * Register the handle picker and the control polygon picker.
   */
  virtual void RegisterPickers();

  /**
   * Moves the control polygon according to the motion vector.
   *
   * @param p1 starting point of the motion vector
   * @param p2 ending point of the motion vector.
   */
  virtual void MoveControlPolygon(double *p1, double *p2);

  /**
   * Moves the indicated control point according to the motion vector.
   *
   * @param cp index to a control point
   * @param p1 starting point of the motion vector.
   * @param p2 ending point of the motion vector.
   */
  void MoveControlPoint(int cp, double *p1, double *p2);

  /**
   * Moves the group of control points indicated by the index. Note that the
   * index can be to any control point contained in the target group of control points.
   *
   * @param cp index to control point.
   * @param p1 starting point of the motion vector.
   * @param p2 ending point of the motion vector.
   */
  void MultiMoveControlPoint(int cp, double *p1, double *p2);

  // Handles visual properties.
  vtkNew<vtkProperty> HandleProperty;
  vtkNew<vtkProperty> SelectedHandleProperty;

  // Control polygon properties.
  vtkNew<vtkProperty> ControlPolygonProperty;
  vtkNew<vtkProperty> SelectedControlPolygonProperty;

  // Bézier surface visual properties.
  vtkNew<vtkProperty> BezierSurfaceProperty;

  // Create default properties.
  void CreateDefaultProperties();

  // Interaction flags.
  int MultiPointInteraction;
  int TranslationInteraction;
  int ContinuousBezierUpdate;

  // Parameters of Bézier surfaces. The reader should note that the number of
  // control points is fixed to 4 to facilitate the interaction (internal,
  // external). Therefore only resolution is availabe through Get/Set methods.
  unsigned int SurfaceResolutionX;
  unsigned int SurfaceResolutionY;
  unsigned int NumberOfControlPointsX;
  unsigned int NumberOfControlPointsY;

  // Resizing factor for handles and control polygons
  double HandleSizeFactor;
  double ControlPolygonSizeFactor;

  // Bézier surface elements
  vtkNew<vtkBezierSurfaceSource> BezierSurfaceSource;
  vtkNew<vtkPolyDataMapper> BezierSurfaceMapper;
  vtkNew<vtkActor> BezierSurfaceActor;

  // Polydata normals
  vtkNew<vtkPolyDataNormals> Normals;
  bool ComputeNormalsFlag;

  // Control points
  vtkNew<vtkPoints> ControlPoints;

  bool AutoSize;
  double HandleSize;
  double TubeRadius;

 private:
  vtkBezierSurfaceWidget( const vtkBezierSurfaceWidget &) VTK_DELETE_FUNCTION;
  void operator=(const vtkBezierSurfaceWidget&) VTK_DELETE_FUNCTION;
};

#endif
