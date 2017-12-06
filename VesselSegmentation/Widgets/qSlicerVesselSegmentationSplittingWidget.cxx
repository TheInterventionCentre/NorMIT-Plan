/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationSplittingWidget.cxx

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

// This module includes
#include "qSlicerVesselSegmentationSplittingWidget.h"
#include "ui_qSlicerVesselSegmentationSplittingWidget.h"

#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScalarVolumeNode.h>

// VTK includes
#include <vtkObject.h>

// QT includes
#include <QString>

// STD includes
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VesselSegmentation
class qSlicerVesselSegmentationSplittingWidgetPrivate
  : public Ui_qSlicerVesselSegmentationSplittingWidget
{
  Q_DECLARE_PUBLIC(qSlicerVesselSegmentationSplittingWidget);
protected:
  qSlicerVesselSegmentationSplittingWidget* const q_ptr;

public:
  qSlicerVesselSegmentationSplittingWidgetPrivate(
    qSlicerVesselSegmentationSplittingWidget& object);
  virtual void setupUi(qSlicerVesselSegmentationSplittingWidget*);
};

// --------------------------------------------------------------------------
qSlicerVesselSegmentationSplittingWidgetPrivate
::qSlicerVesselSegmentationSplittingWidgetPrivate(
  qSlicerVesselSegmentationSplittingWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerVesselSegmentationSplittingWidgetPrivate
::setupUi(qSlicerVesselSegmentationSplittingWidget* widget)
{
  this->Ui_qSlicerVesselSegmentationSplittingWidget::setupUi(widget);

}

// --------------------------------------------------------------------------
qSlicerVesselSegmentationSplittingWidget
::qSlicerVesselSegmentationSplittingWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerVesselSegmentationSplittingWidgetPrivate(*this) )
{
  Q_D(qSlicerVesselSegmentationSplittingWidget);
  d->setupUi(this);

  isHepatic = true;
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationSplittingWidget
::~qSlicerVesselSegmentationSplittingWidget()
{

}

//------------------------------------------------------------------------------
void qSlicerVesselSegmentationSplittingWidget::MergeLabelMaps()
{
  //std::cout << "splitWidget - Merge label maps" << std::endl;

  emit MergeLabelMapsClicked();
}

void qSlicerVesselSegmentationSplittingWidget::PlaceSeedsSplit()
{
  //std::cout << "splitWidget - Place seeds (split)" << std::endl;

  emit PlaceSeedsSplitClicked();
}

void qSlicerVesselSegmentationSplittingWidget::RunSeedAssignment()
{
  //std::cout << "splitWidget - Run seed assignment" << std::endl;

  emit RunSeedAssignmentClicked(this->isHepatic);
}

void qSlicerVesselSegmentationSplittingWidget::onTogglePlaceSeedSplit()
{
  //std::cout << "splitWidget - raise place seeds" << std::endl;

  emit ToggleButtonSeedSplit();
}

void qSlicerVesselSegmentationSplittingWidget::OnHepaticSplit()
{
   //std::cout << "splitWidget - On Hepatic (split)" << std::endl;

   this->isHepatic = true;
}

void qSlicerVesselSegmentationSplittingWidget::OnPortalSplit()
{
   //std::cout << "Widget - On Portal (split)" << std::endl;

   this->isHepatic = true;
}
