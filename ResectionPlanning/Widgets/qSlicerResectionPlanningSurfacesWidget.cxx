/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningSurfacesWidget.cxx

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
#include "qSlicerResectionPlanningSurfacesWidget.h"
#include "ui_qSlicerResectionPlanningSurfacesWidget.h"
#include "qSlicerTableItemWidget.h"

// VTK includes
#include <vtkObject.h>

// QT includes
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QString>
#include <QSharedPointer>
#include <QWidget>
#include <QLabel>

// STD includes
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningSurfacesWidgetPrivate
  : public Ui_qSlicerResectionPlanningSurfacesWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningSurfacesWidget);
protected:
  qSlicerResectionPlanningSurfacesWidget* const q_ptr;

public:
  qSlicerResectionPlanningSurfacesWidgetPrivate(
    qSlicerResectionPlanningSurfacesWidget& object);
  virtual void setupUi(qSlicerResectionPlanningSurfacesWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidgetPrivate
::qSlicerResectionPlanningSurfacesWidgetPrivate(
  qSlicerResectionPlanningSurfacesWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetPrivate
::setupUi(qSlicerResectionPlanningSurfacesWidget* widget)
{
  this->Ui_qSlicerResectionPlanningSurfacesWidget::setupUi(widget);
}

// --------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::qSlicerResectionPlanningSurfacesWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningSurfacesWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);
  d->setupUi(this);

  // connect signals & slots for buttons to add & remove resection surface
  QObject::connect(d->AddSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnAddSurfaceButtonClicked()));
  QObject::connect(d->RemoveSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveSurfaceButtonClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::~qSlicerResectionPlanningSurfacesWidget()
{

}

//-----------------------------------------------------------------------------
// bool qSlicerResectionPlanningSurfacesWidget::SelectResection(QString &resectionID)
// {
//   Q_D(qSlicerResectionPlanningSurfacesWidget);

//   // find the right tumor to select
//   QMap<QString, int>::iterator it;
//   it = this->resectionIDtoRowMap.find(resectionID);
//   if(it == this->resectionIDtoRowMap.end())
//   {
//     return false; // did not find row in map
//   }
//   d->tableResectionSurfaces->setCurrentCell(it.value(),0);
//   d->tableResectionSurfaces->selectionModel()->select(d->tableResectionSurfaces->selectionModel()->currentIndex(), QItemSelectionModel::Select);

//   std::cout << "SurfacesWidget - select row: " << it.value() << std::endl;
//   return true;
// }

// //-----------------------------------------------------------------------------
// QString qSlicerResectionPlanningSurfacesWidget::GetCurrentResectionID()
// {
//   Q_D(qSlicerResectionPlanningSurfacesWidget);

//   if(d->tableResectionSurfaces->currentRow() >= 0) // check not null (something is selected)
//   {
//     std::cout << "SurfacesWidget - current resection, map length: " << this->resectionIDtoRowMap.size() << '\n';

//     QList<QString> ql = this->resectionIDtoRowMap.keys(d->tableResectionSurfaces->currentRow());
//     if (ql.size() == 0)
//     {
//       return NULL; // did not find node in map
//     }
//     QString resectionID = ql[0];
//     return resectionID;
//   }
//   else
//   {
//     std::cout << "SurfacesWidget - no resection selected " << '\n';
//     return NULL;
//   }
// }

// QList<QString> qSlicerResectionPlanningSurfacesWidget::GetResections()
// {
//   Q_D(qSlicerResectionPlanningSurfacesWidget);

//   QList<QString> resectionList;

//   std::cout << "SurfacesWidget - length resections: " << d->tableResectionSurfaces->rowCount() << std::endl;
//   // loop through the list
//   for(int row = 0; row < d->tableResectionSurfaces->rowCount(); row++)
//   {
//      // find the id of the resection
//      QList<QString> ql = this->resectionIDtoRowMap.keys(row);
//      if (ql.size() > 0)
//      {
//        QString resectionID = ql[0]; // found in map
//        std::cout << "SurfacesWidget - adding to list: " << resectionID.toStdString() << std::endl;
//        resectionList.push_back(resectionID);
//      }
//   }
//   return resectionList;
// }

//-----------------------------------------------------------------------------
// void qSlicerResectionPlanningSurfacesWidget
// ::RemoveFromResectionList(QString &nodeID,QString &nodeName)
// {
//   Q_D(qSlicerResectionPlanningSurfacesWidget);

//   QMap<QString, int>::iterator it;
//   it = this->resectionIDtoRowMap.find(nodeID);
//   if (it == this->resectionIDtoRowMap.end())
//   {
//     return; // cannot find in map
//   }
//   // erase from map(s)
//   this->resectionIDtoRowMap.remove(nodeID);

//   d->tableResectionSurfaces->takeItem(*it,0);

//   std::cout << "SurfacesWidget - Surface removed from scene, and from list: " << nodeName.toStdString() << std::endl;
// }

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnAddSurfaceButtonClicked()
{
  emit AddSurfaceButtonClicked();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnResectionAdded(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if (!callData)
    {
    std::cerr << "Error: No call data on resection added." << std::endl;
    return;
    }

  std::pair<std::string, std::string> *pair =
    static_cast< std::pair<std::string,std::string> *>(callData);


  QString nodeId = QString::fromStdString(pair->first);
  QString nodeName = QString::fromStdString(pair->second);

  if (this->ResectionWidgetMap.find(nodeId) !=
      this->ResectionWidgetMap.end())
    {
    std::cerr << "Warning: node " << nodeId.toStdString()
              << "already exists" << std::endl;
    return;
    }

  // Create the table item widget
  qSlicerTableItemWidget *item = new qSlicerTableItemWidget();
  item->setResectionID(nodeId);
  item->setText(nodeName);

  // Insert the item widget
  int row = d->TableResectionSurfaces->rowCount();
  d->TableResectionSurfaces->insertRow(row);
  d->TableResectionSurfaces->setCellWidget(row,0, item);

  // Register the nodeId -- itemWidget association
  this->ResectionWidgetMap[nodeId] = item;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnRemoveSurfaceButtonClicked()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  qSlicerTableItemWidget *widget =
    dynamic_cast<qSlicerTableItemWidget*>(
    d->TableResectionSurfaces->
    cellWidget(d->TableResectionSurfaces->currentRow(),0));

  if (!widget)
    {
    std::cerr << "Warning: no row selected" << std::endl;
    return;
    }

  QString nodeId = this->ResectionWidgetMap.key(widget);
  if (nodeId.isNull())
    {
    std::cerr << "Warning: no nodeId found for the given widget" << std::endl;
    return;
    }

  emit RemoveSurface(nodeId);
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget::
OnResectionRemoved(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if (!callData)
    {
    std::cerr << "Error: no call data on resection removed" << std::endl;
    return;
    }

  std::pair<std::string, std::string> *pair =
    reinterpret_cast< std::pair<std::string,std::string> *>(callData);

  QString nodeId = QString::fromStdString(pair->first);

  ResectionWidgetIt it = this->ResectionWidgetMap.find(nodeId);
  if (it == this->ResectionWidgetMap.end())
    {
    std::cerr << "Warning: node " << nodeId.toStdString()
              << "does not exist in the list of resection surfaces"
              << std::endl;
    return;
    }

  int row;
  for(row = 0; row< d->TableResectionSurfaces->rowCount(); ++row)
    {
    qSlicerTableItemWidget *widget =
      dynamic_cast<qSlicerTableItemWidget*>(
        d->TableResectionSurfaces->cellWidget(row,0));
    if (widget)
      {
      if (widget == it.value())
        {
        break;
        }
      }
    }
  d->TableResectionSurfaces->removeCellWidget(row,0);
  d->TableResectionSurfaces->removeRow(row);
  this->ResectionWidgetMap.erase(it);
}


//-----------------------------------------------------------------------------
// void qSlicerResectionPlanningSurfacesWidget
// ::OnCurrentResectionSurfaceChanged(int rowOld,
//                                    int columnOld,
//                                    int rowNew,
//                                    int columnNew)
// {
//   // find the id of the resection
//   QList<QString> ql = this->resectionIDtoRowMap.keys(rowNew);
//   if (ql.size() > 0)
//   {
//     QString resectionID = ql[0]; // found in map
//     emit CurrentResectionSurfaceChanged(resectionID);
//   }
// }
