/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationDisplayableManager.h

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

#ifndef __VesselSeg__vtkMRMLVesselSegmentationDisplayableManager_h
#define __VesselSeg__vtkMRMLVesselSegmentationDisplayableManager_h

// MRML includes
#include <vtkMRMLAbstractSliceViewDisplayableManager.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

//Slicer includes
#include "vtkSlicerVesselSegmentationModuleMRMLDisplayableManagerExport.h"

class vtkCallbackCommand;
class vtkMatrix4x4;
class vtkActor2D;

/**
 * This class contains the display related functions, as well as event triggered functions related to the display.
 * This class handles the drawing of the line between two fiducials, and other vtk rendered visuals.
 */
class VTK_SLICER_VESSELSEGMENTATION_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLVesselSegmentationDisplayableManager
:public vtkMRMLAbstractSliceViewDisplayableManager
{
  public:
    static vtkMRMLVesselSegmentationDisplayableManager *New();
    vtkTypeMacro(vtkMRMLVesselSegmentationDisplayableManager,
                 vtkMRMLAbstractSliceViewDisplayableManager);
    void PrintSelf(ostream& os, vtkIndent indent);
    static void SetFiducialsMode(bool placingFid);
    
  protected:
    vtkMRMLVesselSegmentationDisplayableManager();
    virtual ~vtkMRMLVesselSegmentationDisplayableManager();
    
    // MRML virtual functions
    virtual void ProcessMRMLNodesEvents(vtkObject *caller,
                                        unsigned long event,
                                        void *callData);
    virtual void SetMRMLSceneInternal(vtkMRMLScene *newScene);
    
    virtual void OnMRMLSceneNodeAdded(vtkMRMLNode *node);
    virtual void OnMRMLNodeModified(vtkMRMLNode *node);
    virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode *node);
    virtual void OnMRMLSceneEndClose();
    
    static void OnFiducialNodeModified(vtkObject *caller, unsigned long int id, void *clientData, void *callerData);
    
    static void OnCrosshairPositionModified(vtkObject *caller, unsigned long int id, void *clientData, void *callerData);
  
    static void OnSliceNodeModified(vtkObject *caller, unsigned long int id, void *clientData, void *callerData);

    void DisplayLine(double *coord1, double *coord2);
    void DisplayPolygon(double *coord, int w);

    vtkNew<vtkMatrix4x4> RAStoXYmatrix;
    
  private:
    vtkNew<vtkActor2D> lineActor;
    vtkNew<vtkActor2D> polygon1Actor;
    vtkNew<vtkActor2D> polygon2Actor;

    vtkNew<vtkCallbackCommand> UpdateFiducialCommand;
    vtkNew<vtkCallbackCommand> UpdateMatrixCommand;
    vtkNew<vtkCallbackCommand> UpdateCursorCommand;

    bool sliceListenerSet;
    static bool placingFiducials;
    double *lastCursorPosition;
};

#endif /* defined(__VesselSeg__vtkMRMLVesselSegmentationDisplayableManager_h) */
