/*
 * qSlicerListWidgetItemDelegate.h
 *
 *  Created on: Mar 23, 2017
 *      Author: louise
 */

#ifndef RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEMDELEGATE_H_
#define RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEMDELEGATE_H_

// Qt includes
#include <QStyledItemDelegate>
#include <QModelIndex>

// MRMLWidgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerListWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    qSlicerListWidgetItemDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

};


#endif /* RESECTIONPLANNING_WIDGETS_QSLICERLISTWIDGETITEMDELEGATE_H_ */
