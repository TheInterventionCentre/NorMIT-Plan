/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationPreprocessingWidget.h

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
#ifndef __qSlicerVesselSegmentationPreprocessingWidget_h
#define __qSlicerVesselSegmentationPreprocessingWidget_h

// This module includes
#include "vtkSlicerVesselSegmentationLogic.h"

// Qt includes
#include <QWidget>
#include <QPointer>

// FooBar Widgets includes
#include "qSlicerVesselSegmentationModuleWidgetsExport.h"

//------------------------------------------------------------------------------
class qSlicerVesselSegmentationPreprocessingWidgetPrivate;
class QString;

/**
 *  \ingroup VesselSegmentation
 *
 * \brief Contains widgets for preprocessing the images for vessel segmentation
 *
 * This class sends and receives information to/from qSlicerVesselSegmentationLogic
 */
class Q_SLICER_MODULE_VESSELSEGMENTATION_WIDGETS_EXPORT qSlicerVesselSegmentationPreprocessingWidget
  : public QWidget
{
  Q_OBJECT;

 public:
  typedef QWidget Superclass;
  qSlicerVesselSegmentationPreprocessingWidget(QWidget *parent=0);
  virtual ~qSlicerVesselSegmentationPreprocessingWidget();

 public slots:

 signals:

 /**
  * Signal emited when the preprocessing button is clicked
  *
  * @param lower threshold from spin box
  * @param upper threshold from spin box
  * @param alpha from spin box
  * @param beta from spin box
  * @param conductance from spin box
  * @param interations from spin box
  */
 void PreprocessingClicked(int lowerThreshold, int upperThreshold, unsigned int alpha, int beta, unsigned int conductance, unsigned int iterations);


 protected slots:

 /**
  * Triggered when preprocessing button pressed
  */
 void PreProcessing();

 /**
   * Triggered when lower threshold spin box is changed
   *
   * @param value of spin box.
   */
  void OnLTSpin(int value);

  /**
   * Triggered when upper threshold spin box is changed
   *
   * @param value of spin box.
   */
  void OnUTSpin(int value);

  /**
   * Triggered when alpha spin box is changed
   *
   * @param value of spin box.
   */
  void OnAlphaSpin(unsigned int value);

  /**
   * Triggered when beta spin box is changed
   *
   * @param value of spin box.
   */
  void OnBetaSpin(int value);

  /**
   * Triggered when conductance spin box is changed
   *
   * @param value of spin box.
   */
  void OnConductanceSpin(unsigned int value);

  /**
   * Triggered when iteration spin box is changed
   *
   * @param value of spin box.
   */
  void OnIterationsSpin(unsigned int value);

 protected:
  QScopedPointer<qSlicerVesselSegmentationPreprocessingWidgetPrivate> d_ptr;

  // values for the preprocessing
  int lowerThreshold;
  int upperThreshold;
  unsigned int alpha;
  int beta;
  unsigned int conductance;
  unsigned int iterations;

 private:
  Q_DECLARE_PRIVATE(qSlicerVesselSegmentationPreprocessingWidget);
  Q_DISABLE_COPY(qSlicerVesselSegmentationPreprocessingWidget);
};


#endif /* VESSELSEGMENTATION_WIDGETS_QSLICERVESSELSEGMENTATIONPREPROCESSINGWIDGET_H_ */
