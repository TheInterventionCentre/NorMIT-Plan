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

class vtkPointSource;
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

    /*
     * Set if are in mode to place seeds
     *
     * @param bool where true is placing seeds
     */
    static void SetSeedsMode(bool seedMode);
    
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
     * Callback controlling actions on node modified.
     *
     * @param node pointer to the modified node.
     */
    virtual void OnMRMLNodeModified(vtkMRMLNode *node);

    /**
     * Callback controlling actions on node removed from scene.
     *
     * @param node pointer to the node removed from the scene.
     */
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode *node);

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
     * Creates the projection of the seed onto the slice
     *
     * @param node seed node holding the position of the seed
     *
     * @return true if the seed was successfully created, false otherwise.
     */
    bool AddSeed(vtkMRMLVesselSegmentationSeedNode *node);

    /**
     * Update the visibility of the projection of the seed node.
     *
     * @param node pointer to seed node to be used for the update.
     */
    void UpdateVisibility(vtkMRMLVesselSegmentationSeedNode *node);

    /**
     * Observe the slice node for modifications.
     *
     * @param pointer to a slice node.
     */
    void SetAndObserveSliceNode(vtkMRMLSliceNode *node);
    
    // Map SeedNode -- PolygonSurfaceSource
    std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkPointSource> > SeedMap;
    typedef std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkPointSource> >::iterator SeedIt;

    //Map SeedNode -- Mapper
    std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkPolyDataMapper2D> > SeedMapperMap;
    typedef std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkPolyDataMapper2D> >::iterator SeedMapperIt;

    // Map seedNode -- Actor
    std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkActor2D> > SeedActorMap;
    typedef std::map<vtkMRMLVesselSegmentationSeedNode*,
      vtkSmartPointer<vtkActor2D> >::iterator SeedActorIt;

  private:
    vtkMRMLVesselSegmentationDisplayableManager2D(
      const vtkMRMLVesselSegmentationDisplayableManager2D&);
    void operator=(const vtkMRMLVesselSegmentationDisplayableManager2D&);

    static bool placingSeeds;
};

#endif /* defined(__VesselSeg__vtkMRMLVesselSegmentationDisplayableManager_h) */
