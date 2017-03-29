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
#include <QTableWidgetItem>
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
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerTableItemWidget : public QWidget, public QTableWidgetItem
{
  Q_OBJECT

public:
  qSlicerTableItemWidget(QWidget *parent = 0);
  virtual ~qSlicerTableItemWidget();

  int type() const;

  virtual QTableWidgetItem *clone() const;

  /**
   * Set the text of the label for the resection name
   *
   * @param resection name
   */
  void setText(QString&);
  QString text();

  /**
   * Set the resection ID
   *
   * @param resection ID
   */
  void setResectionID(QString&);
  QString getResectionID();

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
  * Used to set the slider value
  *
  * @param double for value of slider
  */
  void setMarginSlider(double);

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
   * Called when the visibility checkbox state is changed
   *
   * @param double for value of slider
   */
  void marginSliderChanged(double);

private:
  QString resectionName;
  QString resectionID;
  Ui::qSlicerTableItem ui;
};

Q_DECLARE_METATYPE(qSlicerTableItemWidget*);


#endif /* RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_ */
