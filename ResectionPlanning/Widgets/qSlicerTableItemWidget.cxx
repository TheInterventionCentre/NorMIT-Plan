/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerTableItemWidget.cxx

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
#include "qSlicerTableItemWidget.h"

#include <iostream>

/// \ingroup Slicer_QtModules_ResectionPlanning
//-----------------------------------------------------------------------------
qSlicerTableItemWidget::qSlicerTableItemWidget(QWidget *parent) :
    QWidget (parent)
{
  ui.setupUi(this);

  QObject::connect(ui.resectionVisibleCheckbox, SIGNAL(stateChanged(int)),
                   this, SLOT(onVisibilityCheckBoxChanged(int)));

}

//-----------------------------------------------------------------------------
qSlicerTableItemWidget
::~qSlicerTableItemWidget()
{

}

//-----------------------------------------------------------------------------
int qSlicerTableItemWidget::type() const
{
  return 1001;
}

//-----------------------------------------------------------------------------
void qSlicerTableItemWidget::setResectionName(const char *name)
{
  ui.resectionName->setText(name);
}

//-----------------------------------------------------------------------------
const char* qSlicerTableItemWidget::getResectionName() const
{
  return ui.resectionName->text().toStdString().c_str();
}

//------------------------------------------------------------------------------
void qSlicerTableItemWidget::setResectionVisibility(bool visibility)
{
  ui.resectionVisibleCheckbox->
    setCheckState(visibility?Qt::Checked:Qt::Unchecked);
}

//------------------------------------------------------------------------------
bool qSlicerTableItemWidget::getResectionVisibility() const
{
  return (Qt::Checked == ui.resectionVisibleCheckbox->checkState())? true: false;
}

//------------------------------------------------------------------------------
void qSlicerTableItemWidget::setResectionMargin(double margin)
{
  ui.marginSpinbox->setValue(margin);
}

//------------------------------------------------------------------------------
double qSlicerTableItemWidget::getResectionMargin() const
{
  return ui.marginSpinbox->value();
}

//------------------------------------------------------------------------------
void qSlicerTableItemWidget::setTransparency(int value)
{
  if (value < 0 || value > 100)
    {
    std::cerr << "Error: transparency value out of limits ([0-100]), got: "
              << value << std::endl;
    return;
    }

  ui.transparencySlider->setValue(value);
}

//------------------------------------------------------------------------------
int qSlicerTableItemWidget::getTransparency() const
{
  return ui.transparencySlider->value();
}

//------------------------------------------------------------------------------
void qSlicerTableItemWidget::setWidgetVisibility(bool visibility)
{
  ui.widgetVisibleCheckbox->
    setCheckState(visibility?Qt::Checked:Qt::Unchecked);
}

//------------------------------------------------------------------------------
bool qSlicerTableItemWidget::getWidgetVisibility() const
{
  return (Qt::Checked == ui.widgetVisibleCheckbox->checkState())? true: false;
}


/*
 * SLOTS for widgets to connect with
 */
//-----------------------------------------------------------------------------
void qSlicerTableItemWidget::onVisibilityCheckBoxChanged(int state)
{
  emit visibilityChanged(state);
}

//-----------------------------------------------------------------------------
void qSlicerTableItemWidget::widgetCheckboxChanged(bool state)
{

}

//-----------------------------------------------------------------------------
void qSlicerTableItemWidget::marginSpinboxChanged(double value)
{

}

//-----------------------------------------------------------------------------
void qSlicerTableItemWidget::transparencySliderChanged(int value)
{

}
