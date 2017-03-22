/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager2D.h

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

#ifndef __vtkMRMLResectionDisplayableManager2D_h
#define __vtkMRMLResectionDisplayableManager2D_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLDisplayableManagerExport.h"

// MRML includes
#include <vtkMRMLAbstractSliceViewDisplayableManager.h>

//------------------------------------------------------------------------------
class vtkMRMLScene;
class vtkMRMLResectionSurfaceNode;

//------------------------------------------------------------------------------
/**
 * \ingroup ResectionPlanning
 *
 * Resection surface displayable manager for 2D views
 *
 * This class encapsulates the functionality to display the projection of the
 * reseciton in the 2D slices.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLResectionDisplayableManager2D
:public vtkMRMLAbstractSliceViewDisplayableManager
{
 public:

  /**
   * Standard VTK object creation.
   *
   *
   * @return a pointer to the new displayable manager created.
   */
  static vtkMRMLResectionDisplayableManager2D *New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager2D,
               vtkMRMLAbstractSliceViewDisplayableManager);

  /**
   * Standard VTK object information print function.
   *
   * @param os output stream to print the info to.
   * @param indent indentation value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);

 protected:
  vtkMRMLResectionDisplayableManager2D();
  ~vtkMRMLResectionDisplayableManager2D();

  /**
   * Set a new MRML scene
   *
   * @param newScene
   */
  virtual void SetMRMLSceneInternal(vtkMRMLScene *newScene);

  /**
   * Actions when a node gets added to the mrml scene
   *
   * @param node node added to the scene
   */
  void OnMRMLSceneNodeAdded(vtkMRMLNode *node);

  /**
   * Observe the interesting events for the given node
   *
   * @param node pointer to a resection node to be observed.
   */
  void SetAndObserveNode(vtkMRMLResectionSurfaceNode *node);

 private:
  vtkMRMLResectionDisplayableManager2D(
    const vtkMRMLResectionDisplayableManager2D&);
  void operator=(const vtkMRMLResectionDisplayableManager2D&);

};

#endif