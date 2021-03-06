/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningVolumesWidget.cxx
Copyright (c) 2017 The Intervention Centre, Oslo University Hospital
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

// FooBar Widgets includes
#include "qSlicerResectionPlanningVolumesWidget.h"
#include "ui_qSlicerResectionPlanningVolumesWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningVolumesWidgetPrivate
  : public Ui_qSlicerResectionPlanningVolumesWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningVolumesWidget);
protected:
  qSlicerResectionPlanningVolumesWidget* const q_ptr;

public:
  qSlicerResectionPlanningVolumesWidgetPrivate(
    qSlicerResectionPlanningVolumesWidget& object);
  virtual void setupUi(qSlicerResectionPlanningVolumesWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidgetPrivate
::qSlicerResectionPlanningVolumesWidgetPrivate(
  qSlicerResectionPlanningVolumesWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningVolumesWidgetPrivate
::setupUi(qSlicerResectionPlanningVolumesWidget* widget)
{
  this->Ui_qSlicerResectionPlanningVolumesWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidget
::qSlicerResectionPlanningVolumesWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningVolumesWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningVolumesWidget);
  d->setupUi(this);

  QObject::connect(d->calculateVolumetryButton, SIGNAL(clicked()),
                   this, SLOT(OnCalculateVolumetryClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidget
::~qSlicerResectionPlanningVolumesWidget()
{
}


//-----------------------------------------------------------------------------
void qSlicerResectionPlanningVolumesWidget
::OnCalculateVolumetryClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningVolumesWidget);

  emit VolumesButtonClicked();
}
