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
#include "qSlicerResectionPlanningFooBarWidget.h"
#include "ui_qSlicerResectionPlanningFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningFooBarWidgetPrivate
  : public Ui_qSlicerResectionPlanningFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningFooBarWidget);
protected:
  qSlicerResectionPlanningFooBarWidget* const q_ptr;

public:
  qSlicerResectionPlanningFooBarWidgetPrivate(
    qSlicerResectionPlanningFooBarWidget& object);
  virtual void setupUi(qSlicerResectionPlanningFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningFooBarWidgetPrivate
::qSlicerResectionPlanningFooBarWidgetPrivate(
  qSlicerResectionPlanningFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningFooBarWidgetPrivate
::setupUi(qSlicerResectionPlanningFooBarWidget* widget)
{
  this->Ui_qSlicerResectionPlanningFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningFooBarWidget
::qSlicerResectionPlanningFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningFooBarWidget
::~qSlicerResectionPlanningFooBarWidget()
{
}
