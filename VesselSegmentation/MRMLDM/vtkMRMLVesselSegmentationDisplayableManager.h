//
//  SlicerVesselSegmentationDisplayableManager.h
//  VesselSeg
//
//  Created by Louise Oram on 2015-10-12.
//
//

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
