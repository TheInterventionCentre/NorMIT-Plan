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
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::~qSlicerResectionPlanningSurfacesWidget()
{

}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::AddToResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  QListWidgetItem *item = new QListWidgetItem();
  item->setText(nodeName);
  item->setToolTip(nodeID);

  d->listResectionSurfaces->addItem(item);

  // keep in map(s)
  this->resectionIDtoItemMap.insert(std::pair<QString, QListWidgetItem*>(nodeID, item));
  this->itemToResectionIDMap.insert(std::pair<QListWidgetItem*, QString>(item, nodeID));

  std::cout << "SurfacesWidget - Surface added to scene, and to list: " << nodeName.toStdString() << std::endl;
}

void qSlicerResectionPlanningSurfacesWidget
::RemoveFromResectionList(QString &nodeID,QString &nodeName)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  std::map<QString, QListWidgetItem*>::iterator it;
  it = this->resectionIDtoItemMap.find(nodeID);
  if (it == this->resectionIDtoItemMap.end())
  {
    return; // cannot find in map
  }
  // erase from map(s)
  this->resectionIDtoItemMap.erase(nodeID);
  this->itemToResectionIDMap.erase(it->second);

  d->listResectionSurfaces->removeItemWidget(it->second);

  std::cout << "SurfacesWidget - Surface removed from scene, and from list: " << nodeName.toStdString() << std::endl;
}


//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnAddSurfaceButtonClicked()
{
  std::cout << "SurfacesWidget - On Add Surface" << std::endl;

  // EMIT SIGNAL
  emit AddSurfaceButtonClicked();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnRemoveSurfaceButtonClicked()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  std::cout << "SurfacesWidget - On Remove Surface" << std::endl;

  if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
  {
    // find which one is currently highlighted
    QListWidgetItem *item = d->listResectionSurfaces->currentItem();
    std::map<QListWidgetItem*, QString>::iterator it;
    it = this->itemToResectionIDMap.find(item);
    if (it == this->itemToResectionIDMap.end())
    {
      return; // cannot find in map
    }
    QString resectionName = it->first->text();

    // EMIT SIGNAL
    emit RemoveSurfaceButtonClicked(it->second, resectionName);
  }
  else
  {
    std::cout << "SurfacesWidget - No resection selected for deletion" << std::endl;
  }
}
