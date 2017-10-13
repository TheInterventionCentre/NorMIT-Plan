/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionInitializationDisplayNode.h

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

#ifndef __vtkMRMLResectionInitializationDisplayNode_h
#define __vtkMRMLResectionInitializationDisplayNode_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLExport.h"

// MRML includes
#include <vtkMRMLDisplayNode.h>


// VTK includes

/**
 * \ingroup ResectionPlanning
 *
 * This class holds the information related to the visualization of the
 * initialization node.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRML_EXPORT
vtkMRMLResectionInitializationDisplayNode: public vtkMRMLDisplayNode
{
 public:

  /**
   * Standard VTK object instantiation method
   *
   *
   * @return a pointer to a new crated vtkMRMLResectionSurfaceNode.
   */
  static vtkMRMLResectionInitializationDisplayNode *New();
  vtkTypeMacro(vtkMRMLResectionInitializationDisplayNode, vtkMRMLDisplayNode);

  /**
   * Standard print object information method.
   *
   * @param os output stream to print the information to.
   * @param indent indent value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);

  /**
   * Get the tag name of the node.
   *
   *
   * @return
   */
  virtual const char* GetNodeTagName()
  {return "ResectionInitializationDisplay";}

  /**
   * Get the icon associated to the node.
   *
   *
   * @return string pointing to the resource where the icon is located.
   */
  virtual const char* GetIcon() {return "";}

  virtual vtkMRMLNode* CreateNodeInstance();

 protected:
  vtkMRMLResectionInitializationDisplayNode();
  ~vtkMRMLResectionInitializationDisplayNode();

 private:

  vtkMRMLResectionInitializationDisplayNode(
    const vtkMRMLResectionInitializationDisplayNode&);
  void operator=(const vtkMRMLResectionInitializationDisplayNode&);

};

#endif
