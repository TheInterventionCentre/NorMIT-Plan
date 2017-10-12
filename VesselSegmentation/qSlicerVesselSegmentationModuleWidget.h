/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationModuleWidget.h

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

#ifndef __qSlicerVesselSegmentationModuleWidget_h
#define __qSlicerVesselSegmentationModuleWidget_h

// This module includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerVesselSegmentationModuleExport.h"

class qSlicerVesselSegmentationModuleWidgetPrivate;
class qSlicerVesselSegmentationModule;
class vtkSlicerVesselSegmentationLogic;
class vtkEventQtSlotConnect;
class vtkMRMLNode;

/**
 *  \ingroup VesselSegmentation
 *
 * \brief Widget class for the module, contains the interface for the module.
 *
 * This class sends and receives information to/from the subwidgets (such as qSlicerVesselSegmentationPreprocessingWidget)
 * and also sends and receives information to/from the module logic (vtkSlicerResectionPlanningLogic)
 *
 */
class Q_SLICER_QTMODULES_VESSELSEGMENTATION_EXPORT qSlicerVesselSegmentationModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVesselSegmentationModuleWidget(QWidget *parent=0);
  virtual ~qSlicerVesselSegmentationModuleWidget();

  virtual void enter();
  virtual void exit();

public slots:

  /**
   * Called when the preprocessing button is clicked
   * connected to signal from preprocessing widget: preprocessingClicked()
   * This function sends the parameter information up to the Logic
   */
   void onPreprocessing();

  /**
   * Called when the lower threshold value is changed
   * connected to signal from preprocessing widget: LTSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetLowerThreshold(int value);

  /**
   * Called when the upper threshold value is changed
   * connected to signal from preprocessing widget: UTSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetUpperThreshold(int value);

  /**
   * Called when the alpha value is changed
   * connected to signal from preprocessing widget: AlphaSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetAlpha(int value);

  /**
   * Called when the beta value is changed
   * connected to signal from preprocessing widget: BetaSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetBeta(int value);

  /**
   * Called when the conductance value is changed
   * connected to signal from preprocessing widget: ConductanceSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetConductance(int value);

  /**
   * Called when the iterations value is changed
   * connected to signal from preprocessing widget: IterationsSpinChanged()
   * This function sends the parameter information up to the Logic
   *
   * @param value of spin
   */
  void onSetIterations(int value);


 /**
  * Called when the place seeds button is clicked
  * connected to signal from segmentation widget: PlaceSeedsSegClicked()
  * This function sends the parameter information up to the Logic
  */
  void onPlaceSeedSeg();

  /**
   * Called when the run segment button is clicked
   * connected to signal from segmentation widget: RunSegmentClicked()
   * This function sends the parameter information up to the Logic
   */
  void onRunSegment();

  /**
   * Called when the hepatic radio button is selected
   * connected to signal from segmentation widget: HepaticSegSelected()
   * This function sends the parameter information up to the Logic
   */
  void onHepaticSeg();

  /**
   * Called when the portal radio button is selected
   * connected to signal from segmentation widget: PortalSegSelected()
   * This function sends the parameter information up to the Logic
   */
  void onPortalSeg();


  /**
   * Called when the merge labelmaps button is clicked
   * connected to signal from splitting widget: MergeLabelMapsClicked()
   * This function sends the parameter information up to the Logic
   */
  void onMergeLabelMaps();

  /**
   * Called when the place seeds is clicked
   * connected to signal from splitting widget: PlaceSeedsMergeClicked()
   * This function sends the parameter information up to the Logic
   */
  void onPlaceSeedsMerge();

  /**
   * Called when the run seed assignment button is clicked
   * connected to signal from splitting widget: RunSeedAssignmentClicked()
   * This function sends the parameter information up to the Logic
   */
  void onRunSeedAssignment();

  /**
   * Called when the hepatic radio button is selected
   * connected to signal from splitting widget: HepaticMergeSelected()
   * This function sends the parameter information up to the Logic
   */
  void onHepaticMerge();

  /**
   * Called when the portal radio button is selected
   * connected to signal from splitting widget: PortalMergeSelected()
   * This function sends the parameter information up to the Logic
   */
  void onPortalMerge();


protected:
  vtkSlicerVesselSegmentationLogic *vesselSegmentationLogic();

  QScopedPointer<qSlicerVesselSegmentationModuleWidgetPrivate> d_ptr;

  /**
   *  Called on initialization of the widget class
   */
  virtual void setup();
    
protected slots:
  void nodeSelectionChanged(vtkMRMLNode*);
  //void setMRMLScene(vtkMRMLScene* scene);

private:
  //qSlicerVesselSegmentationModule *Module;
  //vtkSlicerVesselSegmentationLogic *ModuleLogic;
  vtkEventQtSlotConnect *connections;

  Q_DECLARE_PRIVATE(qSlicerVesselSegmentationModuleWidget);
  Q_DISABLE_COPY(qSlicerVesselSegmentationModuleWidget);
};

#endif
