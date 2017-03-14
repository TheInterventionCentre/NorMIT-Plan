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

#include <iostream>
#include <string>

#include <QListWidget>
#include <QLabel>
#include <QListWidgetItem>
#include <QString>

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

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningFooBarWidget methods

//-----------------------------------------------------------------------------
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

  // connect signals & slots for buttons to add & remove tumors from a resection surface
  QObject::connect(d->AddTumorButton, SIGNAL(clicked()),
                   this, SLOT(OnAddTumorButtonClicked()));
  QObject::connect(d->RemoveTumorButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveTumorButtonClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::~qSlicerResectionPlanningSurfacesWidget()
{

}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::AddToTumorList(QPair<QString&,QString&> &myPair)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QString nodeID = myPair.first;
  QString nodeName = myPair.second;

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(nodeName);
  item->setToolTip(nodeID);

  d->listAvailableTumors->addItem(item);

  // keep in map(s)
  this->tumorIDtoItemMap.insert(std::pair<QString, QListWidgetItem*>(nodeID, item));

  std::cout << "SurfacesWidget - Tumor added to scene, and to list: " << nodeName.toStdString() << std::endl;
}

void qSlicerResectionPlanningSurfacesWidget
::RemoveFromTumorList(QPair<QString&,QString&> &myPair)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QString nodeID = myPair.first;
  QString nodeName = myPair.second;

  QListWidgetItem *item = this->tumorIDtoItemMap.find(nodeID)->second;

  d->listAvailableTumors->removeItemWidget(item);

  // erase from map(s)
  this->tumorIDtoItemMap.erase(nodeID);

  std::cout << "SurfacesWidget - Tumor removed from scene, and from list: " << nodeName.toStdString() << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnAddSurfaceButtonClicked()
{
  std::cout << "SurfacesWidget - On Add Surface" << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnRemoveSurfaceButtonClicked()
{
  std::cout << "SurfacesWidget - On Remove Surface" << std::endl;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnAddTumorButtonClicked()
{
  // figure out which tumor is highlighted and add to the list related to the current resection node
  Q_D(qSlicerResectionPlanningSurfacesWidget);

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

    QString lastTumorAdded = item->text();
    QString resectionName = "no_resection";
    if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
    {
      resectionName = d->listResectionSurfaces->currentItem()->text();
    }
    QPair<QString&,QString&> myPair(resectionName, lastTumorAdded);

    std::cout << "SurfacesWidget - Add Tumor to resection: " << lastTumorAdded.toStdString() << std::endl;

    emit AddTumorButtonClicked( myPair );
  }
  else
  {
    std::cout << "SurfacesWidget - Add Tumor to resection: no tumor selected" << std::endl;
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnRemoveTumorButtonClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if(d->listResectionTumors->selectedItems().size() > 0) // check not null (something is selected)
  {
    // remove tumor from list
    QListWidgetItem *item = d->listResectionTumors->currentItem();
    QString itemID = item->toolTip();
    QString tumorRemoved = item->text();
    delete d->listResectionTumors->item(d->listResectionTumors->currentRow());

    // make item selectable again (in base list)
    // find the right tumor to enable
    QListWidgetItem *item2 = this->tumorIDtoItemMap.find(itemID)->second;

    item2->setFlags(item2->flags() | Qt::ItemIsEnabled);
    d->listAvailableTumors->setCurrentItem(item2);
    d->listAvailableTumors->selectionModel()->select(d->listAvailableTumors->selectionModel()->currentIndex(), QItemSelectionModel::Select);

    QString resectionName = "no_resection";
    if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
    {
      resectionName = d->listResectionSurfaces->currentItem()->text();
    }
    QPair<QString&,QString&> myPair(resectionName, tumorRemoved);

    std::cout << "SurfacesWidget - Remove Tumor from resection: " << tumorRemoved.toStdString() << std::endl;

    emit RemoveTumorButtonClicked( myPair );
  }
  else
  {
    std::cout << "SurfacesWidget - Remove Tumor from resection: no tumor to remove " << std::endl;
  }
}

