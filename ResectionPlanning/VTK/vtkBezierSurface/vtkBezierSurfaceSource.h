/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkBezierSurfaceSource.h

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
