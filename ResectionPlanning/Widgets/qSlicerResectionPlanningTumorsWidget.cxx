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
                   this, SLOT(OnAddTumorButtonClicked()));
  QObject::connect(d->RemoveTumorButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveTumorButtonClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningTumorsWidget
::~qSlicerResectionPlanningTumorsWidget()
{

}
/*
void vtkSlicerResectionPlanningLogic
::SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::cout << "'Resection: " << rsNodeName << ", associated to tumor: " << tumorNodeName << '\n';
  this->resectionToTumorMap.insert(std::pair<std::string, std::string>(rsNodeName, tumorNodeName));
}

void vtkSlicerResectionPlanningLogic
::RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::multimap<std::string, std::string>::iterator it;

  for (it=this->resectionToTumorMap.begin(); it!=this->resectionToTumorMap.end(); ++it) {
    if(((*it).first == rsNodeName) && ((*it).second == tumorNodeName))
    {
      std::cout << "'Resection: " << (*it).first << ", removed association to tumor: " << (*it).second << '\n';
      this->resectionToTumorMap.erase(it);
    }
  }
}

std::list<std::string> vtkSlicerResectionPlanningLogic::GetTumorsAssociatedWithResection(std::string resectionName)
{
  std::list<std::string> tumorList;
  std::multimap<std::string, std::string>::iterator it;

  for (it=this->resectionToTumorMap.begin(); it!=this->resectionToTumorMap.end(); ++it) {
    if((*it).first == resectionName)
    {
      tumorList.push_back((*it).second);
    }
  }
  return tumorList;
}*/

QString qSlicerResectionPlanningTumorsWidget::SelectItemInAvailableTumors(QString &id)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  // find the right tumor to select
  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->tumorIDtoItemMap.find(id);
  if (it == this->tumorIDtoItemMap.end())
  {
    std::cout << "TumorsWidget - return" << std::endl;
    QString test("nothing");
    return test;
  }
  std::cout << "TumorsWidget - don't return" << std::endl;
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
  it = this->tumorIDtoItemMap.find(id);
  if (it == this->tumorIDtoItemMap.end())
  {
    std::cout << "TumorsWidget - return" << std::endl;
    QString test("nothing");
    return test;
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

  d->listAvailableTumors->addItem(item);

  // keep in map(s)
  this->tumorIDtoItemMap.insert(std::pair<QString, QListWidgetItem*>(nodeID, item));

  std::cout << "TumorsWidget - Tumor added to scene, and to list: " << nodeName.toStdString() << std::endl;
}

void qSlicerResectionPlanningTumorsWidget
::RemoveFromTumorList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningTumorsWidget);

  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->tumorIDtoItemMap.find(nodeID);
  if (it == this->tumorIDtoItemMap.end())
  {
    return;
  }

  d->listAvailableTumors->removeItemWidget(it->second);

  // erase from map(s)
  this->tumorIDtoItemMap.erase(nodeID);

  std::cout << "TumorsWidget - Tumor removed from scene, and from list: " << nodeName.toStdString() << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::OnAddTumorButtonClicked()
{
  // figure out which tumor is highlighted and add to the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  if(d->listAvailableTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // add tumor to list
    QListWidgetItem *item = new QListWidgetItem();
    item->setText(d->listAvailableTumors->currentItem()->text());
    item->setToolTip(d->listAvailableTumors->currentItem()->toolTip());
    d->listResectionTumors->addItem(item);

    // make item unselectable (in base list)
    d->listAvailableTumors->currentItem()->setFlags(d->listAvailableTumors->currentItem()->flags() & ~Qt::ItemIsEnabled);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Deselect);

    QString tumorAdded = item->text();
    QString resectionName = "no_resection";
    // TODO: get current resection from resectionWidget

    std::cout << "TumorsWidget - Add Tumor to resection: " << tumorAdded.toStdString() << std::endl;

    emit AddTumorButtonClicked(resectionName, tumorAdded);
  }
  else
  {
    std::cout << "TumorsWidget - Add Tumor to resection: no tumor selected" << std::endl;
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningTumorsWidget
::OnRemoveTumorButtonClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningTumorsWidget);

  if(d->listResectionTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // remove tumor from list
    QListWidgetItem *item = d->listResectionTumors->currentItem();
    QString itemID = item->toolTip();
    QString tumorRemoved = item->text();
    delete d->listResectionTumors->item(d->listResectionTumors->currentRow());

    // make item selectable again (in base list)
    // find the right tumor to enable
    std::map<QString, QListWidgetItem*>::iterator it;
    it = this->tumorIDtoItemMap.find(itemID);
    if (it == this->tumorIDtoItemMap.end())
    {
      return;
    }

    it->second->setFlags(it->second->flags() | Qt::ItemIsEnabled);
    d->listAvailableTumors->setCurrentItem(it->second);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

    QString resectionName = "no_resection";
    // TODO: get current resection from resectionWidget

    std::cout << "TumorsWidget - Remove Tumor from resection: " << tumorRemoved.toStdString() << std::endl;

    emit RemoveTumorButtonClicked(resectionName, tumorRemoved);
  }
  else
  {
    std::cout << "TumorsWidget - Remove Tumor from resection: no tumor to remove " << std::endl;
  }
}

