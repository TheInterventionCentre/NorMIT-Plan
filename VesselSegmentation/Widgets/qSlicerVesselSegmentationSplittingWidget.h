/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationSplittingWidget.h

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

#ifndef __qSlicerVesselSegmentationSplittingWidget_h
#define __qSlicerVesselSegmentationSplittingWidget_h

// This module includes
#include "vtkSlicerVesselSegmentationLogic.h"

// Qt includes
#include <QWidget>
#include <QPointer>

// FooBar Widgets includes
#include "qSlicerVesselSegmentationModuleWidgetsExport.h"

//------------------------------------------------------------------------------
class qSlicerVesselSegmentationSplittingWidgetPrivate;
class QString;

/**
 *  \ingroup VesselSegmentation
 *
 * \brief Contains widgets for splitting areas where portal and hepatic vessel segmentations have merged
 *
 * This class sends and receives information to/from qSlicerVesselSegmentationModuleWidget
 */
class Q_SLICER_MODULE_VESSELSEGMENTATION_WIDGETS_EXPORT qSlicerVesselSegmentationSplittingWidget
  : public QWidget
{
  Q_OBJECT;

 public:
  typedef QWidget Superclass;
  qSlicerVesselSegmentationSplittingWidget(QWidget *parent=0);
  virtual ~qSlicerVesselSegmentationSplittingWidget();

 public slots:

 signals:

 /**
  * Signal emitted when the merge labelmaps button is clicked
  */
 void MergeLabelMapsClicked();

 /**
  * Signal emitted when the place seeds button is clicked
  */
 void PlaceSeedsMergeClicked();

 /**
  * Signal emitted when the run seed assignment button is clicked
  */
 void RunSeedAssignmentClicked(bool isHepatic);

 protected slots:

 /**
  * Triggered when merge label maps button pressed
  */
 void MergeLabelMaps();

 /**
  * Triggered when place seeds button pressed
  */
 void PlaceSeedsMerge();

 /**
  * Triggered when run seeds assignment button pressed
  */
 void RunSeedAssignment();

 /**
  * Triggered when hepatic radio button selected
  */
 void OnHepaticMerge();

 /**
  * Triggered when portal radio button selected
  */
 void OnPortalMerge();

 protected:
  QScopedPointer<qSlicerVesselSegmentationSplittingWidgetPrivate> d_ptr;

  bool isHepatic;

 private:
  Q_DECLARE_PRIVATE(qSlicerVesselSegmentationSplittingWidget);
  Q_DISABLE_COPY(qSlicerVesselSegmentationSplittingWidget);
};


#endif /* VESSELSEGMENTATION_WIDGETS_QSLICERVESSELSEGMENTATIONSPLITTINGWIDGET_H_ */
