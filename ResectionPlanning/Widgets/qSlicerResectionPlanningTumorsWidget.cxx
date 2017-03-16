/*=========================================================================
  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningTumorsWidget.cxx
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
#include "qSlicerResectionPlanningTumorsWidget.h"
#include "ui_qSlicerResectionPlanningTumorsWidget.h"

#include <iostream>
#include <string>

#include <QListWidget>
#include <QLabel>
#include <QListWidgetItem>
#include <QString>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningTumorsWidgetPrivate
  : public Ui_qSlicerResectionPlanningTumorsWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningTumorsWidget);
protected:
  qSlicerResectionPlanningTumorsWidget* const q_ptr;

public:
  qSlicerResectionPlanningTumorsWidgetPrivate(
    qSlicerResectionPlanningTumorsWidget& object);
  virtual void setupUi(qSlicerResectionPlanningTumorsWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningTumorsWidgetPrivate
::qSlicerResectionPlanningTumorsWidgetPrivate(
  qSlicerResectionPlanningTumorsWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidgetPrivate
::setupUi(qSlicerResectionPlanningTumorsWidget* widget)
{
  this->Ui_qSlicerResectionPlanningTumorsWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningTumorsWidget
::qSlicerResectionPlanningTumorsWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningTumorsWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningTumorsWidget);
  d->setupUi(this);

  // connect signals & slots for buttons to add & remove tumors from a resection surface
  QObject::connect(d->AddTumorButton, SIGNAL(clicked()),
                   this, SLOT(OnAddTumorToResectionButtonClicked()));
  QObject::connect(d->RemoveTumorButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveTumorToResectionButtonClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningTumorsWidget
::~qSlicerResectionPlanningTumorsWidget()
{

}

QString qSlicerResectionPlanningTumorsWidget::SelectItemInAvailableTumors(QString &id)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find the right tumor to select
  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->IDtoItemMap_availableTumors.find(id);
  if (it == this->IDtoItemMap_availableTumors.end())
  {
    QString test("nothing");
    return test; // did not find node in map
  }
  d->listAvailableTumors->setCurrentItem(it->second);
  d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

  std::cout << "TumorsWidget - selected item: " << it->second->text().toStdString() << std::endl;
  return it->second->text();
}

QString qSlicerResectionPlanningTumorsWidget::SelectItemInResectionTumors(QString &id)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find the right tumor to select
  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->IDtoItemMap_resectionTumors.find(id);
  if (it == this->IDtoItemMap_resectionTumors.end())
  {
    QString test("nothing");
    return test; // did not find node in map
  }
  d->listAvailableTumors->setCurrentItem(it->second);
  d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

  std::cout << "TumorsWidget - select item: " << it->second->text().toStdString() << std::endl;
  return it->second->text();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::AddToTumorList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(nodeName);
  item->setToolTip(nodeID);

  // add to widget
  d->listAvailableTumors->addItem(item);

  // keep in map
  this->IDtoItemMap_availableTumors.insert(std::pair<QString, QListWidgetItem*>(nodeID, item));
  this->itemToIDMap_availableTumors.insert(std::pair<QListWidgetItem*, QString>(item, nodeID));

  std::cout << "TumorsWidget - Tumor added to list: " << nodeID.toStdString() << std::endl;
}

void qSlicerResectionPlanningTumorsWidget
::RemoveFromTumorList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find item with right ID
  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->IDtoItemMap_availableTumors.find(nodeID);
  if (it == this->IDtoItemMap_availableTumors.end())
  {
    return; // cannot find in map
  }
  // erase from map
  this->IDtoItemMap_availableTumors.erase(nodeID);
  this->itemToIDMap_availableTumors.erase(it->second);

  // remove from widget
  d->listAvailableTumors->removeItemWidget(it->second);

  std::cout << "TumorsWidget - Tumor removed from list: " << nodeID.toStdString() << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::OnAddTumorToResectionButtonClicked()
{
  // figure out which tumor is highlighted and add to the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  if(d->listAvailableTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find ID of selected item
    std::map<QListWidgetItem*, QString>::iterator it;
    it = this->itemToIDMap_availableTumors.find(d->listAvailableTumors->currentItem());
    if (it == this->itemToIDMap_availableTumors.end())
    {
      return; // cannot find in map
    }
    QString tumorID = it->second;

    // add tumor to list
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(d->listAvailableTumors->currentItem()->text());
    item->setToolTip(tumorID);
    d->listResectionTumors->addItem(item);

    // keep in map
    this->IDtoItemMap_resectionTumors.insert(std::pair<QString, QListWidgetItem*>(tumorID, item));
    this->itemToIDMap_resectionTumors.insert(std::pair<QListWidgetItem*, QString>(item, tumorID));

    // make item unselectable (in base list)
    d->listAvailableTumors->currentItem()->setFlags(d->listAvailableTumors->currentItem()->flags() & ~Qt::ItemIsEnabled);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Deselect);

    QString resectionID = "no_resectionID";
    // TODO: get current resection from resectionWidget

    std::cout << "TumorsWidget - Add Tumor to resection: " << tumorID.toStdString() << std::endl;

    // EMIT SIGNAL
    emit AddTumorToResectionButtonClicked(resectionID, tumorID);
  }
  else
  {
    std::cout << "TumorsWidget - Add Tumor to resection: no tumor selected" << std::endl;
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::OnRemoveTumorToResectionButtonClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  if(d->listResectionTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find ID of selected item
    std::map<QListWidgetItem*, QString>::iterator it_rt;
    it_rt = this->itemToIDMap_resectionTumors.find(d->listResectionTumors->currentItem());
    if (it_rt == this->itemToIDMap_resectionTumors.end())
    {
      return; // cannot find in map
    }
    QString tumorID = it_rt->second;
    // remove tumor from list
    delete d->listResectionTumors->item(d->listResectionTumors->currentRow());
    // or d->listResectionTumors->removeItemWidget(it_rt->first);

    // erase from map
    this->IDtoItemMap_resectionTumors.erase(tumorID);
    this->itemToIDMap_resectionTumors.erase(it_rt->first);

    // find item with right ID
    std::map<QString, QListWidgetItem*>::iterator it_at;
    it_at = this->IDtoItemMap_availableTumors.find(tumorID);
    if (it_at == this->IDtoItemMap_availableTumors.end())
    {
      return; // cannot find in map
    }
    // make item selectable again (in base list)
    it_at->second->setFlags(it_at->second->flags() | Qt::ItemIsEnabled);
    d->listAvailableTumors->setCurrentItem(it_at->second);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

    QString resectionID = "no_resectionID";
    // TODO: get current resection from resectionWidget

    std::cout << "TumorsWidget - Remove Tumor from resection: " << tumorID.toStdString() << std::endl;

    // EMIT SIGNAL
    emit RemoveTumorToResectionButtonClicked(resectionID, tumorID);
  }
  else
  {
    std::cout << "TumorsWidget - Remove Tumor from resection: no tumor to remove " << std::endl;
  }
}

