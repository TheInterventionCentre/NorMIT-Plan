/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationPreprocessingWidget.cxx

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
#include "qSlicerVesselSegmentationPreprocessingWidget.h"
#include "ui_qSlicerVesselSegmentationPreprocessingWidget.h"

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
class qSlicerVesselSegmentationPreprocessingWidgetPrivate
  : public Ui_qSlicerVesselSegmentationPreprocessingWidget
{
  Q_DECLARE_PUBLIC(qSlicerVesselSegmentationPreprocessingWidget);
protected:
  qSlicerVesselSegmentationPreprocessingWidget* const q_ptr;

public:
  qSlicerVesselSegmentationPreprocessingWidgetPrivate(
    qSlicerVesselSegmentationPreprocessingWidget& object);
  virtual void setupUi(qSlicerVesselSegmentationPreprocessingWidget*);
};

// --------------------------------------------------------------------------
qSlicerVesselSegmentationPreprocessingWidgetPrivate
::qSlicerVesselSegmentationPreprocessingWidgetPrivate(
  qSlicerVesselSegmentationPreprocessingWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerVesselSegmentationPreprocessingWidgetPrivate
::setupUi(qSlicerVesselSegmentationPreprocessingWidget* widget)
{
  this->Ui_qSlicerVesselSegmentationPreprocessingWidget::setupUi(widget);

}

// --------------------------------------------------------------------------
qSlicerVesselSegmentationPreprocessingWidget
::qSlicerVesselSegmentationPreprocessingWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerVesselSegmentationPreprocessingWidgetPrivate(*this) )
{
  Q_D(qSlicerVesselSegmentationPreprocessingWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationPreprocessingWidget
::~qSlicerVesselSegmentationPreprocessingWidget()
{

}

/*
 * Called when preprocessing button clicked
 */
void qSlicerVesselSegmentationPreprocessingWidget::PreProcessing()
{
  std::cout << "Widget - On PreProcessing" << std::endl;

  emit PreprocessingClicked();
}

void qSlicerVesselSegmentationPreprocessingWidget::OnLTSpin(int value)
{
  std::cout << "Widget - On LT spin " << value << std::endl;

  emit LTSpinChanged(value);
}

void qSlicerVesselSegmentationPreprocessingWidget::OnUTSpin(int value)
{
  std::cout << "Widget - On UT spin " << value << std::endl;

  emit UTSpinChanged(value);
}

void qSlicerVesselSegmentationPreprocessingWidget::OnAlphaSpin(int value)
{
  std::cout << "Widget - On Alpha spin " << value << std::endl;

  emit AlphaSpinChanged(value);
}

void qSlicerVesselSegmentationPreprocessingWidget::OnBetaSpin(int value)
{
  std::cout << "Widget - On Beta spin " << value << std::endl;

  emit BetaSpinChanged(value);
}

void qSlicerVesselSegmentationPreprocessingWidget::OnConductanceSpin(int value)
{
  std::cout << "Widget - On Conductance spin " << value << std::endl;

  emit ConductanceSpinChanged(value);
}

void qSlicerVesselSegmentationPreprocessingWidget::OnIterationsSpin(int value)
{
  std::cout << "Widget - On Iterations spin " << value << std::endl;

  emit IterationsSpinChanged(value);
}


