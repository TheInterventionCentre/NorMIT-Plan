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
bool vtkMRMLVesselSegmentationDisplayableManager::placingFiducials;

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
    placingFiducials = false;
  }
}

void vtkMRMLVesselSegmentationDisplayableManager::
OnMRMLSceneNodeAdded(vtkMRMLNode *addedNode)
{
  vtkMRMLMarkupsFiducialNode* tempFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(addedNode);

  if(tempFiducialNode != NULL) {

    // now observing all node modified events (calls: OnMRMLNodeModified)
    this->UpdateFiducialCommand->SetCallback(this->OnFiducialNodeModified);
    this->UpdateFiducialCommand->SetClientData(this);
    tempFiducialNode->AddObserver(vtkCommand::ModifiedEvent, this->UpdateFiducialCommand.GetPointer(), -100.0);

    vtkSmartPointer<vtkMRMLSliceNode> tempSliceNode = this->GetMRMLSliceNode();

    if((this->sliceListenerSet == false) && (tempSliceNode != NULL)) {

      this->RAStoXYmatrix->DeepCopy(tempSliceNode->GetXYToRAS());
      this->RAStoXYmatrix->Invert();

//      std::cout << "DM - Have RAS to XY matrix: " << this->RAStoXYmatrix << std::endl;

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
  }
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::
OnMRMLNodeModified(vtkMRMLNode *vtkNotUsed(modifiedNode))
{
  /*
   gets called when new fiducials are added, but before its fully added, so the list is not updated... had to use other callback method
   */
//  std::cout << "DM - Node modified" << std::endl;

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

void vtkMRMLVesselSegmentationDisplayableManager::OnFiducialNodeModified(vtkObject *vtkNotUsed(caller),
                                                                         unsigned long int vtkNotUsed(id),
                                                                         void *clientData,
                                                                         void *vtkNotUsed(callerData))
{
  vtkMRMLVesselSegmentationDisplayableManager* DM = reinterpret_cast<vtkMRMLVesselSegmentationDisplayableManager*>(clientData);

  std::vector<double*> list = vtkSlicerVesselSegmentationLogic::GetFiducialList();
  int s = list.size();

//  std::cout << "DM - Fiducial node modified: " << s << std::endl;

  if(s > 0) {
    double *coords = list[s-1];

    // use the xy matrix to convert the points
    double *fidXY = new double[4];
    DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords, fidXY);

    if(s > 1) {
      double *coords2 = list[s-2];
      double *fid2XY = new double[4];
      DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords2, fid2XY);
      //DM->DisplayPolygon(fid2XY, 2);
    }

    //DM->DisplayPolygon(fidXY, 1);
  }
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

  std::vector<double*> list = vtkSlicerVesselSegmentationLogic::GetFiducialList();
  int s = list.size();

  // this only matters if on odd # of fiducial
  if((s > 0) && (s % 2 == 1)) {
    double *coords = list[s-1];

    //std::cout << "Line adjusted from inside crosshair modified" << std::endl;
    // use the xy matrix to convert the point
    double *fidXY = new double[4];
    DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords, fidXY);

    //DM->DisplayLine(fidXY, curXY);
    //DM->DisplayPolygon(fidXY,1);

    if(s > 1) {
      double *coords2 = list[s-2];
      double *fid2XY = new double[4];
      DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords2, fid2XY);
      //DM->DisplayPolygon(fid2XY, 2);
    }
  }
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

//    std::cout << "DM - Updated RAS to XY matrix (inside slice node modified)" << std::endl;
  }

  std::vector<double*> list = vtkSlicerVesselSegmentationLogic::GetFiducialList();
  int s = list.size();

  // if on odd # of fiducials then crosshair modified will take care of this
  // here we must deal with the even # of fiducials
  if((s > 1) && (s%2 == 0)) {

    double *coords = list[s-1];
    double *coords2 = list[s-2];
    //std::cout << "Line & polygon adjusted from inside update line" << std::endl;

    // use the XY matrix to convert the points
    double *fidXY = new double[4];
    DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords, fidXY);

    double *fid2XY = new double[4];
    DM->RAStoXYmatrix.GetPointer()->MultiplyPoint(coords2, fid2XY);

    //DM->DisplayPolygon(fidXY, 1);
    //DM->DisplayPolygon(fid2XY, 2);
    //DM->DisplayLine(fidXY, fid2XY);
  }
}

void vtkMRMLVesselSegmentationDisplayableManager::DisplayLine(double *coord1, double *coord2)
{
//  std::cout << "Draw line" << std::endl;
  if(placingFiducials == false)
  {
//    std::cout << "not placing fiducials" << std::endl;
    this->GetRenderer()->RemoveActor(this->lineActor.GetPointer());
  }
  else
  {
	if((coord1 != NULL) && (coord2 != NULL)) {
		vtkSmartPointer<vtkLineSource> tempLine = vtkSmartPointer<vtkLineSource>::New();
		//vtkLineSource *tempLine = vtkLineSource::New();

		tempLine->SetPoint1(coord1[0],coord1[1],coord1[2]);
		tempLine->SetPoint2(coord2[0],coord2[1],coord2[2]);

	    //Set up mapper and actor for the line
		vtkSmartPointer<vtkPolyDataMapper2D> lineMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
		//vtkPolyDataMapper2D *lineMapper = vtkPolyDataMapper2D::New();
	    lineMapper->SetInputConnection(tempLine->GetOutputPort());

	    this->lineActor->SetMapper(lineMapper);
	    this->lineActor->GetProperty()->SetLineWidth(4);
	    this->lineActor->GetProperty()->SetColor(0,1,0);

	    // add actors to the scene
	    this->GetRenderer()->AddActor2D(this->lineActor.GetPointer());
	}
  }
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::DisplayPolygon(double *coord, int w)
{
//  std::cout << "Draw polygon" << std::endl;
  if(placingFiducials == false)
  {
//    std::cout << "not placing fiducials" << std::endl;
    this->GetRenderer()->RemoveActor(this->polygon1Actor.GetPointer());
    this->GetRenderer()->RemoveActor(this->polygon2Actor.GetPointer());
  }
  else
  {
	 if(coord != NULL)
	 {
		 vtkSmartPointer<vtkRegularPolygonSource> tempPolygon = vtkSmartPointer<vtkRegularPolygonSource>::New();
		 //vtkRegularPolygonSource *tempPolygon = vtkRegularPolygonSource::New();

	     tempPolygon->SetNumberOfSides(5);
	     tempPolygon->SetRadius(10.0);
	     tempPolygon->SetCenter(coord[0],coord[1],coord[2]);

	    //Set up mapper and actor for the polygon
	    vtkSmartPointer<vtkPolyDataMapper2D> polygonMapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
	    // vtkPolyDataMapper2D *polygonMapper = vtkPolyDataMapper2D::New();
	    polygonMapper->SetInputConnection(tempPolygon->GetOutputPort());

	    if(w==1)
	    {
			this->polygon1Actor->SetMapper(polygonMapper);
			this->GetRenderer()->AddActor2D(this->polygon1Actor.GetPointer());
	    }
	    else if (w==2)
	    {
	    	this->polygon2Actor->SetMapper(polygonMapper);
		    this->GetRenderer()->AddActor2D(this->polygon2Actor.GetPointer());
	    }


	 }
  }
  this->RequestRender();
}

void vtkMRMLVesselSegmentationDisplayableManager::SetFiducialsMode( bool placingFid )
{
    placingFiducials = placingFid;
//    std::cout << "DM - Placing fiducials bool: " << placingFiducials << std::endl;
}
