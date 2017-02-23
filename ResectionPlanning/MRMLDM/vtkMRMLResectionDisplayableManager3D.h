/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkResectionDisplayableManager3D.h

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

#ifndef __vtkMRMLResectionDisplayableManager3D_h
#define __vtkMRMLResectionDisplayableManager3D_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLDisplayableManagerExport.h"

// MRML includes
#include <vtkMRMLAbstractThreeDViewDisplayableManager.h>


//------------------------------------------------------------------------------
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLResectionDisplayableManager3D :
public vtkMRMLAbstractThreeDViewDisplayableManager
{
 public:

  // Description:
  // VTK required methods
  static vtkMRMLResectionDisplayableManager3D *New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager3D,
               vtkMRMLAbstractThreeDViewDisplayableManager);
  void PrintSelf(ostream &os, vtkIndent indent);

 protected:

  // Description:
  // Constructor and destructor
  vtkMRMLResectionDisplayableManager3D();
  virtual ~vtkMRMLResectionDisplayableManager3D();

 private:

  // Description:
  // Copy constructor and copy operator
  vtkMRMLResectionDisplayableManager3D(const vtkMRMLResectionDisplayableManager3D&);
  void operator=(const vtkMRMLResectionDisplayableManager3D&);
};

#endif
