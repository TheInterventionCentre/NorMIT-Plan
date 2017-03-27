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

#include "qSlicerListWidgetItemDelegate.h"
#include "qSlicerListWidgetItem.h"

#include <iostream>
#include <string>

#include <QListView>
#include <QStandardItem>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>
#include <QString>
#include <QTreeView>

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

  // connect signals & slots for buttons to add & remove resection surface
  QObject::connect(d->AddSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnAddSurfaceButtonClicked()));
  QObject::connect(d->RemoveSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveSurfaceButtonClicked()));

  model = new QStandardItemModel();



  model->setItemPrototype(new qSlicerListWidgetItem());
  d->listResectionSurfaces->setModel(model);

  /*
  QStandardItem *item2 = new QStandardItem();
  item2->setText("xxxxxxxxxx");
  model->appendRow(item2);*/

  qSlicerListWidgetItem *item = new qSlicerListWidgetItem();
  QString name = "test";
  item->setText(name);
  model->appendRow(item);

  d->listResectionSurfaces->setItemDelegate(new qSlicerListWidgetItemDelegate(d->listResectionSurfaces));

  d->listResectionSurfaces->setItemDelegateForRow(0, new qSlicerListWidgetItemDelegate(d->listResectionSurfaces));

  //d->listResectionSurfaces2->addItem(item);
  //d->listResectionSurfaces2->setItemWidget(item, new qSlicerListWidgetItem());

  std::cout << "SurfacesWidget - instatiation: " << model->itemPrototype() << std::endl;
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
  QMap<QString, QListWidgetItem*>::iterator it;
  it = this->resectionIDtoItemMap.find(resectionID);
  if(it == this->resectionIDtoItemMap.end())
  {
    return false; // did not find node in map
  }
  /*
  d->listResectionSurfaces->setCurrentItem(it.value());
  d->listResectionSurfaces->selectionModel()->select(d->listResectionSurfaces->selectionModel()->currentIndex(), QItemSelectionModel::Select);
  */

  std::cout << "SurfacesWidget - select item: " << it.value()->text().toStdString() << std::endl;
  return true;
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningSurfacesWidget::GetCurrentResectionID()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);
  /*
  if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
  {
    std::cout << "SurfacesWidget - current resection, map length: " << this->resectionIDtoItemMap.size() << '\n';

    QList<QString> ql = this->resectionIDtoItemMap.keys(d->listResectionSurfaces->currentItem());
    if (ql.size() == 0)
    {
      return NULL; // did not find node in map
    }
    QString resectionID = ql[0];
    return resectionID;
  }
  else
  {
    return NULL;
  }*/
}

QList<QString> qSlicerResectionPlanningSurfacesWidget::GetResections()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QList<QString> resectionList;

  /*
  std::cout << "SurfacesWidget - length resections: " << d->listResectionSurfaces->count() << std::endl;
  // loop through the list
  for(int row = 0; row < d->listResectionSurfaces->count(); row++)
  {
     QListWidgetItem *item = d->listResectionSurfaces->item(row);
     // find the id of the resection
     QList<QString> ql = this->resectionIDtoItemMap.keys(item);
     if (ql.size() > 0)
     {
       QString tumorID = ql[0]; // found in map
       resectionList.push_back(tumorID);
     }
  }*/

  return resectionList;
}

//-----------------------------------------------------------------------------
// Add / remove from resection list
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::AddToResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(nodeName);
  item->setToolTip(nodeID);

  //d->listResectionSurfaces->addItem(item);

  // keep in map(s)
  this->resectionIDtoItemMap.insert(nodeID, item);

  std::cout << "SurfacesWidget - Surface added to scene, and to list: " << nodeName.toStdString() << std::endl;
}

void qSlicerResectionPlanningSurfacesWidget
::RemoveFromResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QMap<QString, QListWidgetItem*>::iterator it;
  it = this->resectionIDtoItemMap.find(nodeID);
  if (it == this->resectionIDtoItemMap.end())
  {
    return; // cannot find in map
  }
  // erase from map(s)
  this->resectionIDtoItemMap.remove(nodeID);

  /*
  int row = d->listResectionSurfaces->row(*it);
  d->listResectionSurfaces->takeItem(row);
  */

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
  /*
  if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find which one is currently highlighted
    QList<QString> ql = this->resectionIDtoItemMap.keys(d->listResectionSurfaces->currentItem());
    if (ql.size() == 0)
    {
      return; // did not find node in map
    }
    QString resectionID = ql[0];
    QString resectionName = d->listResectionSurfaces->currentItem()->text();

    // EMIT SIGNAL
    emit RemoveSurfaceButtonClicked(resectionID, resectionName);
  }
  else
  {
    std::cout << "SurfacesWidget - No resection selected for deletion" << std::endl;
  }
  */
}

//-----------------------------------------------------------------------------
// Callback for selected resection changing
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnCurrentResectionSurfaceChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
   if(current->text() != NULL)
   {
     std::cout << "SurfacesWidget - Selected surface changed: " << current->text().toStdString() << std::endl;
     QString resectionID = current->text();
     emit CurrentResectionSurfaceChanged(resectionID);
   }
}
