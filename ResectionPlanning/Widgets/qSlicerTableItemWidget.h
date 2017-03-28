/*
 * qSlicerTableItemWidget.h
 *
 *  Created on: Mar 28, 2017
 *      Author: louise
 */

#ifndef RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_
#define RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_

// Qt includes
#include <QTableWidgetItem>
#include <QString>
#include <QWidget>

#include "ui_qSlicerTableItemWidget.h"

// MRMLWidgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerTableItemWidget : public QWidget, public QTableWidgetItem
{
  Q_OBJECT

public:
  qSlicerTableItemWidget(QWidget *parent = 0);

  int type() const;

  virtual QTableWidgetItem *clone() const;

  void setText(QString&);

private:
  Ui::qSlicerTableItem ui;

};

#endif /* RESECTIONPLANNING_WIDGETS_QSLICERTABLEITEMWIDGET_H_ */
