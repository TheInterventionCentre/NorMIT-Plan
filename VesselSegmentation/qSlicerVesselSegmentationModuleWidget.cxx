/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationModuleWidget.cxx

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
/// \ingroup Slicer_VesselSegmentation
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
  this->setMRMLScene(0);
}

//-----------------------------------------------------------------------------
void qSlicerVesselSegmentationModuleWidget::setup()
{
  Q_D(qSlicerVesselSegmentationModuleWidget);
  d->setupUi(this);
    
  // connect events to node selection dropdown
  //QObject::connect(d->ActiveVolumeNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(nodeSelectionChanged(vtkMRMLNode*)));
  //QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ActiveVolumeNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));

  // causes generic module tests to fail
  //vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
  //selectionNode->SetReferenceActivePlaceNodeClassName("vtkMRMLMarkupsFiducialNode");

  // connections to preprocessing widget
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(PreprocessingClicked()),
                   this,
                   SLOT(onPreprocessing()));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(LTSpinChanged(int)),
                   this,
                   SLOT(onSetLowerThreshold(int)));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(UTSpinChanged(int)),
                   this,
                   SLOT(onSetUpperThreshold(int)));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(AlphaSpinChanged(int)),
                   this,
                   SLOT(onSetAlpha(int)));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(BetaSpinChanged(int)),
                   this,
                   SLOT(onSetBeta(int)));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(ConductanceSpinChanged(int)),
                   this,
                   SLOT(onSetConductance(int)));
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(IterationsSpinChanged(int)),
                   this,
                   SLOT(onSetIterations(int)));

  // connections to segmentation widget
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(PlaceSeedsSegClicked()),
                   this,
                   SLOT(onPlaceSeedSeg()));
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(RunSegmentClicked()),
                   this,
                   SLOT(onRunSegment()));
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(HepaticSegSelected()),
                   this,
                   SLOT(onHepaticSeg()));
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(PortalSegSelected()),
                   this,
                   SLOT(onPortalSeg()));

  // connections to splitting widget
  QObject::connect(d->SplittingWidget,
                   SIGNAL(MergeLabelMapsClicked()),
                   this,
                   SLOT(onMergeLabelMaps()));
  QObject::connect(d->SplittingWidget,
                   SIGNAL(PlaceSeedsMergeClicked()),
                   this,
                   SLOT(onPlaceSeedsMerge()));
  QObject::connect(d->SplittingWidget,
                   SIGNAL(RunSeedAssignmentClicked()),
                   this,
                   SLOT(onRunSeedAssignment()));
  QObject::connect(d->SplittingWidget,
                   SIGNAL(OnHepaticMergeSelected()),
                   this,
                   SLOT(onPortalMerge()));
  QObject::connect(d->SplittingWidget,
                   SIGNAL(OnPortalMergeSelected()),
                   this,
                   SLOT(onPortalMerge()));

  this->Superclass::setup();
}

void qSlicerVesselSegmentationModuleWidget::enter()
{
  Q_D(qSlicerVesselSegmentationModuleWidget);

  this->Superclass::enter();
}

void qSlicerVesselSegmentationModuleWidget::exit()
{
  this->Superclass::exit();
  this->qvtkDisconnectAll();
}

//-----------------------------------------------------------------------------
/*
void qSlicerVesselSegmentationModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerVesselSegmentationModuleWidget);

  this->Superclass::setMRMLScene(scene);

  std::cout << "Widget - Set MRML scene called " << std::endl;
}*/

//-----------------------------------------------------------------------------
vtkSlicerVesselSegmentationLogic *qSlicerVesselSegmentationModuleWidget::
vesselSegmentationLogic()
{
  if (this->logic() == NULL)
    {
    return NULL;
    }
  return vtkSlicerVesselSegmentationLogic::SafeDownCast(this->logic());
}

//------------------------------------------------------------------------------
/*
 * Called when selector changes which image is selected
 */
void qSlicerVesselSegmentationModuleWidget::nodeSelectionChanged(vtkMRMLNode* node)
{

  Q_D(qSlicerVesselSegmentationModuleWidget);

  std::cout << "Widget - Node selection changed " << std::endl;

  if (!vtkMRMLScalarVolumeNode::SafeDownCast(node))
  {
    qWarning() << Q_FUNC_INFO << "nodeSelectionChanged: passed in node is not a volume";
    return;
  }

  vtkMRMLScalarVolumeNode *activeVol = vtkMRMLScalarVolumeNode::SafeDownCast( node );
  vtkMRMLVolumeNode *volNode = vtkMRMLVolumeNode::SafeDownCast( node );

  // FIX for if the load image before loading the module
  if(volNode == NULL)
  {
    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    char *activeVolID = selectionNode->GetActiveVolumeID();
    activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID(activeVolID));
    volNode = vtkMRMLVolumeNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID(activeVolID));
  }

  this->vesselSegmentationLogic()->SetActiveVolumeNode(volNode);
  this->vesselSegmentationLogic()->SetActiveVolume(activeVol);
}

