/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationDisplayableManager2D.cxx

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

#include "vtkSlicerVesselSegmentationLogic.h"
#include "vtkMRMLVesselSegmentationSeedNode.h"
#include "qSlicerVesselSegmentationModuleWidget.h"
#include "vtkMRMLVesselSegmentationDisplayableManager2D.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLAbstractLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLCrosshairNode.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLDisplayNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkRenderer.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkMatrix4x4.h>

#include <vtkCallbackCommand.h>
#include <vtkInteractorObserver.h>
#include <vtkEventBroker.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkLineSource.h>
#include <vtkRegularPolygonSource.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceView.h>

#include <vtkSliceViewInteractorStyle.h>
#include <vtkInteractorStyle.h>

#include <iostream>
#include <cstdlib>
#include <vector>


vtkStandardNewMacro(vtkMRMLVesselSegmentationDisplayableManager2D);

//-------------------------------------------------------------------------------
vtkMRMLVesselSegmentationDisplayableManager2D::
vtkMRMLVesselSegmentationDisplayableManager2D()
{
  this->AddInteractorObservableEvent(vtkCommand::LeftButtonReleaseEvent);
}

//------------------------------------------------------------------------------
vtkMRMLVesselSegmentationDisplayableManager2D::
~vtkMRMLVesselSegmentationDisplayableManager2D()
{
  // TODO: is there anything that needs deleting?

}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::PrintSelf(ostream &os,
                                                            vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
SetMRMLSceneInternal(vtkMRMLScene *newScene)
{
  Superclass::SetMRMLSceneInternal(newScene);

  //this->OnMRMLSceneEndClose();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");

  /*
  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer");
    return;
    }
    */

  if(this->Seed1Actor != NULL)
    {
    this->GetRenderer()->RemoveActor(this->Seed1Actor);
    }
  if(this->Seed2Actor != NULL)
    {
    this->GetRenderer()->RemoveActor(this->Seed2Actor);
    }

  this->SetUpdateFromMRMLRequested(1);
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
OnMRMLSceneNodeAdded(vtkMRMLNode* addedNode)
{
  vtkDebugMacro("OnMRMLSceneNodeAdded");

  if (!addedNode)
    {
    vtkErrorMacro("No node passed");
    return;
    }

  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene has been set");
    return;
    }

  // Markups does not check for this?
  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  // Markups does not check for this?
  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node present");
    return;
    }

  // Jump out if scene is batch processing
  if (this->GetMRMLScene()->IsBatchProcessing())
    {
    return;
    }

  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast(addedNode);
  vtkMRMLSliceNode *sliceNode =
    vtkMRMLSliceNode::SafeDownCast(addedNode);

  // Check if this is a seed node.
  if(seedNode)
    {
    this->SetAndObserveSeedNode(seedNode);
    // store the current seed node pointer
    this->currentSeedNode = seedNode;
    std::cout << "DM - Current seed node: " << seedNode << std::endl;
    }
  // Check if this is a slice node.
  else if (sliceNode)
    {
    this->SetAndObserveSliceNode(sliceNode);
    char *name = sliceNode->GetID();
    std::cout << "DM - Slice node: " << name << std::endl;
    }

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
OnMRMLSceneNodeRemoved(vtkMRMLNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    }

  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast(node);

  vtkMRMLSliceNode *sliceNode =
    vtkMRMLSliceNode::SafeDownCast(node);

  if(!seedNode && !sliceNode)
    {
    return;
    }
  else if (seedNode)
    {
    // delete the current seed node pointer
    this->currentSeedNode = NULL;

    if(this->Seed1Actor != NULL)
      {
      this->GetRenderer()->RemoveActor(this->Seed1Actor);
      this->Seed1Actor = NULL;
      }
    if(this->Seed2Actor != NULL)
      {
      this->GetRenderer()->RemoveActor(this->Seed2Actor);
      this->Seed2Actor = NULL;
      }

    if(this->Seed1Mapper != NULL)
      {
      this->Seed1Mapper = NULL;
      }
    if(this->Seed2Mapper != NULL)
      {
      this->Seed2Mapper = NULL;
      }

    if(this->Seed1Source != NULL)
      {
      this->Seed1Source = NULL;
      }
    if(this->Seed2Source != NULL)
      {
      this->Seed2Source = NULL;
      }
    // Remove the observations from the node
    vtkUnObserveMRMLNodeMacro(seedNode);
    }
  else if (sliceNode)
    {
    // Remove the observations from the node
    vtkUnObserveMRMLNodeMacro(sliceNode);
    }

  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::UpdateFromMRMLScene()
{
  vtkDebugMacro("UpdateFromMRMLScene");
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::UpdateFromMRML()
{
  vtkDebugMacro("UpdateFromMRML");

  vtkMRMLScene *scene = this->GetMRMLScene();
  if (!scene)
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }

  if ( this->GetInteractor() &&
       this->GetInteractor()->GetRenderWindow() &&
       this->GetInteractor()->GetRenderWindow()->CheckInRenderStatus())
    {
    vtkDebugMacro("skipping update during render");
    return;
    }
}

//-------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
ProcessMRMLNodesEvents(vtkObject *caller,
                       unsigned long event,
                       void *callData)
{
  /*
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }
    */

  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast(caller);

  vtkMRMLSliceNode *sliceNode =
    vtkMRMLSliceNode::SafeDownCast(caller);

  if (seedNode)
    {
    bool seed1Set = seedNode->GetIsSeed1Set();
    bool seed2Set = seedNode->GetIsSeed2Set();
    switch(event)
      {
      std::cout << "DM - seed node event: ModifiedEvent " << std::endl;
      case vtkCommand::ModifiedEvent:
        if(seed1Set && !seed2Set && this->Seed1Actor == NULL)
          {
          this->AddRepresentation(seedNode);
          }
        else if(seed1Set && seed2Set && this->Seed2Actor == NULL)
          {
          this->AddRepresentation(seedNode);
          }
        this->UpdateVisibilityOnSlice(this->currentSeedNode);
        break;
      default:
        break;
      }
    }
  if (sliceNode)
    {
    switch(event)
      {
      case vtkCommand::ModifiedEvent:
        //std::cout << "DM - slice node event: ModifiedEvent " << std::endl;
        if(this->currentSeedNode != NULL)
          {
          // use currentSeedNode, because this is a modification of the slice node
          this->UpdatePositionOnSlice(this->currentSeedNode);
          this->UpdateVisibilityOnSlice(this->currentSeedNode);
          }
        break;
      default:
        break;
      }
    }
  else
    {
    this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
    }
}

//---------------------------------------------------------------------------
/// observe key press events
void vtkMRMLVesselSegmentationDisplayableManager2D::OnInteractorEvent(int eventid)
{
  vtkDebugMacro("OnInteractorEvent");

  // & don't bother doing anything unless have a seed node
  if(this->currentSeedNode != NULL)
    {
    // Check if in placing seed mode
    if(this->currentSeedNode->GetCurrentSeedMode() ==
        vtkMRMLVesselSegmentationSeedNode::PlaceSeedSeg
        || this->currentSeedNode->GetCurrentSeedMode() ==
            vtkMRMLVesselSegmentationSeedNode::PlaceSeedSplit)
      {
      double *pos = NULL;
      bool seed1Set = this->currentSeedNode->GetIsSeed1Set();
      bool seed2Set = this->currentSeedNode->GetIsSeed2Set();

      //std::cout << "DM - currentSeedNode, seed1set: " << seed1Set <<
          //" seed2set: " << seed2Set << std::endl;
      switch(eventid)
        {
        case vtkCommand::LeftButtonReleaseEvent:
          std::cout << "DM - LeftButtonReleaseEvent" << std::endl;
          pos = this->GetCrosshairPosition();
          if(pos != NULL)
            {
            if(!seed1Set && !seed2Set)
              {
              std::cout << "DM - Crosshair position, set seed1: " <<
                  pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
              // set the seed position (causes a modified event)
              this->currentSeedNode->SetSeed1(pos[0],pos[1],pos[2]);
              }
            else if(seed1Set && !seed2Set)
              {
              std::cout << "DM - Crosshair position, set seed2: " <<
                  pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
              // set the seed position (causes a modified event)
              this->currentSeedNode->SetSeed2(pos[0],pos[1],pos[2]);
              }
            else
              {
              return;
              }
            }
          break;
        default:
          std::cout << "DM - Default event " << std::endl;
          break;
        }
      }
    }

  this->Superclass::OnInteractorEvent(eventid);
}

//---------------------------------------------------------------------------
double* vtkMRMLVesselSegmentationDisplayableManager2D::GetCrosshairPosition()
{
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    }
  vtkMRMLNode* tempCrosshairNodeDefault =
      this->GetMRMLScene()->GetNodeByID("vtkMRMLCrosshairNodedefault");

  if (tempCrosshairNodeDefault == NULL)
    {
    vtkErrorMacro("No default crosshair node.");
    }

  vtkMRMLCrosshairNode* tempCrosshairNode =
      vtkMRMLCrosshairNode::SafeDownCast(tempCrosshairNodeDefault);

  if (tempCrosshairNode == NULL)
    {
    vtkErrorMacro("No crosshair node.");
    return NULL;
    }
  else
    {
    double *pos = new double[3];
    if(tempCrosshairNode->GetCursorPositionRAS(pos))
      {
      return pos;
      }
    }
  return NULL;
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
SetAndObserveSeedNode(vtkMRMLVesselSegmentationSeedNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No seed node passed.");
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);

  vtkUnObserveMRMLNodeMacro(node);
  vtkObserveMRMLNodeEventsMacro(node, nodeEvents.GetPointer());
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
SetAndObserveSliceNode(vtkMRMLSliceNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);

  vtkUnObserveMRMLNodeMacro(node);
  vtkObserveMRMLNodeEventsMacro(node, nodeEvents.GetPointer());
}

