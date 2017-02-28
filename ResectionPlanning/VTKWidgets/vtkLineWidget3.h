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

 protected:

  // Description:
  // Constructor and Destructor.
  vtkLineWidget3();
  ~vtkLineWidget3();

 private:
  // Description:
  // Copy constructor.
  vtkLineWidget3(const vtkLineWidget3&);
  void operator=(const vtkLineWidget3&);
};

#endif
