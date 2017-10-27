/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationDisplayableManager.cxx

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

#include "vtkMRMLVesselSegmentationDisplayableManager.h"
#include "vtkSlicerVesselSegmentationLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLAbstractLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkMRMLCrosshairNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkRenderer.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include <vtkCallbackCommand.h>
#include <vtkEventBroker.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkLineSource.h>
#include <vtkRegularPolygonSource.h>

#include <vtkSlicerModelsLogic.h>

#include <iostream>
#include <cstdlib>
#include <vector>


vtkStandardNewMacro(vtkMRMLVesselSegmentationDisplayableManager);
bool vtkMRMLVesselSegmentationDisplayableManager::placingSeeds;

//-------------------------------------------------------------------------------
vtkMRMLVesselSegmentationDisplayableManager::
vtkMRMLVesselSegmentationDisplayableManager()
{

}

vtkMRMLVesselSegmentationDisplayableManager::
~vtkMRMLVesselSegmentationDisplayableManager()
{

}

void vtkMRMLVesselSegmentationDisplayableManager::PrintSelf(ostream &os,
                                                            vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//-------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager::
ProcessMRMLNodesEvents(vtkObject *caller,
                       unsigned long event,
                       void *callData)
{
  Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::
SetMRMLSceneInternal(vtkMRMLScene *newScene)
{
  this->OnMRMLSceneEndClose();
  Superclass::SetMRMLSceneInternal(newScene);
  if(newScene)
  {
    sliceListenerSet = false;
    placingSeeds = false;
  }
}

void vtkMRMLVesselSegmentationDisplayableManager::
OnMRMLSceneNodeAdded(vtkMRMLNode *addedNode)
{
  // TODO: observe all seed nodes added to scene

  vtkSmartPointer<vtkMRMLSliceNode> tempSliceNode = this->GetMRMLSliceNode();

  if((this->sliceListenerSet == false) && (tempSliceNode != NULL)) {

    this->RAStoXYmatrix->DeepCopy(tempSliceNode->GetXYToRAS());
    this->RAStoXYmatrix->Invert();

    //std::cout << "DM - Have RAS to XY matrix: " << this->RAStoXYmatrix << std::endl;

    sliceListenerSet = true;

    // create callback
    this->UpdateMatrixCommand->SetCallback(this->OnSliceNodeModified);
    this->UpdateMatrixCommand->SetClientData(this);

    tempSliceNode->AddObserver(vtkCommand::ModifiedEvent, this->UpdateMatrixCommand.GetPointer());
  }

  /* Observe crosshair node */
  vtkMRMLNode* tempCrosshairNodeDefault = this->GetMRMLScene()->GetNodeByID("vtkMRMLCrosshairNodedefault");
  vtkMRMLCrosshairNode* tempCrosshairNode = vtkMRMLCrosshairNode::SafeDownCast(tempCrosshairNodeDefault);

  this->UpdateCursorCommand->SetCallback(this->OnCrosshairPositionModified);
  this->UpdateCursorCommand->SetClientData(this);
  tempCrosshairNode->AddObserver(vtkMRMLCrosshairNode::CursorPositionModifiedEvent, this->UpdateCursorCommand.GetPointer());
  /* ----------------------- */

  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::
OnMRMLNodeModified(vtkMRMLNode *vtkNotUsed(modifiedNode))
{
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::
OnMRMLSceneNodeRemoved(vtkMRMLNode *vtkNotUsed(node))
{
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::OnMRMLSceneEndClose()
{

}

void vtkMRMLVesselSegmentationDisplayableManager::
OnCrosshairPositionModified(vtkObject *caller,
                            long unsigned int vtkNotUsed(eventId),
                            void *clientData,
                            void *vtkNotUsed(callData))
{
  vtkMRMLVesselSegmentationDisplayableManager* DM = reinterpret_cast<vtkMRMLVesselSegmentationDisplayableManager*>(clientData);

  vtkMRMLCrosshairNode* tempCrosshairNode = vtkMRMLCrosshairNode::SafeDownCast(caller);

  double *position = new double[3];
  tempCrosshairNode->GetCursorPositionRAS(position);

  double *pos2 = new double[4];
  pos2[0] = position[0];
  pos2[1] = position[1];
  pos2[2] = position[2];
  pos2[3] = 1;
  double *curXY = new double[4];
  DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(pos2, curXY);
  DM->lastCursorPosition = curXY;

  //std::cout << "cursorPositionModified: " << curXY[0] << " " << curXY[1] << " " << curXY[2] << std::endl;

}

void vtkMRMLVesselSegmentationDisplayableManager::OnSliceNodeModified(vtkObject *vtkNotUsed(caller),
                                                                      unsigned long int vtkNotUsed(id),
                                                                      void *clientData,
                                                                      void *vtkNotUsed(callerData))
{
  vtkMRMLVesselSegmentationDisplayableManager* DM = reinterpret_cast<vtkMRMLVesselSegmentationDisplayableManager*>(clientData);

  vtkSmartPointer<vtkMRMLSliceNode> tempSliceNode = DM->GetMRMLSliceNode();

  if(tempSliceNode != NULL) {

    DM->RAStoXYmatrix->DeepCopy(tempSliceNode->GetXYToRAS());
    DM->RAStoXYmatrix->Invert();

  // std::cout << "DM - Updated RAS to XY matrix (inside slice node modified)" << std::endl;
  }

}

void vtkMRMLVesselSegmentationDisplayableManager::SetSeedsMode( bool seedMode )
{
  placingSeeds = seedMode;
  //std::cout << "DM - Placing fiducials bool: " << placingFiducials << std::endl;
}
