/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>
#include <QWidget>

#include <iostream>

#include <vtkEventQtSlotConnect.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLScalarVolumeDisplayNode.h>

// SlicerQt includes
#include "qSlicerVesselSegmentationModuleWidget.h"
#include "ui_qSlicerVesselSegmentationModuleWidget.h"

#include "qSlicerVesselSegmentationModule.h"
#include "vtkSlicerVesselSegmentationLogic.h"

#include "vtkMRMLVesselSegmentationDisplayableManager.h"
#include "vtkSlicerVesselSegmentationLogic.h"

// MRML includes
#include "vtkMRMLMarkupsNode.h"
#include "vtkMRMLMarkupsFiducialNode.h"
#include "vtkMRMLAnnotationRulerNode.h"
#include "vtkMRMLNode.h"

// VTK includes
#include <vtkNew.h>

// Qt includes
#include <qSlicerCoreIOManager.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerIO.h>

# include "qSlicerCorePythonManager.h"

// module includes
#include "vtkSlicerVesselSegmentationLogic.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVesselSegmentationModuleWidgetPrivate: public Ui_qSlicerVesselSegmentationModuleWidget
{
public:
  qSlicerVesselSegmentationModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModuleWidgetPrivate::qSlicerVesselSegmentationModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModuleWidget::qSlicerVesselSegmentationModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerVesselSegmentationModuleWidgetPrivate )
{
  this->connections = vtkEventQtSlotConnect::New();
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModuleWidget::~qSlicerVesselSegmentationModuleWidget()
{
  if(this->connections)
  {
    this->connections->Delete();
  }
}

void qSlicerVesselSegmentationModuleWidget::enter()
{
  Q_D(qSlicerVesselSegmentationModuleWidget);

  this->Superclass::enter();
}

//-----------------------------------------------------------------------------
void qSlicerVesselSegmentationModuleWidget::setup()
{
  Q_D(qSlicerVesselSegmentationModuleWidget);
  d->setupUi(this);
    
  // connect events to node selection dropdown
  QObject::connect(d->ActiveVolumeNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(nodeSelectionChanged(vtkMRMLNode*)));
  QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ActiveVolumeNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));

  this->Module = dynamic_cast<qSlicerVesselSegmentationModule*>(this->module());
  this->ModuleLogic = vtkSlicerVesselSegmentationLogic::SafeDownCast(this->Module->logic());

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
  selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
/*
 * Called when selector changes which image is selected
 */
void qSlicerVesselSegmentationModuleWidget::nodeSelectionChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerVesselSegmentationModuleWidget);

  std::cout << "Widget - Node selection changed " << std::endl;
  vtkMRMLScalarVolumeNode *activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(d->ActiveVolumeNodeSelector->currentNode() );
  vtkMRMLVolumeNode *volNode = vtkMRMLVolumeNode::SafeDownCast(d->ActiveVolumeNodeSelector->currentNode() );

  // FIX for if the load image before loading the module
  if(volNode == NULL)
  {
    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    char *activeVolID = selectionNode->GetActiveVolumeID();
    activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID(activeVolID));
    volNode = vtkMRMLVolumeNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID(activeVolID));
  }

  this->ModuleLogic->SetActiveVolumeNode(volNode);

  this->ModuleLogic->SetActiveVolume(activeVol);
}

/*
 * Called when preprocessing button clicked
 */
void qSlicerVesselSegmentationModuleWidget::PreProcessing()
{
  std::cout << "Widget - On PreProcessing" << std::endl;

  vtkMRMLScalarVolumeNode *activeVol = this->ModuleLogic->GetActiveVolume();

  if(activeVol == NULL)
  {
    // get the active volume
    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    char *activeVolID = selectionNode->GetActiveVolumeID();
    activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID(activeVolID));
    this->ModuleLogic->SetActiveVolume(activeVol);
  }

  // if still null...
  if(activeVol == NULL)
  {
    std::cout << "No Active Volume..." << std::endl;
  }
  else {
    std::cout << "Trying to call preprocessing" << std::endl;
    this->ModuleLogic->CallPreprocessing();
  }

}

//-----------------------------------------------------------------------------

/**
 * Functions for vessel segmentation
 * void PlaceSeedsSeg();
 * void RunSegment();
 * void OnHepaticSeg();
 * void OnPortalSeg();
 */

