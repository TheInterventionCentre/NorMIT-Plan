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

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLAbstractLogic.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLCrosshairNode.h>
#include <vtkMRMLInteractionNode.h>

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
#include <vtkMRMLVesselSegmentationDisplayableManager2D.h>
#include <vtkPointSource.h>

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qMRMLSliceWidget.h>
#include <qMRMLSliceView.h>

#include <iostream>
#include <cstdlib>
#include <vector>


vtkStandardNewMacro(vtkMRMLVesselSegmentationDisplayableManager2D);

//-------------------------------------------------------------------------------
vtkMRMLVesselSegmentationDisplayableManager2D::
vtkMRMLVesselSegmentationDisplayableManager2D()
{
  observingSliceNode = false;
  observingSliceView = false;

  this->RemoveInteractorStyleObservableEvent(vtkCommand::LeftButtonReleaseEvent);
  this->RemoveInteractorStyleObservableEvent(vtkCommand::LeftButtonPressEvent);
    
  this->AddInteractorStyleObservableEvent(vtkCommand::LeftButtonReleaseEvent);
  this->AddInteractorStyleObservableEvent(vtkCommand::LeftButtonPressEvent);
}

//------------------------------------------------------------------------------
vtkMRMLVesselSegmentationDisplayableManager2D::
~vtkMRMLVesselSegmentationDisplayableManager2D()
{

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
  this->Superclass::SetMRMLSceneInternal(newScene);
  this->OnMRMLSceneEndClose();

  if(newScene)
  {

  }
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::OnMRMLSceneEndClose()
{
  vtkDebugMacro("OnMRMLSceneEndClose");



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
  std::cout << "DM - added node " << addedNode << std::endl;

  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene has been set");
    return;
    }

  if (!this->GetRenderer())
    {
    vtkErrorMacro("No renderer.");
    return;
    }

  if (!this->GetMRMLSliceNode())
    {
    vtkErrorMacro("No slice node present");
    return;
    }
  else if (this->observingSliceNode == false)
    {
    vtkMRMLSliceNode *sliceNode = this->GetMRMLSliceNode();
    std::cout << "DM - oberving slice node " << std::endl;
    this->SetAndObserveSliceNode(sliceNode);
    this->observingSliceNode = true;
    }

  /*
  if(this->observingSliceView == false)
    {
    // Set up interactor observations
    qSlicerLayoutManager* layoutManager = qSlicerApplication::application()->layoutManager();
    if (!layoutManager)
      {
      vtkErrorMacro("No layoutManager present");
      return;
      }

    // Slice views
    foreach (QString sliceViewName, layoutManager->sliceViewNames())
      {  
      // Create command for slice view
      qMRMLSliceWidget* sliceWidget = layoutManager->sliceWidget(sliceViewName);
      if (!sliceWidget)
        {
        vtkErrorMacro("No slice widget present");
        return;
        }
      qMRMLSliceView* sliceView = sliceWidget->sliceView();
      if (!sliceView)
        {
        vtkErrorMacro("No slice view present");
        return;
        }
          
      vtkRenderWindowInteractor* interactor = sliceView->interactorStyle()->GetInteractor();
      if (!interactor)
        {
        vtkErrorMacro("No interactor present");
        return;
        }
      this->MouseClickCommand->SetCallback(this->OnMouseClick);
      this->MouseClickCommand->SetClientData(this);
      interactor->AddObserver(vtkCommand::LeftButtonReleaseEvent, this->MouseClickCommand.GetPointer(), 1.0);   
      }
      this->observingSliceView = true;
    }
  */
  // TODO: observe all seed nodes added to scene
  // Check this is a seed node.
  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast(addedNode);

  if(seedNode)
    {
    // Check whether the node has an associated representation
    SeedActorIt it = SeedActorMap.find(seedNode);
    if (it != SeedActorMap.end())
      {
      return;
      }

    if (!this->AddSeed(seedNode))
      {
      return;
      }

    this->SetAndObserveSeedNode(seedNode);
    currentSeedNode = seedNode;
    std::cout << "DM - Set current seed node: " << currentSeedNode.GetPointer() << std::endl;
    this->RequestRender();
    }
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
OnMRMLNodeModified(vtkMRMLNode *vtkNotUsed(modifiedNode))
{
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
  if (seedNode)
    {
    // Remove the observations from the node
    vtkUnObserveMRMLNodeMacro(seedNode);
    }

  vtkMRMLSliceNode *sliceNode =
    vtkMRMLSliceNode::SafeDownCast(node);
  if (sliceNode)
    {
    vtkUnObserveMRMLNodeMacro(sliceNode);
    }
}

//-------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
ProcessMRMLNodesEvents(vtkObject *caller,
                       unsigned long event,
                       void *callData)
{
  std::cout << "DM - processMRMLNodesEvents " << std::endl;
  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast(caller);

  if (seedNode)
    {
    switch(event)
      {
      case vtkMRMLDisplayableNode::DisplayModifiedEvent:
        this->UpdateVisibility(seedNode);
        break;

      default:
        break;
      }
    }

  vtkMRMLSliceNode *sliceNode =
    vtkMRMLSliceNode::SafeDownCast(caller);
  if (sliceNode == this->GetMRMLSliceNode())
    {
      std::cout << "DM - slice node calling event " << std::endl;
      if(currentSeedNode.GetPointer() != NULL)
      {
        std::cout << "DM - and have seed " << std::endl;
        if(currentSeedNode.GetPointer()->GetCurrentSeedState() == 0 )
        {
          std::cout << "DM - seed state is 0" << std::endl;
          switch(event)
          {                 
            case vtkCommand::ModifiedEvent:
            std::cout << "DM - slice node modified " << std::endl;
            break;
                  
            default:
            std::cout << "DM - slice node other event? " << std::endl;
            break;
          }      
        }
      }
    }

  this->Superclass::ProcessMRMLNodesEvents(caller, event, callData);
  this->RequestRender();
}

//---------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::OnInteractorStyleEvent(int eventid)
{
  std::cout << "DM - OnInteractorStyleEvent " << std::endl;

  switch(eventid)
    {
    case vtkCommand::LeftButtonReleaseEvent:
      std::cout << "DM - LeftButtonReleaseEvent " << std::endl;
      break;
    case vtkCommand::LeftButtonPressEvent:
      std::cout << "DM - LeftButtonPressEvent " << std::endl;
      break;
    default:
      break;
    }
  this->Superclass::OnInteractorStyleEvent(eventid);
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
SetAndObserveSeedNode(vtkMRMLVesselSegmentationSeedNode *node)
{
  if (!node)
    {
    vtkErrorMacro("No node passed.");
    return;
    }

  vtkNew<vtkIntArray> nodeEvents;
  nodeEvents->InsertNextValue(vtkCommand::ModifiedEvent);
  nodeEvents->InsertNextValue(vtkMRMLDisplayableNode::DisplayModifiedEvent);

  vtkUnObserveMRMLNodeMacro(node);
  vtkObserveMRMLNodeEventsMacro(node, nodeEvents.GetPointer());
}

//------------------------------------------------------------------------------
bool vtkMRMLVesselSegmentationDisplayableManager2D::
AddSeed(vtkMRMLVesselSegmentationSeedNode *node)
{
  vtkDebugMacro("AddSeed");

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

  double *seed1 = node->GetSeed1();
  double *seed2 = node->GetSeed2();

  // Set up the point surface
  vtkSmartPointer<vtkPointSource> pointSource =
    vtkSmartPointer<vtkPointSource>::New();
  pointSource->SetCenter(seed1[0], seed1[1], seed1[2]);
  pointSource->SetNumberOfPoints(1);
  pointSource->SetRadius(5.0);

  // Register polygon source
  this->SeedMap[node] = pointSource;

  // Set the mapper, actor and add it to the scene
  vtkSmartPointer<vtkPolyDataMapper2D> mapper =
    vtkSmartPointer<vtkPolyDataMapper2D>::New();
  mapper->SetInputConnection(pointSource->GetOutputPort());
  mapper->GlobalWarningDisplayOff();

  // Register the mapper
  this->SeedMapperMap[node] = mapper;

  // Set the actor
  vtkSmartPointer<vtkActor2D> actor =
    vtkSmartPointer<vtkActor2D>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetLineWidth(3);

  // Register the actor
  this->SeedActorMap[node] = actor;

  this->GetRenderer()->AddActor2D(actor);

  return true;
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
UpdateVisibility(vtkMRMLVesselSegmentationSeedNode *node)
{
  std::cout << "DM - seed node update visibility" << std::endl;
}

//------------------------------------------------------------------------------
void vtkMRMLVesselSegmentationDisplayableManager2D::
SetAndObserveSliceNode(vtkMRMLSliceNode *node)
{
  std::cout << "DM - set and observe slice node " << std::endl;
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
void vtkMRMLVesselSegmentationDisplayableManager2D::OnMouseClick
(vtkObject *caller, unsigned long int vtkNotUsed(id), void *clientData, void *callerData)
{
  vtkMRMLVesselSegmentationDisplayableManager2D* DM = 
      reinterpret_cast<vtkMRMLVesselSegmentationDisplayableManager2D*>(clientData);
    
  std::cout << "DM - mouse click callback " << std::endl;
    
  vtkMRMLNode* tempCrosshairNodeDefault = DM->GetMRMLScene()->GetNodeByID("vtkMRMLCrosshairNodedefault");
  vtkMRMLCrosshairNode* tempCrosshairNode = vtkMRMLCrosshairNode::SafeDownCast(tempCrosshairNodeDefault);
    
  if(tempCrosshairNode != NULL)
    {
    double *pos = new double[3];
    if(tempCrosshairNode->GetCursorPositionRAS(pos))
    {
      std::cout << "DM - crosshairposition: " << pos[0] << " " << pos[1] << " " <<
          pos[2] << std::endl;
    } 
  }
}