//------------------------------------------------------------------------------
bool vtkMRMLVesselSegmentationDisplayableManager2D::
AddRepresentation(vtkMRMLVesselSegmentationSeedNode *node)
{
  //std::cout << "DM - Add representation " << std::endl;
  vtkDebugMacro("AddRepresentation");

  if (!node)
    {
    vtkErrorMacro("No seed node passed");
    return false;
    }

  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node.");
    return false;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return false;
    }

  double *pos;
  bool seed1Set = this->currentSeedNode->GetIsSeed1Set();
  bool seed2Set = this->currentSeedNode->GetIsSeed2Set();
  std::cout << "DM - AddRepresentation, seed1set: " << seed1Set <<
      " seed2set: " << seed2Set << std::endl;

  if(seed1Set && !seed2Set)
    {
    pos = node->GetSeed1();
    std::cout << "AddRepresentation: Position (seed1) set to: " <<
        pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    }
  else if(seed1Set && seed2Set)
    {
    pos = node->GetSeed2();
    std::cout << "AddRepresentation: Position (seed2) set to: " <<
        pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    }
  else
    {
    return false;
    }

  vtkMRMLSliceNode* sliceNode = this->GetMRMLSliceNode();
  vtkNew<vtkMatrix4x4> rasToXY;
  vtkMatrix4x4::Invert(sliceNode->GetXYToRAS(), rasToXY.GetPointer());

  /*
  double scalingFactorPixelPerMm = sqrt(
    rasToXY->GetElement(0,0)*rasToXY->GetElement(0,0) +
    rasToXY->GetElement(0,1)*rasToXY->GetElement(0,1) +
    rasToXY->GetElement(0,2)*rasToXY->GetElement(0,2));

  std::cout << "AddRepresentation - scalingFactorPixelPerMm: "
      << scalingFactorPixelPerMm << std::endl;
          */

  // convert the ras pos to XYZ
  vtkSmartPointer<vtkMatrix4x4> RAStoXYmatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  RAStoXYmatrix->DeepCopy(sliceNode->GetXYToRAS());
  RAStoXYmatrix->Invert();
  double *XYZ = new double[4];
  double *pos2 = new double[4];
  pos2[0] = pos[0];
  pos2[1] = pos[1];
  pos2[2] = pos[2];
  pos2[3] = 1;
  RAStoXYmatrix->MultiplyPoint(pos2, XYZ);

  if(XYZ != NULL)
    {
    std::cout << "AddRepresentation - add representation for seed: "
        << node << std::endl;

    // Set up the seed point
    vtkSmartPointer<vtkRegularPolygonSource> polygonSource =
        vtkSmartPointer<vtkRegularPolygonSource>::New();
    polygonSource->SetNumberOfSides(5);
    polygonSource->SetRadius(3.0);//*scalingFactorPixelPerMm);
    polygonSource->SetCenter(XYZ[0], XYZ[1], XYZ[2]);

    std::cout << "AddRepresentation - position set to: " << XYZ[0] << " "
        << XYZ[1] << " " << XYZ[2] << std::endl;

    // Set the mapper, actor and add it to the scene
    vtkSmartPointer<vtkPolyDataMapper2D> mapper =
      vtkSmartPointer<vtkPolyDataMapper2D>::New();
    mapper->SetInputConnection(polygonSource->GetOutputPort());
    mapper->GlobalWarningDisplayOff();

    // Set the actor
    vtkSmartPointer<vtkActor2D> actor =
      vtkSmartPointer<vtkActor2D>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(1);
    actor->GetProperty()->SetColor(1,0,0); // colour

    if(seed1Set && !seed2Set)
      {
      //std::cout << "AddRepresentation set seed1" << std::endl;
      // Register seed point source
      this->Seed1Source = polygonSource;
      // Register the mapper
      this->Seed1Mapper = mapper;
      // Register the actor
      this->Seed1Actor = actor;
      }
    else if(seed1Set && seed2Set)
      {
      //std::cout << "AddRepresentation set seed2" << std::endl;
      // Register seed point source
      this->Seed2Source = polygonSource;
      // Register the mapper
      this->Seed2Mapper = mapper;
      // Register the actor
      this->Seed2Actor = actor;
      }

    this->GetRenderer()->AddActor2D(actor);

    this->RequestRender();

    return true;
    }

  return false;
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
UpdateVisibilityOnSlice(vtkMRMLVesselSegmentationSeedNode *node)
{
  vtkDebugMacro("UpdateVisibilityOnSlice");

  if (!node)
    {
    vtkErrorMacro("No seed node passed");
    return;
    }

  // Check for slice node
  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node.");
    return;
    }

  // look for an existing representation (for seed1 or seed2)
  if (this->Seed1Actor == NULL  && this->Seed2Actor == NULL)
    {
    vtkErrorMacro("Seed node is not currently "
                  << "handled by the displayable manager");
    return;
    }
  // else at least one is being handled by the manager

  // Compute the slice normal
  vtkMatrix4x4 *sliceToRASMatrix = this->GetMRMLSliceNode()->GetSliceToRAS();

  double slicePlaneNormal[3];
  slicePlaneNormal[0] = sliceToRASMatrix->GetElement(0,2);
  slicePlaneNormal[1] = sliceToRASMatrix->GetElement(1,2);
  slicePlaneNormal[2] = sliceToRASMatrix->GetElement(2,2);
  double slicePlaneOrigin[3];
  slicePlaneOrigin[0] = sliceToRASMatrix->GetElement(0,3);
  slicePlaneOrigin[1] = sliceToRASMatrix->GetElement(1,3);
  slicePlaneOrigin[2] = sliceToRASMatrix->GetElement(2,3);

  if(this->Seed1Actor != NULL)
    {
    double *pos1 = node->GetSeed1();
    int distanceToSeed = slicePlaneNormal[0]*(pos1[0]-slicePlaneOrigin[0]) +
        slicePlaneNormal[1]*(pos1[1]-slicePlaneOrigin[1]) +
        slicePlaneNormal[2]*(pos1[2]-slicePlaneOrigin[2]);

    //std::cout << "distance to seed1: " << distanceToSeed << std::endl;
    if(distanceToSeed == 0)
      {
      vtkActor2D *actor = this->Seed1Actor;
      actor->SetVisibility(true);
      }
    else
      {
      vtkActor2D *actor = this->Seed1Actor;
      actor->SetVisibility(false);
      }
    }
  if(this->Seed2Actor != NULL)
    {
    double *pos2 = node->GetSeed2();
    int distanceToSeed = slicePlaneNormal[0]*(pos2[0]-slicePlaneOrigin[0]) +
        slicePlaneNormal[1]*(pos2[1]-slicePlaneOrigin[1]) +
        slicePlaneNormal[2]*(pos2[2]-slicePlaneOrigin[2]);

    //std::cout << "distance to seed2: " << distanceToSeed << std::endl;
    if(distanceToSeed == 0)
      {
      vtkActor2D *actor = this->Seed2Actor;
      actor->SetVisibility(true);
      }
    else
      {
      vtkActor2D *actor = this->Seed2Actor;
      actor->SetVisibility(false);
      }
    }
  this->RequestRender();
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
UpdatePositionOnSlice(vtkMRMLVesselSegmentationSeedNode *node)
{
  vtkDebugMacro("UpdatePositionOnSlice");

  if (!node)
    {
    vtkErrorMacro("No seed node passed");
    return;
    }

  // Check for slice node
  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node.");
    return;
    }

  // look for an existing representation (for seed1 or seed2)
  if (this->Seed1Actor == NULL && this->Seed2Actor == NULL)
    {
    vtkErrorMacro("Seed node is not currently "
                  << "handled by the displayable manager");
    return;
    }
  // else at least one is being handled by the manager

  vtkSmartPointer<vtkMRMLSliceNode> sliceNode = this->GetMRMLSliceNode();
  vtkSmartPointer<vtkMatrix4x4> RAStoXYmatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  RAStoXYmatrix.GetPointer()->DeepCopy(sliceNode.GetPointer()->GetXYToRAS());
  RAStoXYmatrix.GetPointer()->Invert();

  /*
  double scalingFactorPixelPerMm = sqrt(
      RAStoXYmatrix.GetPointer()->GetElement(0,0)*RAStoXYmatrix.GetPointer()->GetElement(0,0) +
      RAStoXYmatrix.GetPointer()->GetElement(0,1)*RAStoXYmatrix.GetPointer()->GetElement(0,1) +
      RAStoXYmatrix.GetPointer()->GetElement(0,2)*RAStoXYmatrix.GetPointer()->GetElement(0,2));
  */

  bool seed1Set = this->currentSeedNode->GetIsSeed1Set();
  bool seed2Set = this->currentSeedNode->GetIsSeed2Set();
 // std::cout << "DM - UpdatePositionOnSlice, seed1set: " << seed1Set <<
     // " seed2set: " << seed2Set << std::endl;

  if(this->Seed1Source && seed1Set)
    {
    double *ras1 = this->currentSeedNode->GetSeed1();
    //std::cout << "UpdatePositionOnSlice - RAS seed1: "
        //<< ras1[0] << " " << ras1[1] << " " << ras1[2] << std::endl;
    // convert the ras pos to XYZ
    double *XYZ = new double[4];
    double pos[4];
    pos[0] = ras1[0];
    pos[1] = ras1[1];
    pos[2] = ras1[2];
    pos[3] = 1;
    RAStoXYmatrix.GetPointer()->MultiplyPoint(pos, XYZ);
    // Change the seed point
    this->Seed1Source->SetRadius(3.0);//*scalingFactorPixelPerMm);
    this->Seed1Source->SetCenter(XYZ[0], XYZ[1], XYZ[2]);
    //std::cout << "UpdatePositionOnSlice - XYZ seed1 set to: "
        //<< XYZ[0] << " " << XYZ[1] << " " << XYZ[2] << std::endl;
    this->Seed1Source->Update();
    this->Seed1Mapper->Update();
    }
  if(this->Seed2Source && seed2Set)
    {
    double *ras2 = this->currentSeedNode->GetSeed2();
    //std::cout << "UpdatePositionOnSlice - RAS seed2: "
        //<< ras2[0] << " " << ras2[1] << " " << ras2[2] << std::endl;
    // convert the ras pos to XYZ
    double *XYZ = new double[4];
    double pos[4];
    pos[0] = ras2[0];
    pos[1] = ras2[1];
    pos[2] = ras2[2];
    pos[3] = 1;
    RAStoXYmatrix.GetPointer()->MultiplyPoint(pos, XYZ);
    // Change the seed point
    this->Seed2Source->SetRadius(3.0);//*scalingFactorPixelPerMm);
    this->Seed2Source->SetCenter(XYZ[0], XYZ[1], XYZ[2]);
    //std::cout << "UpdatePositionOnSlice - XYZ seed2 set to: "
        //<< XYZ[0] << " " << XYZ[1]<< " " << XYZ[2] << std::endl;
    this->Seed2Source->Update();
    this->Seed2Mapper->Update();
    }
  this->RequestRender();
}