void qSlicerVesselSegmentationModuleWidget::PlaceSeedsSeg()
{
  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
  
  std::cout << "Widget - Place seeds (Segment): " << interactionNode->GetCurrentInteractionMode() << std::endl;

  if(interactionNode->GetCurrentInteractionMode() == vtkMRMLInteractionNode::Place)
  {
    interactionNode->SetPlaceModePersistence(0);
    interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::ViewTransform);

    vtkMRMLVesselSegmentationDisplayableManager::SetFiducialsMode(false);
  }
  else {
    interactionNode->SetPlaceModePersistence(1);
    interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);

    vtkMRMLVesselSegmentationDisplayableManager::SetFiducialsMode(true);
  }
}

void qSlicerVesselSegmentationModuleWidget::RunSegment()
{
  std::cout << "Widget - run Segment" << std::endl;

  vtkMRMLScalarVolumeNode *activeVol = this->ModuleLogic->GetActiveVolume();
  
  if(activeVol == NULL)
  {
    // get the active volume
    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    char *activeVolID = selectionNode->GetActiveVolumeID();
    activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID(activeVolID));
    this->ModuleLogic->SetActiveVolume(activeVol);
  }

  if(activeVol == NULL)
  {
    std::cout << "No Active Volume..." << std::endl;
  }
  else {
    std::cout << "Trying to call centreline" << std::endl;
    this->ModuleLogic->CallSegmentationAlgorithm();
  }
  
}
void qSlicerVesselSegmentationModuleWidget::OnHepaticSeg()
{
    std::cout << "Widget - On Hepatic (Segment)" << std::endl;
    this->ModuleLogic->IsHepaticSeg(true);
}

void qSlicerVesselSegmentationModuleWidget::OnPortalSeg()
{
   std::cout << "Widget - On Portal (segment)" << std::endl;
   this->ModuleLogic->IsHepaticSeg(false);
}

//------------------------------------------------------------------------------
/*
 * Functions for hepatic / portal splitting
 * void MergeLabelMaps();
 * void PlaceSeedsMerge();
 * void RunSeedAssignment();
 * void OnHepaticMerge();
 * void OnPortalMerge();
 */
void qSlicerVesselSegmentationModuleWidget::MergeLabelMaps()
{
  std::cout << "Widget - Merge label maps" << std::endl;
  this->ModuleLogic->CallMergeLabelMaps();

}

void qSlicerVesselSegmentationModuleWidget::PlaceSeedsMerge()
{
  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(this->ModuleLogic->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));

  std::cout << "Widget - Place seeds (Merge): " << interactionNode->GetCurrentInteractionMode() << std::endl;

  interactionNode->SetPlaceModePersistence(0);
  interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);

  vtkMRMLVesselSegmentationDisplayableManager::SetFiducialsMode(false);
}

void qSlicerVesselSegmentationModuleWidget::RunSeedAssignment()
{
  std::cout << "Widget - Run seed assignment" << std::endl;
  this->ModuleLogic->CallAssignSeeds();
}

void qSlicerVesselSegmentationModuleWidget::OnHepaticMerge()
{
    std::cout << "Widget - On Hepatic (Merge)" << std::endl;
    this->ModuleLogic->IsHepaticMerge(true);
}

void qSlicerVesselSegmentationModuleWidget::OnPortalMerge()
{
   std::cout << "Widget - On Portal (Merge)" << std::endl;
   this->ModuleLogic->IsHepaticMerge(false);
}

void qSlicerVesselSegmentationModuleWidget::OnLTSpin(int value)
{
  //std::cout << "Widget - On LT spin " << value << std::endl;
  this->ModuleLogic->SetLowerThreshold(value);
}

void qSlicerVesselSegmentationModuleWidget::OnUTSpin(int value)
{
  //std::cout << "Widget - On UT spin " << value << std::endl;
  this->ModuleLogic->SetUpperThreshold(value);
}

void qSlicerVesselSegmentationModuleWidget::OnAlphaSpin(int value)
{
  //std::cout << "Widget - On Alpha spin " << value << std::endl;
  this->ModuleLogic->SetAlpha(value);
}

void qSlicerVesselSegmentationModuleWidget::OnBetaSpin(int value)
{
  //std::cout << "Widget - On Beta spin " << value << std::endl;
  this->ModuleLogic->SetBeta(value);
}

void qSlicerVesselSegmentationModuleWidget::OnConductanceSpin(int value)
{
  //std::cout << "Widget - On Conductance spin " << value << std::endl;
  this->ModuleLogic->SetConductance(value);
}

void qSlicerVesselSegmentationModuleWidget::OnInterationsSpin(int value)
{
  //std::cout << "Widget - On Interations spin " << value << std::endl;
  this->ModuleLogic->SetIterations(value);
}



