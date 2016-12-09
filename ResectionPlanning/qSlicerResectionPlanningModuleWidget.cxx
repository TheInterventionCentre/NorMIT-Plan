/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerResectionPlanningModuleWidget.h"
#include "ui_qSlicerResectionPlanningModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerResectionPlanningModuleWidgetPrivate: public Ui_qSlicerResectionPlanningModuleWidget
{
public:
  qSlicerResectionPlanningModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModuleWidgetPrivate::qSlicerResectionPlanningModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModuleWidget::qSlicerResectionPlanningModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerResectionPlanningModuleWidgetPrivate )
{
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModuleWidget::~qSlicerResectionPlanningModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::setup()
{
  Q_D(qSlicerResectionPlanningModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();
}
