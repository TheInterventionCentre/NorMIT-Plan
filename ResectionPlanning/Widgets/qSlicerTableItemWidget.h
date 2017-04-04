/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerTableItemWidget.h
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

#ifndef RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_
#define RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_

// Qt includes
#include <QString>
#include <QWidget>

#include "ui_qSlicerTableItemWidget.h"

// MRMLWidgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

/**
 *  \ingroup ResectionPlanning
 *
 * \brief Contains widget used inside the resection table
 *
 * This class sends and receives information to/from qSlicerResectionPlanningSurfacesWidget
 */
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerTableItemWidget : public QWidget
{
  Q_OBJECT

public:
  qSlicerTableItemWidget(QWidget *parent = 0);
  virtual ~qSlicerTableItemWidget();

  int type() const;

  /**
   * Set the text of the label for the resection name
   *
   * @param resection name
   */
  void setResectionName(const char*);
  const char* getResectionName() const;

  /**
   * Set the visibility of the resection (checkbox)
   *
   * @param visibility true if resection is visible, false otherwise.
   */
  void setResectionVisibility(bool visibility);

  /**
   * Get the visibility of the resection (checkbox)
   *
   *
   * @return true if resection is visible, false otherwise.
   */
  bool getResectionVisibility() const;

  /**
   * Set the resection margin in the double spinbox
   *
   * @param margin resection margin.
   */
  void setResectionMargin(double margin);

  /**
   * Get the resection margin in the spinbox
   *
   *
   * @return resection margin
   */
  double getResectionMargin() const;

  /**
   * Set the transparency value for the slider
   *
   * @param value transparency value.
   */
  void setTransparency(int value);

  /**
   * Get the transparency value from the slider.
   *
   *
   * @return transparency value
   */
  int getTransparency() const;

  /**
   * Set the visibility of the widget (checkbox)
   *
   * @param visibility true if widget is visible, false otherwise.
   */
  void setWidgetVisibility(bool visibility);

  /**
   * Get the visibility of the widget (checkbox)
   *
   *
   * @return true if widget is visible, false otherwise.
   */
  bool getWidgetVisibility() const;

signals:
  /**
  * Used to set the visibility checkbox state
  *
  * @param bool for state of checkbox
  */
  void setVisibleCheckbox(bool);

  /**
  * Used to set the widget checkbox state
  *
  * @param bool for state of checkbox
  */
  void setWidgetCheckbox(bool);

  /**
  * Used to set the spinbox value
  *
  * @param double for value of spinbox
  */
  void setMarginSpinbox(double);

  /**
  * Used to set the slider value
  *
  * @param int for value of slider
  */
  void setTransparencySlider(int);

protected slots:
  /**
   * Called when the visibility checkbox state is changed
   *
   * @param bool for state of checkbox
   */
  void visibleCheckboxChanged(bool);

  /**
   * Called when the widget checkbox state is changed
   *
   * @param bool for state of checkbox
   */
  void widgetCheckboxChanged(bool);

  /**
   * Called when the margin spinbox state is changed
   *
   * @param double for value of spinbox
   */
  void marginSpinboxChanged(double);

  /**
   * Called when the transparency slider state is changed
   *
   * @param in for value of slider
   */
  void transparencySliderChanged(int);

private:

  Ui::qSlicerTableItem ui;
};

Q_DECLARE_METATYPE(qSlicerTableItemWidget*);


#endif /* RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_ */