/*
 * Functions associated with preprocessing widget
 */
void qSlicerVesselSegmentationModuleWidget::onPreprocessing()
{
  vtkMRMLScalarVolumeNode *activeVol = this->vesselSegmentationLogic()->GetActiveVolume();

  if(activeVol == NULL)
  {
    // get the active volume
    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    char *activeVolID = selectionNode->GetActiveVolumeID();
    activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID(activeVolID));
    this->vesselSegmentationLogic()->SetActiveVolume(activeVol);
  }

  // if still null...
  if(activeVol == NULL)
  {
    std::cout << "No Active Volume..." << std::endl;
  }
  else {
    std::cout << "Trying to call preprocessing" << std::endl;
    this->vesselSegmentationLogic()->CallPreprocessing();
  }
}
void qSlicerVesselSegmentationModuleWidget::onSetLowerThreshold(int value)
{
  this->vesselSegmentationLogic()->SetLowerThreshold(value);
}
void qSlicerVesselSegmentationModuleWidget::onSetUpperThreshold(int value)
{
  this->vesselSegmentationLogic()->SetUpperThreshold(value);
}
void qSlicerVesselSegmentationModuleWidget::onSetAlpha(int value)
{
  this->vesselSegmentationLogic()->SetAlpha(value);
}
void qSlicerVesselSegmentationModuleWidget::onSetBeta(int value)
{
  this->vesselSegmentationLogic()->SetBeta(value);
}
void qSlicerVesselSegmentationModuleWidget::onSetConductance(int value)
{
  this->vesselSegmentationLogic()->SetConductance(value);
}
void qSlicerVesselSegmentationModuleWidget::onSetIterations(int value)
{
  this->vesselSegmentationLogic()->SetIterations(value);
}


/*
 * Functions associated with segmentation widget
 */
 void qSlicerVesselSegmentationModuleWidget::onPlaceSeedSeg()
 {
   vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));

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

 void qSlicerVesselSegmentationModuleWidget::onRunSegment()
 {
   std::cout << "Widget - run Segment" << std::endl;

   vtkMRMLScalarVolumeNode *activeVol = this->vesselSegmentationLogic()->GetActiveVolume();

   if(activeVol == NULL)
   {
     // get the active volume
     vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
     char *activeVolID = selectionNode->GetActiveVolumeID();
     activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID(activeVolID));
     this->vesselSegmentationLogic()->SetActiveVolume(activeVol);
   }

   if(activeVol == NULL)
   {
     std::cout << "No Active Volume..." << std::endl;
   }
   else {
     std::cout << "Trying to call centreline" << std::endl;
     this->vesselSegmentationLogic()->CallSegmentationAlgorithm();
   }
 }

 void qSlicerVesselSegmentationModuleWidget::onHepaticSeg()
 {
   this->vesselSegmentationLogic()->IsHepaticSeg(true);
 }

 void qSlicerVesselSegmentationModuleWidget::onPortalSeg()
 {
   this->vesselSegmentationLogic()->IsHepaticSeg(false);
 }


 /*
  * Functions associated with splitting widget
  */
 void qSlicerVesselSegmentationModuleWidget::onMergeLabelMaps()
 {
   this->vesselSegmentationLogic()->CallMergeLabelMaps();
 }

 void qSlicerVesselSegmentationModuleWidget::onPlaceSeedsMerge()
 {
   vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));

   std::cout << "Widget - Place seeds (Merge): " << interactionNode->GetCurrentInteractionMode() << std::endl;

   interactionNode->SetPlaceModePersistence(0);
   interactionNode->SetCurrentInteractionMode(vtkMRMLInteractionNode::Place);

   vtkMRMLVesselSegmentationDisplayableManager::SetFiducialsMode(false);
 }

 void qSlicerVesselSegmentationModuleWidget::onRunSeedAssignment()
 {
   this->vesselSegmentationLogic()->CallAssignSeeds();
 }

 void qSlicerVesselSegmentationModuleWidget::onHepaticMerge()
 {
   this->vesselSegmentationLogic()->IsHepaticMerge(true);
 }

 void qSlicerVesselSegmentationModuleWidget::onPortalMerge()
 {
   this->vesselSegmentationLogic()->IsHepaticMerge(false);
 }

