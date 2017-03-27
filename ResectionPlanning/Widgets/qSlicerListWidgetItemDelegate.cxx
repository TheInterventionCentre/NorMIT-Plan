/*
 * qSlicerListWidgetItemDelegate.cxx
 *
 *  Created on: Mar 23, 2017
 *      Author: louise
 */

#include "qSlicerListWidgetItemDelegate.h"
#include "qSlicerListWidgetItem.h"


//-----------------------------------------------------------------------------
qSlicerListWidgetItemDelegate::qSlicerListWidgetItemDelegate(QObject *parent)
  : QStyledItemDelegate(parent) { }

void qSlicerListWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

//-----------------------------------------------------------------------------
QWidget* qSlicerListWidgetItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
  qSlicerListWidgetItem *editor = new qSlicerListWidgetItem(parent);
  return editor;
}

//-----------------------------------------------------------------------------
void qSlicerListWidgetItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  double value = index.model()->data(index, Qt::EditRole).toDouble();
  qSlicerListWidgetItem *item = static_cast<qSlicerListWidgetItem*>(editor);

  QStyledItemDelegate::setEditorData(item, index);
  //item->setValue(value);
}

//-----------------------------------------------------------------------------
void qSlicerListWidgetItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  qSlicerListWidgetItem *item = static_cast<qSlicerListWidgetItem*>(editor);

  QStyledItemDelegate::setModelData(item, model, index);
  //item->interpretText();
  //double value = item->value();
  //model->setData(index, value, Qt::EditRole);
}


