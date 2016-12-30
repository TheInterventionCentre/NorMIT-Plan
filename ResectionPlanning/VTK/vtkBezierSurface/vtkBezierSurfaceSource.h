/*===============================================================================

  Project: vtkBezierSurface
  Module: vtkBezierSurfaceSource.h

  Contributors:
  - Rafael Palomar <rafael.palomar@rr-research.no>

  Copyright (c) 2015-2016, The Intervention Centre - Oslo University Hospital

  All rights reserved. This is propietary software. In no event shall
  the author be liable for any claim or damages.

  =============================================================================*/

#ifndef __vtkBezierSurfaceSource_h
#define __vtkBezierSurfaceSource_h

// VTK includes
#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>

//-------------------------------------------------------------------------------
class vtkPoints;
class vtkPolyData;
class vtkFloatArray;

//-------------------------------------------------------------------------------
class vtkBezierSurfaceSource : public vtkPolyDataAlgorithm
{
 public:

  //VTK functions
  static vtkBezierSurfaceSource *New();
  vtkTypeMacro(vtkBezierSurfaceSource, vtkPolyDataAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent);

  void SetControlPoints(vtkPoints *points);
  vtkSmartPointer<vtkPoints> GetControlPoints() const;
  void SetNumberOfControlPoints(unsigned int m, unsigned int n);
  void SetResolution(unsigned int x, unsigned int y);
  void GetResolution(unsigned int *resolution) const;

  unsigned int GetResolutionX() const
  {return this->Resolution[0];}

  unsigned int GetResolutionY() const
  {return this->Resolution[1];}

  void ResetControlPoints();

  unsigned int GetNumberOfControlPointsX() const
  {return this->NumberOfControlPoints[0];}

  unsigned int GetNumberOfControlPointsY() const
  {return this->NumberOfControlPoints[1];}

 protected:
  vtkBezierSurfaceSource();
  ~vtkBezierSurfaceSource();
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

 private:
  vtkBezierSurfaceSource(const vtkBezierSurfaceSource&);  // Not implemented.
  void operator=(const vtkBezierSurfaceSource&);  // Not implemented.

  void UpdateTopology();
  void ComputeBinomialCoefficients();
  void UpdateBezierSurfacePolyData(vtkPolyData *polyData);
  void EvaluateBezierSurface(vtkPoints *points);

  unsigned int NumberOfControlPoints[2];
  unsigned int Resolution[2];
  double **ControlPoints;
  double *BinomialCoefficientsX;
  double *BinomialCoefficientsY;
  vtkSmartPointer<vtkDoubleArray> DataArray;
  vtkSmartPointer<vtkCellArray> Topology;
};

#endif
