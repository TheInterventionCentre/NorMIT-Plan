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
#include <QtPlugin>

// ResectionPlanning Logic includes
#include <vtkSlicerResectionPlanningLogic.h>

// ResectionPlanning includes
#include "qSlicerResectionPlanningModule.h"
#include "qSlicerResectionPlanningModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerResectionPlanningModule, qSlicerResectionPlanningModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerResectionPlanningModulePrivate
{
public:
  qSlicerResectionPlanningModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModulePrivate::qSlicerResectionPlanningModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModule methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModule::qSlicerResectionPlanningModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerResectionPlanningModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModule::~qSlicerResectionPlanningModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerResectionPlanningModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerResectionPlanningModule::icon() const
{
  return QIcon(":/Icons/ResectionPlanning.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerResectionPlanningModule::categories() const
{
  return QStringList() << "NorMIT-Plan";
}

//-----------------------------------------------------------------------------
QStringList qSlicerResectionPlanningModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerResectionPlanningModule
::createWidgetRepresentation()
{
  return new qSlicerResectionPlanningModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerResectionPlanningModule::createLogic()
{
  return vtkSlicerResectionPlanningLogic::New();
}
