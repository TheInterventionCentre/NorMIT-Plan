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

//------------------------------------------------------------------------------
class vtkBezierSurfaceWidget: public vtk3DWidget
{
 public:

  static vtkBezierSurfaceWidget *New();

  vtkTypeMacro(vtkBezierSurfaceWidget, vtk3DWidget);
  void PrintSelf(ostream &os, vtkIndent indent);

  virtual void SetEnabled(int);
  virtual void PlaceWidget(double bounds[6]);
  void PlaceWidget()
  {this->Superclass::PlaceWidget();}
  void PlaceWidget(double xmin, double xmax,
                   double ymin, double ymax,
                   double zmin, double zmax)
  {this->Superclass::PlaceWidget(xmin, xmax, ymin, ymax, zmin, zmax);}

  vtkGetMacro(MultiPointInteraction, int);
  vtkSetMacro(MultiPointInteraction, int);
  vtkBooleanMacro(MultiPointInteraction, int);

  vtkGetMacro(TranslationInteraction, int);
  vtkSetMacro(TranslationInteraction, int);
  vtkBooleanMacro(TranslationInteraction, int);

  vtkGetMacro(ContinuousBezierUpdate, int);
  vtkSetMacro(ContinuousBezierUpdate, int);
  vtkBooleanMacro(ContinuousBezierUpdate, int);

  vtkGetObjectMacro(HandleProperty, vtkProperty);
  vtkSetObjectMacro(HandleProperty, vtkProperty);
  vtkGetObjectMacro(SelectedHandleProperty, vtkProperty);
  vtkSetObjectMacro(SelectedHandleProperty, vtkProperty);
  vtkGetObjectMacro(ControlPolygonProperty, vtkProperty);
  vtkSetObjectMacro(ControlPolygonProperty, vtkProperty);
  vtkGetObjectMacro(SelectedControlPolygonProperty, vtkProperty);
  vtkSetObjectMacro(SelectedControlPolygonProperty, vtkProperty);
  vtkGetObjectMacro(BezierSurfaceProperty, vtkProperty);
  vtkSetObjectMacro(BezierSurfaceProperty, vtkProperty);

  void GetWidgetPolyData(vtkPolyData const *pd) const;
  void GetTubedWidgetPolyData(vtkPolyData const *pd) const;
  void GetControlPoints(vtkPoints const *points) const;
  void GetBezierSurfacePolyData(vtkPolyData const *pd) const;

  void HandlesOn();
  void HandlesOff();

  void ControlPolygonOn();
  void ControlPolygonOff();

  void BezierSurfaceOn();
  void BezierSurfaceOff();

 protected:
  vtkBezierSurfaceWidget();
  ~vtkBezierSurfaceWidget();

  int State;
  enum WidgetState
  {
    Start=0,
    Deforming,
    MultiDeforming,
    Moving,
    Outside
  };

  static void ProcessEvents(vtkObject *object,
                            unsigned long event,
                            void *clientdata,
                            void *calldata);

  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnRightButtonDown();

  vtkSmartPointer<vtkActor> ControlPolygonActor;
  vtkSmartPointer<vtkPolyData> ControlPolygonPolyData;
  vtkSmartPointer<vtkPolyDataMapper> ControlPolygonMapper;
  vtkSmartPointer<vtkCollection> HandleActorCollection;
  vtkSmartPointer<vtkCollection> HandleMapperCollection;
  vtkSmartPointer<vtkCollection> HandlePolyDataCollection;

  vtkSmartPointer<vtkTubeFilter> TubeFilter;

  virtual void PositionHandles();
  virtual void SizeHandles();
  virtual void SizeControlPolygon();

  vtkWeakPointer<vtkActor> CurrentHandle;

  void HighlightHandle(vtkProp *prop);
  void MultiHighlightHandle(vtkProp *prop);
  void HighlightControlPolygon(int highlight);

  vtkSmartPointer<vtkCellPicker> HandlePicker;
  vtkSmartPointer<vtkCellPicker> ControlPolygonPicker;

  virtual void RegisterPickers();

  virtual void MoveControlPolygon(double *p1, double *p2);
  void MoveControlPoint(int cp, double *p1, double *p2);
  void MultiMoveControlPoint(int cp, double *p1, double *p2);

  vtkSmartPointer<vtkProperty> HandleProperty;
  vtkSmartPointer<vtkProperty> SelectedHandleProperty;
  vtkSmartPointer<vtkProperty> ControlPolygonProperty;
  vtkSmartPointer<vtkProperty> SelectedControlPolygonProperty;
  vtkSmartPointer<vtkProperty> BezierSurfaceProperty;
  void CreateDefaultProperties();

  int MultiPointInteraction;
  int TranslationInteraction;
  int ContinuousBezierUpdate;
  unsigned int SurfaceResolutionX;
  unsigned int SurfaceResolutionY;
  unsigned int NumberOfControlPointsX;
  unsigned int NumberOfControlPointsY;

  double HandleSizeFactor;

  vtkSmartPointer<vtkBezierSurfaceSource> BezierSurfaceSource;
  vtkSmartPointer<vtkPolyDataMapper> BezierSurfaceMapper;
  vtkSmartPointer<vtkActor> BezierSurfaceActor;

 private:
  vtkBezierSurfaceWidget( const vtkBezierSurfaceWidget &) VTK_DELETE_FUNCTION;
  void operator=(const vtkBezierSurfaceWidget&) VTK_DELETE_FUNCTION;
};

#endif
