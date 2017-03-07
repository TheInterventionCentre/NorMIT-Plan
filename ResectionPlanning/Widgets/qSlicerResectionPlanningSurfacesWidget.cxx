/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

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

    QPair<QString&,QString&> myPair(lastTumorAdded, resectionName);

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

    QPair<QString&,QString&> myPair(lastTumorAdded, resectionName);

    std::cout << "SurfacesWidget - Remove Tumor button clicked: " << lastTumorAdded.toStdString() << std::endl;

    emit RemoveTumorButtonClicked( myPair );
  }
  else
  {
    std::cout << "SurfacesWidget - On Remove Tumor: no tumor to remove " << std::endl;
  }
}

