/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningVolumesWidget.h
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

#ifndef __qSlicerResectionPlanningVolumesWidget_h
#define __qSlicerResectionPlanningVolumesWidget_h

// Qt includes
#include <QWidget>
#include <QString>

// FooBar Widgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class qSlicerResectionPlanningVolumesWidgetPrivate;

/**
 *  \ingroup ResectionPlanning
 *
 * \brief Contains widgets for volume calculations
 *
 * This class sends and receives information to/from qSlicerResectionPlanningModuleWidget
 */
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerResectionPlanningVolumesWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerResectionPlanningVolumesWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningVolumesWidget();

signals:
  /**
   * Signal emited when the button to calculate the volumes is clicked
   */
void VolumesButtonClicked(QString&);


protected slots:
  /**
   * Triggered when the button for calculating volumetry is clicked
   */
  void OnCalculateVolumetryClicked();

protected:
  QScopedPointer<qSlicerResectionPlanningVolumesWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningVolumesWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningVolumesWidget);
};

#endif