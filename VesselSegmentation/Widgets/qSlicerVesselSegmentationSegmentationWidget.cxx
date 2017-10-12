/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationSegmentationWidget.cxx

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
#include "qSlicerVesselSegmentationSegmentationWidget.h"
#include "ui_qSlicerVesselSegmentationSegmentationWidget.h"

// VTK includes
#include <vtkObject.h>

// QT includes
#include <QString>
#include <QWidget>

// STD includes
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VesselSegmentation
class qSlicerVesselSegmentationSegmentationWidgetPrivate
  : public Ui_qSlicerVesselSegmentationSegmentationWidget
{
  Q_DECLARE_PUBLIC(qSlicerVesselSegmentationSegmentationWidget);
protected:
  qSlicerVesselSegmentationSegmentationWidget* const q_ptr;

public:
  qSlicerVesselSegmentationSegmentationWidgetPrivate(
    qSlicerVesselSegmentationSegmentationWidget& object);
  virtual void setupUi(qSlicerVesselSegmentationSegmentationWidget*);
};

// --------------------------------------------------------------------------
qSlicerVesselSegmentationSegmentationWidgetPrivate
::qSlicerVesselSegmentationSegmentationWidgetPrivate(
  qSlicerVesselSegmentationSegmentationWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerVesselSegmentationSegmentationWidgetPrivate
::setupUi(qSlicerVesselSegmentationSegmentationWidget* widget)
{
  this->Ui_qSlicerVesselSegmentationSegmentationWidget::setupUi(widget);

}

// --------------------------------------------------------------------------
qSlicerVesselSegmentationSegmentationWidget
::qSlicerVesselSegmentationSegmentationWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerVesselSegmentationSegmentationWidgetPrivate(*this) )
{
  Q_D(qSlicerVesselSegmentationSegmentationWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationSegmentationWidget
::~qSlicerVesselSegmentationSegmentationWidget()
{

}

//-----------------------------------------------------------------------------
void qSlicerVesselSegmentationSegmentationWidget::PlaceSeedsSeg()
{
  std::cout << "Widget - Place Seeds (Segment)" << std::endl;

  emit PlaceSeedsSegClicked();
}

void qSlicerVesselSegmentationSegmentationWidget::RunSegment()
{
  std::cout << "Widget - Run Segment" << std::endl;

  emit RunSegmentClicked();
}
void qSlicerVesselSegmentationSegmentationWidget::OnHepaticSeg()
{
    std::cout << "Widget - On Hepatic (Segment)" << std::endl;

    emit HepaticSegSelected();
}

void qSlicerVesselSegmentationSegmentationWidget::OnPortalSeg()
{
   std::cout << "Widget - On Portal (segment)" << std::endl;

   emit PortalSegSelected();
}

