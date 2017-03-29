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

// FooBar Widgets includes
#include "qSlicerResectionPlanningSurfacesWidget.h"
#include "ui_qSlicerResectionPlanningSurfacesWidget.h"

#include "qSlicerTableItemWidget.h"

#include <iostream>
#include <string>

#include <QListWidget>
#include <QTableWidget>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include <QLabel>
#include <QString>
#include <QSharedPointer>


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

qSlicerResectionPlanningSurfacesWidget
::qSlicerResectionPlanningSurfacesWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningSurfacesWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);
  d->setupUi(this);

  //d->tableResectionSurfaces->setItemPrototype(new qSlicerTableItemWidget());

  // start with no rows
  d->tableResectionSurfaces->setRowCount(0);
  d->tableResectionSurfaces->setColumnCount(1);
  d->tableResectionSurfaces->setSelectionMode(QAbstractItemView::SingleSelection);
  d->tableResectionSurfaces->horizontalHeader()->setStretchLastSection(true);
  d->tableResectionSurfaces->horizontalHeader()->hide();


  // test:
  qSlicerTableItemWidget *item = new qSlicerTableItemWidget();
  QString text1 = "TestResection";
  QString ID1 = "ID1";
  item->setText(text1);
  item->setResectionID(ID1);

  int row = d->tableResectionSurfaces->rowCount();
  d->tableResectionSurfaces->insertRow(row);
  d->tableResectionSurfaces->setCellWidget(row,0,item);

  qSlicerTableItemWidget *item2 = new qSlicerTableItemWidget();
  QString text2 = "TestResection2";
  QString ID2 = "ID2";
  item2->setText(text2);
  item2->setResectionID(ID2);

  row = d->tableResectionSurfaces->rowCount();
  d->tableResectionSurfaces->insertRow(row);
  d->tableResectionSurfaces->setCellWidget(row,0,item2);


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
// Current resection methods
//-----------------------------------------------------------------------------
bool qSlicerResectionPlanningSurfacesWidget::SelectResection(QString &resectionID)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  // find the right tumor to select
  QMap<QString, int>::iterator it;
  it = this->resectionIDtoRowMap.find(resectionID);
  if(it == this->resectionIDtoRowMap.end())
  {
    return false; // did not find row in map
  }
  d->tableResectionSurfaces->setCurrentCell(it.value(),0);
  d->tableResectionSurfaces->selectionModel()->select(d->tableResectionSurfaces->selectionModel()->currentIndex(), QItemSelectionModel::Select);

  std::cout << "SurfacesWidget - select row: " << it.value() << std::endl;
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningSurfacesWidget::GetCurrentResectionID()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if(d->tableResectionSurfaces->currentRow() >= 0) // check not null (something is selected)
  {
    std::cout << "SurfacesWidget - current resection, map length: " << this->resectionIDtoRowMap.size() << '\n';

    QList<QString> ql = this->resectionIDtoRowMap.keys(d->tableResectionSurfaces->currentRow());
    if (ql.size() == 0)
    {
      return NULL; // did not find node in map
    }
    QString resectionID = ql[0];
    return resectionID;
  }
  else
  {
    std::cout << "SurfacesWidget - no resection selected " << '\n';
    return NULL;
  }
}

QList<QString> qSlicerResectionPlanningSurfacesWidget::GetResections()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QList<QString> resectionList;

  std::cout << "SurfacesWidget - length resections: " << d->tableResectionSurfaces->rowCount() << std::endl;
  // loop through the list
  for(int row = 0; row < d->tableResectionSurfaces->rowCount(); row++)
  {
     // find the id of the resection
     QList<QString> ql = this->resectionIDtoRowMap.keys(row);
     if (ql.size() > 0)
     {
       QString resectionID = ql[0]; // found in map
       std::cout << "SurfacesWidget - adding to list: " << resectionID.toStdString() << std::endl;
       resectionList.push_back(resectionID);
     }
  }
  return resectionList;
}

//-----------------------------------------------------------------------------
// Add / remove from resection list
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::AddToResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  qSlicerTableItemWidget *item = new qSlicerTableItemWidget();
  item->setText(nodeName);
  item->setResectionID(nodeID);

  int row = d->tableResectionSurfaces->rowCount();
  d->tableResectionSurfaces->insertRow(row);
  d->tableResectionSurfaces->setCellWidget(row,0,item);

  // keep in map(s)
  this->resectionIDtoRowMap.insert(nodeID, row);

  std::cout << "SurfacesWidget - Surface added to scene, and to list: " << nodeName.toStdString() << ", row: " << row << std::endl;
}

void qSlicerResectionPlanningSurfacesWidget
::RemoveFromResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QMap<QString, int>::iterator it;
  it = this->resectionIDtoRowMap.find(nodeID);
  if (it == this->resectionIDtoRowMap.end())
  {
    return; // cannot find in map
  }
  // erase from map(s)
  this->resectionIDtoRowMap.remove(nodeID);

  d->tableResectionSurfaces->takeItem(*it,0);

  std::cout << "SurfacesWidget - Surface removed from scene, and from list: " << nodeName.toStdString() << std::endl;
}

//-----------------------------------------------------------------------------
// Create / remove resections
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnAddSurfaceButtonClicked()
{
  std::cout << "SurfacesWidget - On Add Surface" << std::endl;

  // EMIT SIGNAL
  emit AddSurfaceButtonClicked();
}

void qSlicerResectionPlanningSurfacesWidget
::OnRemoveSurfaceButtonClicked()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  std::cout << "SurfacesWidget - On Remove Surface" << std::endl;

  if(d->tableResectionSurfaces->currentRow() >= 0) // check not null (something is selected)
  {
    // find which one is currently highlighted
    QList<QString> ql = this->resectionIDtoRowMap.keys(d->tableResectionSurfaces->currentRow());
    if (ql.size() == 0)
    {
      return; // did not find node in map
    }
    QString resectionID = ql[0];
    QString resectionName = d->tableResectionSurfaces->currentItem()->text();

    // EMIT SIGNAL
    emit RemoveSurfaceButtonClicked(resectionID, resectionName);
  }
  else
  {
    std::cout << "SurfacesWidget - No resection selected for deletion" << std::endl;
  }
}

//-----------------------------------------------------------------------------
// Callback for selected resection changing
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnCurrentResectionSurfaceChanged(int rowOld, int columnOld, int rowNew, int columnNew)
{
  // find the id of the resection
  QList<QString> ql = this->resectionIDtoRowMap.keys(rowNew);
  if (ql.size() > 0)
  {
    QString resectionID = ql[0]; // found in map
    std::cout << "SurfacesWidget - Selected surface changed: " << resectionID.toStdString() << std::endl;
    emit CurrentResectionSurfaceChanged(resectionID);
  }
}
