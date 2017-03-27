/*
 * qListWidgetItemDelegate.cxx
 *
 *  Created on: Mar 22, 2017
 *      Author: louise
 */

#include "qSlicerListWidgetItem.h"

/// \ingroup Slicer_QtModules_ResectionPlanning
//-----------------------------------------------------------------------------
qSlicerListWidgetItem::qSlicerListWidgetItem(QWidget *parent) :
    QWidget (parent)
{
  ui.setupUi(this);
}

int qSlicerListWidgetItem::type() const
{
  return 1001;
}

QStandardItem *qSlicerListWidgetItem::clone() const
{
  return new qSlicerListWidgetItem();
}

void qSlicerListWidgetItem::setText(QString &name)
{
  ui.resectionName->setText(name);
}

Q_DECLARE_METATYPE(qSlicerListWidgetItem*);


