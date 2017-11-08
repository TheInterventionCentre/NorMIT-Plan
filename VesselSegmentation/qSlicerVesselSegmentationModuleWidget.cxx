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

#include "vtkMRMLNode.h"

// VTK includes
#include <vtkNew.h>

// Qt includes
#include <qSlicerCoreIOManager.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerApplication.h>
#include <qSlicerIO.h>
#include <vtkMRMLVesselSegmentationDisplayableManager2D.h>

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

  // connections to preprocessing widget
  QObject::connect(d->PreprocessingWidget,
                   SIGNAL(PreprocessingClicked(int,int,unsigned int,int,unsigned int,unsigned int)),
                   this,
                   SLOT(onPreprocessing(int,int,unsigned int,int,unsigned int,unsigned int)));

  // connections to segmentation widget
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(PlaceSeedsSegClicked()),
                   this,
                   SLOT(onPlaceSeedSeg()));
  QObject::connect(d->SegmentationWidget,
                   SIGNAL(RunSegmentClicked(bool)),
                   this,
                   SLOT(onRunSegment(bool)));

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
                   SIGNAL(RunSeedAssignmentClicked(bool)),
                   this,
                   SLOT(onRunSeedAssignment(bool)));

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
void qSlicerVesselSegmentationModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerVesselSegmentationModuleWidget);

  this->Superclass::setMRMLScene(scene);

  std::cout << "Widget - Set MRML scene called " << std::endl;
}

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

  // TODO: update things based on the active volume?
  this->vesselSegmentationLogic()->SetAndPropagateActiveVolume(activeVol);
  activeVol = this->vesselSegmentationLogic()->GetActiveVolume();
}

//------------------------------------------------------------------------------
/*
 * Functions associated with preprocessing widget
 */
void qSlicerVesselSegmentationModuleWidget::onPreprocessing(int lowerThreshold, int upperThreshold, unsigned int alpha, int beta, unsigned int conductance, unsigned int iterations)
{
  this->vesselSegmentationLogic()->PreprocessImage( lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
}

//------------------------------------------------------------------------------
/*
 * Functions associated with segmentation widget
 */
 void qSlicerVesselSegmentationModuleWidget::onPlaceSeedSeg()
 {
   //vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast
       //(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
   if (!this->vesselSegmentationLogic())
     {
     std::cerr << "Error: No module logic." << std::endl;
     return;
     }

   // change if in mode for placing seeds or not
   // right now we just add a seed...
   this->vesselSegmentationLogic()->AddSeedNode();

 }

 //------------------------------------------------------------------------------
 void qSlicerVesselSegmentationModuleWidget::onRunSegment(bool isHepatic)
 {
   this->vesselSegmentationLogic()->SegmentVesselsFromWidget(isHepatic);
 }

 //------------------------------------------------------------------------------
 /*
  * Functions associated with splitting widget
  */
 void qSlicerVesselSegmentationModuleWidget::onMergeLabelMaps()
 {
   this->vesselSegmentationLogic()->MergeLabelMaps();
 }

 //------------------------------------------------------------------------------
 void qSlicerVesselSegmentationModuleWidget::onPlaceSeedsMerge()
 {
   //vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast
       //(this->vesselSegmentationLogic()->GetMRMLScene()->GetNodeByID("vtkMRMLInteractionNodeSingleton"));
   if (!this->vesselSegmentationLogic())
     {
     std::cerr << "Error: No module logic." << std::endl;
     return;
     }

   // change if in mode for placing seeds or not
   // right now we just add a seed...
   this->vesselSegmentationLogic()->AddSeedNode();
 }

 //------------------------------------------------------------------------------
 void qSlicerVesselSegmentationModuleWidget::onRunSeedAssignment(bool isHepatic)
 {
   this->vesselSegmentationLogic()->SplitVesselsFromWidget(isHepatic);
 }
