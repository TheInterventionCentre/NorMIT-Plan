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
#include "qSlicerResectionPlanningVolumesWidget.h"
#include "ui_qSlicerResectionPlanningVolumesWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningVolumesWidgetPrivate
  : public Ui_qSlicerResectionPlanningVolumesWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningVolumesWidget);
protected:
  qSlicerResectionPlanningVolumesWidget* const q_ptr;

public:
  qSlicerResectionPlanningVolumesWidgetPrivate(
    qSlicerResectionPlanningVolumesWidget& object);
  virtual void setupUi(qSlicerResectionPlanningVolumesWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidgetPrivate
::qSlicerResectionPlanningVolumesWidgetPrivate(
  qSlicerResectionPlanningVolumesWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningVolumesWidgetPrivate
::setupUi(qSlicerResectionPlanningVolumesWidget* widget)
{
  this->Ui_qSlicerResectionPlanningVolumesWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidget
::qSlicerResectionPlanningVolumesWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningVolumesWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningVolumesWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningVolumesWidget
::~qSlicerResectionPlanningVolumesWidget()
{
}
