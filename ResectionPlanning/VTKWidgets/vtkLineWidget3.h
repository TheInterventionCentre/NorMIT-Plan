/*=========================================================================

  Program: NorMIT-Nav
  Module: vtkLineWidget3.h

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

#ifndef __vtkLineWidget3_h
#define __vtkLineWidget3_h

//VTK includes
#include <vtk3DWidget.h>
#include <vtkWeakPointer.h>
#include <vtkNew.h>

//------------------------------------------------------------------------------
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkActor;
class vtkSphereSource;
class vtkProperty;
class vtkCellPicker;
class vtkProp;
class vtkLineSource;
class vtkCutter;
class vtkPlane;

//------------------------------------------------------------------------------
/**
 * \ingroup ResectionPlanning
 *
 * \brief This class is used to create a line widget which can be manipulated by
 * two handles at the end-points in the line.
 *
 */
class vtkLineWidget3: public vtk3DWidget
{
 public:

  /**
   * Object instantiation.
   *
   * @return pointer to newly created vtkLineWidget3 object.
   */
  static vtkLineWidget3 *New();

  vtkTypeMacro(vtkLineWidget3, vtk3DWidget);

  /**
   * Print the properties of the object.
   *
   * @param os output stream to print the properties to.
   * @param indent indentation value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);


  /**
   * Place the widget within the specified bounds.
   *
   * @param bounds array of bounds where the order is xmin, xmax, ymin, ymax, zmin, zmax.
   */
  virtual void PlaceWidget(double bounds[6]);

  /**
   * Place the widget within the default bounds (-0.5, 0.5 for all dimensions).
   */
  virtual void PlaceWidget();

  /**
   * Place the widget within the specified bounds
   *
   * @param xmin
   * @param xmax
   * @param ymin
   * @param ymax
   * @param zmin
   * @param zmax
   */
  virtual void PlaceWidget(double xmin, double xmax,
                           double ymin, double ymax,
                           double zmin, double zmax);

  /**
   * Get handle resizing factor.
   *
   * @return handle resizing factor.
   */
  vtkGetMacro(HandleSizeFactor, double);

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
   * Set handle resizing factor.
   *
   * @param double resizing factor
   *
   */
  vtkSetMacro(HandleSizeFactor, double);

  /**
   * Get line resizing factor.
   *
   * @return line resizing factor (line thickness).
   */
  vtkGetMacro(LineSizeFactor, double);

  /**
   * Set the line resizing factor.
   *
   * @param double line resizing factor (line thickness).
   */
  vtkSetMacro(LineSizeFactor, double);


  /**
   * Enable/Disable widget according to the specified flag.
   *
   * @param enabling flag indicating whether the widget should be enabled (1) or
   * disabled (0).
   */
  void SetEnabled(int enabling);

  /**
   * Set the cutting target
   *
   * @param target pointer to polydata to be sliced.
   */
  void SetCuttingTarget(vtkPolyData *target);

  /**
   * Get the cutting target
   *
   *
   * @return pointer to polydata representing the cutting target.
   */
  vtkPolyData *GetCuttingTarget() const;

  vtkGetVector3Macro(Point1, double);
  vtkGetVector3Macro(Point2, double);

  /**
   * Set the coordinates of the Point1.
   *
   * @param point1
   */
  void SetPoint1(double point1[3]);

  /**
   * Set the coordinates of the Point2.
   *
   * @param point2
   */
  void SetPoint2(double point2[3]);

 protected:

  // Description:
  // Constructor and Destructor.
  vtkLineWidget3();
  ~vtkLineWidget3();

    //BTX - manage the state of the widget.
  int State;
  enum WidgetState
  {
    Start=0, //!< Start interaction.
    Moving,  //!< Mofing interaction.
    Outside  //!< Outside the widget.
  };
  //ETX


  /**
   * Processes the events related to the mouse movement for the widget.
   *
   * @param object object which triggered the event.
   * @param event event identifier.
   * @param clientdata client data.
   * @param calldata call data.
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
   * Resize the handles.
   */
  virtual void SizeHandles();

  /**
   * Resize the line.
   */
  virtual void SizeLine();

  // End points of the line
  double Point1[3];
  double Point2[3];

  // Sphere source for handles representation.
  vtkNew<vtkSphereSource> Handle1Source;
  vtkNew<vtkSphereSource> Handle2Source;

  // Line source for the line representation.
  vtkNew<vtkLineSource> LineSource;

  // Tube filter.
  vtkNew<vtkTubeFilter> TubeFilter;

  // Cutter and cutting plane
  vtkNew<vtkCutter> Cutter;
  vtkNew<vtkPlane> CuttingPlane;

  // Mappers.
  vtkNew<vtkPolyDataMapper> LineMapper;
  vtkNew<vtkPolyDataMapper> Handle1Mapper;
  vtkNew<vtkPolyDataMapper> Handle2Mapper;
  vtkNew<vtkPolyDataMapper> SlicingContourMapper;

  // Actors.
  vtkNew<vtkActor> LineActor;
  vtkNew<vtkActor> Handle1Actor;
  vtkNew<vtkActor> Handle2Actor;
  vtkNew<vtkActor> SlicingContourActor;

  // Visual properties of line
  vtkNew<vtkProperty> LineProperty;

  // Visual properties of Handle1
  vtkNew<vtkProperty> Handle1Property;
  vtkNew<vtkProperty> SelectedHandle1Property;

  // Visual properties of Handle2
  vtkNew<vtkProperty> Handle2Property;
  vtkNew<vtkProperty> SelectedHandle2Property;

  // Visual properties of the slicing contour.
  vtkNew<vtkProperty> SlicingContourProperty;

  /**
   * Create the default visual properties for the line and the handles.
   */
  virtual void CreateDefaultProperties();

  // Picker associated to handle1
  vtkNew<vtkCellPicker> Handle1Picker;

  // Picker associated to handle2
  vtkNew<vtkCellPicker> Handle2Picker;

  /**
   * Move the specified handle
   *
   * @param prop pointer to property (actor) to be highlighted.
   * @param p1 starting point of the motion vector.
   * @param p2 ending point of the motion vector.
   */
  void MoveHandle(vtkProp *prop, double *p1, double *p2);

  /**
   * Change the visual properties of the handle to highlight it.
   *
   * @param prop pointer to vtkProp (actor) to be highlighted (the other handle
   * will be un-highlighted). If NULL is passed, all the handles will be
   * un-highlighted.
   */
  void HighlightHandle(vtkProp *prop);

private:

  // Copy constructor.
  vtkLineWidget3(const vtkLineWidget3&);
  void operator=(const vtkLineWidget3&);

  vtkWeakPointer<vtkActor> CurrentHandle;
  vtkWeakPointer<vtkPolyData> CuttingTarget;

  // Description:
  // Resizing factor for the handles.
  double HandleSizeFactor;

  // Description:
  // Resizing factor for the line.
  double LineSizeFactor;

  bool AutoSize;
  double HandleSize;
  double TubeRadius;
};

#endif
