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
class vtkPolyData;
class vtkTubeFilter;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkProperty;
class vtkCellPicker;

//------------------------------------------------------------------------------
class vtkLineWidget3: public vtk3DWidget
{
 public:

  // Description:
  // Object instantiation.
  static vtkLineWidget3 *New();

  vtkTypeMacro(vtkLineWidget3, vtk3DWidget);

  // Description:
  // Print properties of the object.
  void PrintSelf(ostream &os, vtkIndent indent);

  // Description:
  // Positions the widget in the scene
  virtual void PlaceWidget(double bounds[6]);
  virtual void PlaceWidget();
  virtual void PlaceWidget(double xmin, double xmax,
                           double ymin, double ymax,
                           double zmin, double zmax);

  // Description:
  // Set/Get handle size factor
  vtkGetMacro(HandleSizeFactor, double);
  vtkSetMacro(HandleSizeFactor, double);

  // Description:
  // Set/Get line size factor
  vtkGetMacro(LineSizeFactor, double);
  vtkSetMacro(LineSizeFactor, double);

  // Description:
  // Enable/Disable widget
  void SetEnabled(int enabling);

 protected:

  // Description:
  // Constructor and Destructor.
  vtkLineWidget3();
  ~vtkLineWidget3();

    //BTX - manage the state of the widget.
  int State;
  enum WidgetState
  {
    Start=0,
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
 // Resize the handles.
 virtual void SizeHandles();
 virtual void SizeLine();

 // Description:
 // End points of the line
 double Point1[3];
 double Point2[3];

 // Description:
 // Polydata describing the line.
 vtkNew<vtkPolyData> LinePolyData;

 // Desctiption:
 // Sphere source for handles representation.
 vtkNew<vtkSphereSource> Handle1Source;
 vtkNew<vtkSphereSource> Handle2Source;

 // Description:
 // Tube filter.
 vtkNew<vtkTubeFilter> TubeFilter;

 // Description:
 // Mappers.
 vtkNew<vtkPolyDataMapper> LineMapper;
 vtkNew<vtkPolyDataMapper> Handle1Mapper;
 vtkNew<vtkPolyDataMapper> Handle2Mapper;

 // Description:
 // Actors.
 vtkNew<vtkActor> LineActor;
 vtkNew<vtkActor> Handle1Actor;
 vtkNew<vtkActor> Handle2Actor;

 // Description:
 // Visual properties of line
 vtkNew<vtkProperty> LineProperty;

 // Description:
 // Visual properties of Handle1
 vtkNew<vtkProperty> Handle1Property;
 vtkNew<vtkProperty> SelectedHandle1Property;

 // Description:
 // Visual properties of Handle2
 vtkNew<vtkProperty>Handle2Property;
 vtkNew<vtkProperty>SelectedHandle2Property;

 virtual void CreateDefaultProperties();

  // Description:
 // Picker associated to handle1
 vtkNew<vtkCellPicker> Handle1Picker;

 // Description
 // Picker associated to handle2
 vtkNew<vtkCellPicker> Handle2Picker;

 private:
 // Description:
  // Copy constructor.
  vtkLineWidget3(const vtkLineWidget3&);
  void operator=(const vtkLineWidget3&);

  vtkWeakPointer<vtkActor> CurrentHandle;

  // Description:
  // Resizing factor for the handles.
  double HandleSizeFactor;

  // Description:
  // Resizing factor for the line.
  double LineSizeFactor;


};

#endif
