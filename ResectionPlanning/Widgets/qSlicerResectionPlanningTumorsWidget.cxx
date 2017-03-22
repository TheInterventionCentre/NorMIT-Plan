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

///-----------------------------------------------------------------------------
// Functions used currently for software testing
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget::ClickButtonAddTumor()
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  d->AddTumorButton->click();
}
void qSlicerResectionPlanningTumorsWidget::ClickButtonRemoveTumor()
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  d->RemoveTumorButton->click();
}

bool qSlicerResectionPlanningTumorsWidget::SelectItemInAvailableTumors(QString &id)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find the right tumor to select
  QMap<QString, QListWidgetItem*>::iterator qit;
  qit = this->IDtoItemMap_availTumors.find(id);
  if (qit == this->IDtoItemMap_availTumors.end())
  {
    return false; // did not find node in map
  }

  d->listAvailableTumors->setCurrentItem(qit.value());
  d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

  std::cout << "TumorsWidget - select item: " << qit.value()->text().toStdString() << std::endl;
  return true;
}

bool qSlicerResectionPlanningTumorsWidget::SelectItemInResectionTumors(QString &id)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find the right tumor to select
  QMap<QString, QListWidgetItem*>::iterator qit;
  qit = this->IDtoItemMap_resecTumors.find(id);
  if (qit == this->IDtoItemMap_resecTumors.end())
  {
    return false; // did not find node in map
  }

  d->listResectionTumors->setCurrentItem(qit.value());
  d->listResectionTumors->selectionModel()->select(d->listResectionTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

  std::cout << "TumorsWidget - select item: " << qit.value()->text().toStdString() << std::endl;
  return true;
}

QList<QString> qSlicerResectionPlanningTumorsWidget::GetAvailableTumors()
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  QList<QString> tumorList;

  std::cout << "TumorsWidget - length available tumors: " << d->listAvailableTumors->count() << std::endl;
  // loop through the list
  for(int row = 0; row < d->listAvailableTumors->count(); row++)
  {
     QListWidgetItem *item = d->listAvailableTumors->item(row);
     // find the id of the tumor
     QList<QString> ql = this->IDtoItemMap_availTumors.keys(item);
     if (ql.size() > 0)
     {
       QString tumorID = ql[0]; // found in map
       tumorList.push_back(tumorID);
     }
  }
  return tumorList;
}

QList<QString> qSlicerResectionPlanningTumorsWidget::GetResectionTumors()
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  QList<QString> tumorList;

  std::cout << "TumorsWidget - length resection tumors: " << d->listResectionTumors->count() << std::endl;
  // loop through the list
  for(int row = 0; row < d->listResectionTumors->count(); row++)
  {
     QListWidgetItem *item = d->listResectionTumors->item(row);
     // find the id of the tumor
     QList<QString> ql = this->IDtoItemMap_resecTumors.keys(item);
     if (ql.size() > 0)
     {
       QString tumorID = ql[0]; // found in map
       tumorList.push_back(tumorID);
     }
  }
  return tumorList;
}

//-----------------------------------------------------------------------------
// Available tumor list
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
  this->IDtoItemMap_availTumors.insert(nodeID, item);

  std::cout << "TumorsWidget - Tumor added to list: " << nodeID.toStdString() << std::endl;
}

void qSlicerResectionPlanningTumorsWidget
::RemoveFromTumorList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find item with right ID
  QMap<QString, QListWidgetItem*>::iterator qit;
  qit = this->IDtoItemMap_availTumors.find(nodeID);
  if (qit == this->IDtoItemMap_availTumors.end())
  {
    return; // did not find node in map
  }

  // erase from map
  int itemsRemoved = this->IDtoItemMap_availTumors.remove(nodeID);

  // remove from widget
  int row = d->listAvailableTumors->row(*qit);
  d->listAvailableTumors->takeItem(row);

  std::cout << "TumorsWidget - Tumor removed from list: " << nodeID.toStdString() << " # " << itemsRemoved << std::endl;
}

//-----------------------------------------------------------------------------
// Resection tumor list
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::OnAddTumorToResectionButtonClicked()
{
  // figure out which tumor is highlighted and add to the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  if(d->listAvailableTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find ID of selected item
    QList<QString> ql = this->IDtoItemMap_availTumors.keys(d->listAvailableTumors->currentItem());
    if (ql.size() == 0)
    {
      return; // did not find node in map
    }
    QString tumorID = ql[0];

    // add tumor to list
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(d->listAvailableTumors->currentItem()->text());
    item->setToolTip(tumorID);
    d->listResectionTumors->addItem(item);

    // keep in map
    this->IDtoItemMap_resecTumors.insert(tumorID, item);

    // make item unselectable (in base list)
    d->listAvailableTumors->currentItem()->setFlags(d->listAvailableTumors->currentItem()->flags() & ~Qt::ItemIsEnabled);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Deselect);

    std::cout << "TumorsWidget - Add Tumor to resection: " << tumorID.toStdString() << std::endl;

    // EMIT SIGNAL
    emit AddTumorToResectionButtonClicked(tumorID);
  }
  else
  {
    std::cout << "TumorsWidget - Add Tumor to resection: no tumor selected" << std::endl;
  }
}

void qSlicerResectionPlanningTumorsWidget
::OnRemoveTumorToResectionButtonClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  std::cout << "TumorsWidget - Remove Tumor from resection clicked" << std::endl;

  if(d->listResectionTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find ID of selected item
    QList<QString> ql = this->IDtoItemMap_resecTumors.keys(d->listResectionTumors->currentItem());
    if (ql.size() == 0)
    {
      return; // did not find node in map
    }
    QString tumorID = ql[0];

    // erase from map
    int itemsRemoved = this->IDtoItemMap_resecTumors.remove(tumorID);

    // remove tumor from list
    d->listResectionTumors->takeItem(d->listResectionTumors->currentRow());

    // find item with right ID
    // make item selectable again (in base list)
    QMap<QString, QListWidgetItem*>::iterator qit;
    qit = this->IDtoItemMap_availTumors.find(tumorID);
    if (qit == this->IDtoItemMap_availTumors.end())
    {
      return; // did not find node in map
    }
    qit.value()->setFlags(qit.value()->flags() | Qt::ItemIsEnabled);
    d->listAvailableTumors->setCurrentItem(qit.value());
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

    d->listResectionTumors->clearSelection();

    std::cout << "TumorsWidget - Remove Tumor from resection: " << tumorID.toStdString() << std::endl;

    // EMIT SIGNAL
    emit RemoveTumorToResectionButtonClicked(tumorID);
  }
  else
  {
    std::cout << "TumorsWidget - Remove Tumor from resection: no tumor to remove " << std::endl;
  }
}

