/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLLRPModelDisplayNode.h

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

#ifndef __vtkMRMLLRPModelDisplayNode_h
#define __vtkMRMLLRPModelDisplayNode_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLExport.h"

//MRML
#include <vtkMRMLModelDisplayNode.h>

//------------------------------------------------------------------------------
/**
 * \ingroup ResectionPlanning
 *
 * This is a support class to distinguish between regular 3D Slicer models
 * and 3D models corresponding to organs in the resection planning systems.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRML_EXPORT
vtkMRMLLRPModelDisplayNode: public vtkMRMLModelDisplayNode
{
 public:

  /**
   * Standard VTK object instantiation methods
   *
   * @return a pointer to a newly created vtkMRMLLRPModelDisplayNode.
   */
  static vtkMRMLLRPModelDisplayNode* New();

  vtkTypeMacro(vtkMRMLLRPModelDisplayNode, vtkMRMLModelDisplayNode);

  /**
   * Standard print object information method.
   *
   * @param os output stream to print the information to.
   * @param indent intdent value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);

  /**
   * Standard MRML method to create the node instance.
   *
   *
   * @return a pointer to the newly created vtkMRMLNode.
   */

  virtual vtkMRMLNode* CreateNodeInstance();

  /**
   * Get the tag name of the node.
   *
   *
   * @return string with the tag name of the node.
   */
  virtual const char* GetNodeTagName() {return "LRPModelDisplayNode";}


  /**
   * Get the icon associated to the node.
   *
   *
   * @return string pointing to the resource where the icon is located.
   */
  virtual const char* GetIcon() {return "";}

 protected:
  vtkMRMLLRPModelDisplayNode();
  ~vtkMRMLLRPModelDisplayNode();

};

#endif
