/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionInitializationDisplayableManager3D.h

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

#ifndef __vtkMRMLResectionInitializationDisplayableManager3D_h
#define __vtkMRMLResectionInitializationDisplayableManager3D_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLDisplayableManagerExport.h"

// MRML includes
#include <vtkMRMLAbstractThreeDViewDisplayableManager.h>

// VTK includes
#include <vtkSmartPointer.h>

// STD includes
#include <map>

//------------------------------------------------------------------------------
class vtkMRMLResectionInitializationNode;
class vtkCollection;
class vtkLineWidget3;

//------------------------------------------------------------------------------
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLResectionInitializationDisplayableManager3D:
public vtkMRMLAbstractThreeDViewDisplayableManager
{
 public:

  /**
   * Standard VTK object creation macro.
   *
   *
   * @return a pointer to the new created object.
   */
  static vtkMRMLResectionInitializationDisplayableManager3D* New();

  vtkTypeMacro(vtkMRMLResectionInitializationDisplayableManager3D,
               vtkMRMLAbstractThreeDViewDisplayableManager);

  /**
   * Standard VTK print information.
   *
   * @param os output stream to print the information to.
   * @param indent indent value.
   */
  void PrintSelf(ostream &os, vtkIndent indent);

  /**
   * Process MRML nodes events
   *
   * @param object object which triggered the event.
   * @param eventId Id of the event.
   * @param data additional data.
   *
   */
  void ProcessMRMLNodesEvents(vtkObject *object,
                             unsigned long int eventId,
                             void *data);
 protected:
  vtkMRMLResectionInitializationDisplayableManager3D();
  ~ vtkMRMLResectionInitializationDisplayableManager3D();

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
  void SetAndObserveNode(vtkMRMLResectionInitializationNode* node);

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
   * Add a new widget to the scene
   *
   * @param node pointer to mrml node which will be used as a base for the
   * widget creation.
   *
   * @return true if the widget was added, false otherwise.
   */
  bool AddWidget(vtkMRMLResectionInitializationNode *node);

  /**
   * Update MRML node according to the widget interaction
   *
   * @param caller object triggering the event (typically the widget).
   * @param id Identification of the triggered event.
   * @param clientData Additional data
   * @param callerData Additional data (not used).
   */
  static void UpdateMRML(vtkObject *caller,
                         unsigned long int id,
                         void *clientData,
                         void *callerData);

  /**
   * Update the visibility properties of the widget based on the node
   * information.
   *
   * @param node pointer to a resection initialization node to update the
   * visibility from.
   */
  void UpdateVisibility(vtkMRMLResectionInitializationNode *node);

  /**
   * Update the line widget
   *
   * @param node pointer to a resection initialization node to update the
   * visibility from.
   */
  void UpdateLineWidget(vtkMRMLResectionInitializationNode *node);


 private:
    vtkMRMLResectionInitializationDisplayableManager3D(
      const vtkMRMLResectionInitializationDisplayableManager3D&);
    void operator=(const vtkMRMLResectionInitializationDisplayableManager3D&);

    // Map holding the association between node and widget.
    std::map<vtkMRMLResectionInitializationNode*,
      vtkSmartPointer<vtkLineWidget3> > NodeWidgetMap;
    typedef std::map<vtkMRMLResectionInitializationNode*,
      vtkSmartPointer<vtkLineWidget3> >::iterator
      NodeWidgetIt;
};

#endif
