/*
 * qSlicerTableItemWidget.cxx
 *
 *  Created on: Mar 28, 2017
 *      Author: louise
 */

#include "qSlicerTableItemWidget.h"

/// \ingroup Slicer_QtModules_ResectionPlanning
//-----------------------------------------------------------------------------
qSlicerTableItemWidget::qSlicerTableItemWidget(QWidget *parent) :
    QWidget (parent)
{
  ui.setupUi(this);
}

qSlicerTableItemWidget
::~qSlicerTableItemWidget()
{

}

int qSlicerTableItemWidget::type() const
{
  return 1001;
}

QTableWidgetItem *qSlicerTableItemWidget::clone() const
{
  return new qSlicerTableItemWidget();
}

void qSlicerTableItemWidget::setText(QString &name)
{
  ui.resectionName->setText(name);
}

QString qSlicerTableItemWidget::text()
{
  return ui.resectionName->text();
}


