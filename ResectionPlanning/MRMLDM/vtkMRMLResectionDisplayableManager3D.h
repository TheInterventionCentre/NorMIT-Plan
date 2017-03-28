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

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>

//------------------------------------------------------------------------------
class vtkMRMLResectionSurfaceNode;
class vtkBezierSurfaceWidget;
class vtk3DWidget;
class vtkHausdorffDistancePointSetFilter;

//------------------------------------------------------------------------------
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLResectionDisplayableManager3D :
public vtkMRMLAbstractThreeDViewDisplayableManager
{
 public:

  /**
   * Standard VTK object creation macro.
   *
   *
   * @return a pointer to the new created object.
   */
  static vtkMRMLResectionDisplayableManager3D *New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager3D,
               vtkMRMLAbstractThreeDViewDisplayableManager);

  /**
   * Standard VTK print information.
   *
   * @param os output stream to print the information to.
   * @param indent indent value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);

  /**
   * This method creates a new widget for the resection surface node and
   * registers it with the helper.
   *
   * @param node base node to create the widget from.
   *
   * @return true if the widget was successfully added, false otherwise.
   */
  bool AddWidget(vtkMRMLResectionSurfaceNode *node);

 protected:
  vtkMRMLResectionDisplayableManager3D();
  virtual ~vtkMRMLResectionDisplayableManager3D();

  /**
   * Sets a new scene.
   *
   * @param newScene pointer to the new scene.
   */
  virtual void SetMRMLSceneInternal(vtkMRMLScene *newScene);

  /**
   * Sets and observes a resection node.
   *
   * @param node pointer to the node to be observed.
   */
  void SetAndObserveNode(vtkMRMLResectionSurfaceNode* node);

  /**
   * Process MRML nodes events
   *
   * @param object node triggering the event.
   * @param eventId id of the event triggered.-
   * @param data associated data to the event.
   */
  virtual void ProcessMRMLNodesEvents(vtkObject *object,
                                      unsigned long int eventId,
                                      void *data);

  /**
   * Update manager based on the MRML node if UpdateFromMRMLRequested is true
   * \sa RequestRender() SetUpdateFromMRMLRequested().
   *
   */
  virtual void UpdateFromMRML();

  /**
   * Update manager from MRMLScene.
   *
   */
  virtual void UpdateFromMRMLScene();

  /**
   * Controls what happens when the MRML scene is clsoed
   *
   */
  virtual void OnMRMLSceneEndClose();

  /**
   * Callback controlling actions on new node added.
   *
   * @param node pointer to the new node added to the scene.
   */
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);

  /**
   * Callback controlling actions on node removed from scene.
   *
   * @param node pointer to the node removed from the scene.
   */
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  /**
   * Update the geometry of the Bézier surface and the control points.
   *
   * @param pointer to vtkMRMLResectionSurfaceNode to update.
   */
  void UpdateGeometry(vtkMRMLResectionSurfaceNode *node);

  /**
   * Update visibilty property of the models
   *
   * @param pointer to vtkMRMLResectionSurfaceNode to update.
   */
  void UpdateVisibility(vtkMRMLResectionSurfaceNode *node);

  /**
   * Update MRML node values based on modifications from interactions with the
   * widget.
   *
   * @param object pointer to widget triggering the event.
   * @param eventId id of the event.
   * @param clientData pointer to surface node to be modified.
   * @param callData Not used.
   *
   */
  static void UpdateMRML(vtkObject *object,
                         unsigned long int eventId,
                         void *clientData,
                         void *callData);
 private:

  // Description:
  // Copy constructor and copy operator
  vtkMRMLResectionDisplayableManager3D(const vtkMRMLResectionDisplayableManager3D&);
  void operator=(const vtkMRMLResectionDisplayableManager3D&);

  // Map and iterator holding ResectionNode-BézierWidget relationship.
  std::map<vtkMRMLResectionSurfaceNode*,
    vtkSmartPointer<vtkBezierSurfaceWidget> > NodeWidgetMap;
  typedef std::map<vtkMRMLResectionSurfaceNode*,
    vtkSmartPointer<vtkBezierSurfaceWidget> >::iterator NodeWidgetIt;

  // Map and iterator holding the ResecionNode-DistanceFilter relationship
  std::map<vtkMRMLResectionSurfaceNode*,
    vtkSmartPointer<vtkHausdorffDistancePointSetFilter> > NodeDistanceFilterMap;
  std::map<vtkMRMLResectionSurfaceNode*,
    vtkSmartPointer<vtkHausdorffDistancePointSetFilter> >::iterator NodeDistanceFilterIt;

  // Map and iterator holding the ResectionNode-ContourFilter relationship

};

#endif
