/*
 * qListWidgetItemDelegate.h
 *
 *  Created on: Mar 22, 2017
 *      Author: louise
 */

#ifndef RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_
#define RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_

// Qt includes
#include <QItemDelegate>
#include <QModelIndex>

#include "ui_qSlicerListWidgetItem.h"

// MRMLWidgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerListWidgetItem : public QItemDelegate
{
  Q_OBJECT
public:
  qSlicerListWidgetItem(QObject *parent = 0);

private:
  Ui::qSlicerListWidgetItem ui;

};

#endif /* RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEM_H_ */
