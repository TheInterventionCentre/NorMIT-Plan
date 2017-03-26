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

// VTK includes
#include <vtkSmartPointer.h>

//------------------------------------------------------------------------------
class vtkMRMLScene;
class vtkMRMLResectionSurfaceNode;
class vtkActor2D;
class vtkBezierSurfaceSource;
class vtkCutter;
class vtkTransformPolyDataFilter;

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
   * Actions to be performed when the MRML scene is going to close.
   *
   */
  void OnMRMLSceneEndClose();

  /**
   * Actions when a node gets added to the mrml scene.
   *
   * @param node node added to the scene.
   */
  void OnMRMLSceneNodeAdded(vtkMRMLNode *node);

  /**
   * Actiosn when a node gets removed from the mrml scene.
   *
   * @param node pointer to a resection node to be removed.
   */
  void OnMRMLSceneNodeRemoved(vtkMRMLNode *node);

  /**
   * Observe the interesting events for the given node.
   *
   * @param node pointer to a resection node to be observed.
   */
  void SetAndObserveNode(vtkMRMLResectionSurfaceNode *node);

  /**
   * Render the scene due to changes in MRML scene.
   *
   */
  void UpdateFromMRMLScene();

  /**
   * Update all the resection projections based on the resection nodes
   *
   */
  void UpdateFromMRML();

  /**
   * Actions to be performed on observed nodes.
   *
   * @param object node triggering the action.
   * @param eventId id of the triggered event.
   * @param callData extra data (not used).
   */
  void ProcessMRMLNodesEvents(vtkObject *object,
                              unsigned long int eventId,
                              void *callData);

  /**
   * Creates the projection of the resection onto the slice
   *
   * @param node resection node holding the geometry of the resection
   *
   * @return true if the representation was successfully created, false otherwise.
   */
  bool AddRepresentation(vtkMRMLResectionSurfaceNode *node);

  /**
   * Update the geometry of the projection of the resection surface.
   *
   * @param node pointer to resection node to be used for the update.
   */
  void UpdateGeometry(vtkMRMLResectionSurfaceNode *node);

  /**
   * Update the visibilty of the projection of the resection surface.
   *
   * @param node poitner to resection node to be used for the update.
   */
  void UpdateVisibility(vtkMRMLResectionSurfaceNode *node);


  // Map ResectionNode -- BézierSurfaceSource
  std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkBezierSurfaceSource> > ResectionBezierMap;
  typedef std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkBezierSurfaceSource> >::iterator ResectionBezierIt;

  // Map ResectionNode -- Cutter
  std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkCutter> >  ResectionCutterMap;
  typedef std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkCutter> >::iterator ResectionCutterIt;

  // Map ResectionNode -- Transform Filter
  std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkTransformPolyDataFilter> >
    ResectionTransformFilterMap;
  typedef std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkTransformPolyDataFilter> >::iterator
    ResectionTransformFilterIt;

  // Map ResectionNode -- Actor
  std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkActor2D> > ResectionActorMap;
  typedef std::map<vtkMRMLResectionSurfaceNode *,
    vtkSmartPointer<vtkActor2D> >::iterator ResectionActorIt;

 private:
  vtkMRMLResectionDisplayableManager2D(
    const vtkMRMLResectionDisplayableManager2D&);
  void operator=(const vtkMRMLResectionDisplayableManager2D&);
};

#endif
