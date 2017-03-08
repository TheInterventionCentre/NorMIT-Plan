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

#include "vtkMRMLResectionSurfaceNode.h"

#include <iostream>
#include <string>

#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>

#include <QListWidget>
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

  QObject::connect(d->AddSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnAddSurfaceButtonClicked()));
  QObject::connect(d->RemoveSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(OnRemoveSurfaceButtonClicked()));

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
::AddToTumorList(QString nodeName)
{
  std::cout << "SurfacesWidget - Add to tumor list: " << nodeName.toStdString() << std::endl;

  Q_D(qSlicerResectionPlanningSurfacesWidget);

  new QListWidgetItem(nodeName, d->listTumorsToAdd);

}

void qSlicerResectionPlanningSurfacesWidget
::RemoveFromTumorList(QString nodeName)
{
  std::cout << "SurfacesWidget - Remove from tumor list:" << nodeName.toStdString() << std::endl;

  Q_D(qSlicerResectionPlanningSurfacesWidget);

  delete d->listTumorsToAdd->item(d->listTumorsToAdd->currentRow());

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

  if(d->listTumorsToAdd->selectedItems().size() > 0) // check not null (something is selected)
  {
    // add tumor to list
    new QListWidgetItem(d->listTumorsToAdd->currentItem()->text(), d->listTumorsAdded);

    QString lastTumorAdded = d->listTumorsToAdd->currentItem()->text();
    QString resectionName = "no_resection";
    if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
    {
      resectionName = d->listResectionSurfaces->currentItem()->text();
    }

    QPair<QString&,QString&> myPair(resectionName, lastTumorAdded);

    std::cout << "SurfacesWidget - Add Tumor button clicked: " << lastTumorAdded.toStdString() << std::endl;

    emit AddTumorButtonClicked( myPair );
  }
  else
  {
    std::cout << "SurfacesWidget - Add Tumor button clicked: no tumor selected" << std::endl;
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::OnRemoveTumorButtonClicked()
{
  // figure out which tumor is highlighted and remove from the list related to the current resection node
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if(d->listTumorsAdded->selectedItems().size() > 0) // check not null (something is selected)
  {
    // remove tumor from list
    delete d->listTumorsAdded->item(d->listTumorsAdded->currentRow());

    QString lastTumorAdded = d->listTumorsToAdd->currentItem()->text();
    QString resectionName = "no_resection";
    if(d->listResectionSurfaces->selectedItems().size() > 0) // check not null (something is selected)
    {
      resectionName = d->listResectionSurfaces->currentItem()->text();
    }

    QPair<QString&,QString&> myPair(resectionName, lastTumorAdded);

    std::cout << "SurfacesWidget - Remove Tumor button clicked: " << lastTumorAdded.toStdString() << std::endl;

    emit RemoveTumorButtonClicked( myPair );
  }
  else
  {
    std::cout << "SurfacesWidget - On Remove Tumor: no tumor to remove " << std::endl;
  }
}

