/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager3D.cxx

  Copyright (c) 2018, The Intervention Centre, Oslo University Hospital

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

#ifndef __vtkDistanceFilter_h
#define __vtkDistanceFilter_h

#include <vtkPolyDataAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkType.h>

//------------------------------------------------------------------------------
class vtkKdTreePointLocator;
class vtkDoubleArray;

//------------------------------------------------------------------------------
class VTK_EXPORT vtkDistanceFilter : public
vtkPolyDataAlgorithm
{
 public:
  static vtkDistanceFilter *New();
  vtkTypeMacro(vtkDistanceFilter, vtkPolyDataAlgorithm);

  void PrintSelf(ostream &os, vtkIndent indent);

 protected:
  vtkDistanceFilter();
  ~vtkDistanceFilter() override;

  int RequestData(vtkInformation*, vtkInformationVector **, vtkInformationVector *) override;
  int FillInputPortInformation( int port, vtkInformation* info );
  int FillOutputPortInformation( int port, vtkInformation* info );

 private:

  vtkDistanceFilter(const vtkDistanceFilter&) = delete;
  void operator=(const vtkDistanceFilter&) = delete;

  vtkNew<vtkKdTreePointLocator> PointLocator;
  vtkSmartPointer<vtkDoubleArray> DistanceArray;
  vtkMTimeType TargetLastModified;


};


#endif