/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationDisplayableManager2D.h

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

#ifndef __VesselSeg__vtkMRMLVesselSegmentationDisplayableManager2D_h
#define __VesselSeg__vtkMRMLVesselSegmentationDisplayableManager2D_h

// MRML includes
#include <vtkMRMLAbstractSliceViewDisplayableManager.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

//Slicer includes
#include "vtkSlicerVesselSegmentationModuleMRMLDisplayableManagerExport.h"

class vtkRegularPolygonSource;
class vtkCallbackCommand;
class vtkMatrix4x4;
class vtkActor2D;
class vtkPolyDataMapper2D;
class vtkMRMLVesselSegmentationSeedNode;

/**
 * \ingroup VesselSegmentation
 *
 * This class encapsulates the functionality to display the projection of the
 * seeds in the 2D slices.
 */
class VTK_SLICER_VESSELSEGMENTATION_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLVesselSegmentationDisplayableManager2D
:public vtkMRMLAbstractSliceViewDisplayableManager
{
  public:
   /**
    * Standard VTK object creation macro.
    *
    * @return a pointer to the new created object.
    */
    static vtkMRMLVesselSegmentationDisplayableManager2D *New();
    vtkTypeMacro(vtkMRMLVesselSegmentationDisplayableManager2D,
                 vtkMRMLAbstractSliceViewDisplayableManager);

    /**
     * Standard VTK print information.
     *
     * @param os output stream to print the information to.
     * @param indent indent value.
     */
    void PrintSelf(ostream& os, vtkIndent indent);
    
  protected:
    vtkMRMLVesselSegmentationDisplayableManager2D();
    virtual ~vtkMRMLVesselSegmentationDisplayableManager2D();
    
    /**
     * Sets a new scene.
     *
     * @param newScene pointer to the new scene.
     */
    virtual void SetMRMLSceneInternal(vtkMRMLScene *newScene);
    
    /**
     * Controls what happens when the MRML scene is closed
     */
    virtual void OnMRMLSceneEndClose();

    /**
     * Callback controlling actions on new node added.
     *
     * @param node pointer to the new node added to the scene.
     */
    virtual void OnMRMLSceneNodeAdded(vtkMRMLNode *node);

    /**
     * Callback controlling actions on node removed from scene.
     *
     * @param node pointer to the node removed from the scene.
     */
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode *node);

    /**
     * Render the scene due to changes in MRML scene.
     *
     */
    void UpdateFromMRMLScene();

    /**
     * Update all the seed projections based on the seed nodes
     *
     */
    void UpdateFromMRML();

    /**
     * Process MRML nodes events
     *
     * @param object object which triggered the event.
     * @param eventId Id of the event.
     * @param data additional data.
     *
     */
    virtual void ProcessMRMLNodesEvents(vtkObject *caller,
                                        unsigned long event,
                                        void *callData);
    
    /**
     * Observe the interesting events for the given node.
     *
     * @param node pointer to a seed node to be observed.
     */
    void SetAndObserveSeedNode(vtkMRMLVesselSegmentationSeedNode *node);

    /**
     * Observe the slice node for modifications.
     *
     * @param pointer to a slice node.
     */
    void SetAndObserveSliceNode(vtkMRMLSliceNode *node);
    
    /**
     * Respond to interactor events, particularly we are interested in
     * when the user clicks on one of the slice views.
     *
     * @param event id.
     */
    virtual void OnInteractorEvent(int eventid) VTK_OVERRIDE;

    /**
     * Creates the seed representation
     *
     * @param node seed node holding the geometry of the seed
     *
     * @return true if the representation was successfully created, false otherwise.
     */
    bool AddRepresentation(vtkMRMLVesselSegmentationSeedNode *node);

    /**
     * Update the position of the seed when image is scaled.
     *
     * @param node pointer to seed node to be used for the update.
     */
    void UpdatePositionOnSlice(vtkMRMLVesselSegmentationSeedNode *node);

    /**
     * Updates if the seed representation is visible on the current slice
     *
     * @param node seed node holding the geometry of the seed
     */
    void UpdateVisibilityOnSlice(vtkMRMLVesselSegmentationSeedNode *node);

  private:
    vtkWeakPointer<vtkMRMLVesselSegmentationSeedNode> currentSeedNode;

    // PolygonSurfaceSource
    vtkSmartPointer<vtkRegularPolygonSource> Seed1Source;
    vtkSmartPointer<vtkRegularPolygonSource> Seed2Source;
    // Mapper
    vtkSmartPointer<vtkPolyDataMapper2D> Seed1Mapper;
    vtkSmartPointer<vtkPolyDataMapper2D> Seed2Mapper;
    //  Actor
    vtkSmartPointer<vtkActor2D> Seed1Actor;
    vtkSmartPointer<vtkActor2D> Seed2Actor;

    double* GetCrosshairPosition();

    vtkMRMLVesselSegmentationDisplayableManager2D(
      const vtkMRMLVesselSegmentationDisplayableManager2D&);
    void operator=(const vtkMRMLVesselSegmentationDisplayableManager2D&);
};

#endif /* defined(__VesselSeg__vtkMRMLVesselSegmentationDisplayableManager_h) */
