/*
 * qListWidgetItemDelegate.h
 *
 *  Created on: Mar 22, 2017
 *      Author: louise
 */

#ifndef RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_
#define RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_

// Qt includes
#include <QStandardItem>
#include <QListWidgetItem>
#include <QString>
#include <QWidget>

#include "ui_qSlicerListWidgetItem.h"

// MRMLWidgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerListWidgetItem : public QWidget, public QStandardItem
{
  Q_OBJECT

public:
  qSlicerListWidgetItem(QWidget *parent = 0);

  int type() const;

  virtual QStandardItem *clone() const;

  void setText(QString&);

private:
  Ui::qSlicerListItem ui;

};

#endif /* RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_ */
