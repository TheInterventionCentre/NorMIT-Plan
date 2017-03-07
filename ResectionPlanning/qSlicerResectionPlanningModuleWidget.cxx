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

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLDisplayNode.h>

#include <vtkEventQtSlotConnect.h>
#include "vtkSlicerResectionPlanningLogic.h"
#include "qSlicerResectionPlanningModule.h"

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
  if(this->Connections)
  {
    this->Connections->Delete();
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::setup()
{
  Q_D(qSlicerResectionPlanningModuleWidget);
  d->setupUi(this);

  this->Module = dynamic_cast<qSlicerResectionPlanningModule*>(this->module());
  this->ModuleLogic = vtkSlicerResectionPlanningLogic::SafeDownCast(this->Module->logic());

  this->Connections = vtkEventQtSlotConnect::New();

  // connect events to node selection dropdown
  QObject::connect(d->ActiveParenchymaModelNodeSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(nodeSelectionChanged(vtkMRMLNode*)));
  //QObject::connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->ActiveParenchymaModelNodeSelector, SLOT(setMRMLScene(vtkMRMLScene*)));

  // connections to lower level widgets (surfaces, volumes)
  QObject::connect(d->SurfacesWidget,
                     SIGNAL(AddTumorButtonClicked(QPair<QString&,QString&>&)),
                     this,
                     SLOT(OnAddTumorFromWidget(QPair<QString&,QString&>&)));
  QObject::connect(d->SurfacesWidget,
                     SIGNAL(RemoveTumorButtonClicked(QPair<QString&,QString&>&)),
                     this,
                     SLOT(OnAddTumorFromWidget(QPair<QString&,QString&>&)));

  // connections to the logic
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorNodeAdded,
                       this, SLOT(OnTumorAdded(vtkObject*,unsigned long,void*,void*)));
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorNodeRemoved,
                       this, SLOT(OnTumorRemoved(vtkObject*,unsigned long,void*,void*)));

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::nodeSelectionChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  vtkMRMLModelNode *activeParenchymaModel = vtkMRMLModelNode::SafeDownCast(d->ActiveParenchymaModelNodeSelector->currentNode() );
  if(activeParenchymaModel != NULL)
  {
    std::cout << "Widget - Parenchyma Model node selection changed " << std::endl;
  }
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerResectionPlanningModuleWidget);
  this->Superclass::setMRMLScene(scene);

  std::cout << "Widget - Set MRML scene called " << std::endl;
}

void qSlicerResectionPlanningModuleWidget::OnAddTumorFromWidget(QPair<QString&,QString&> &myPair)
{
   this->ModuleLogic->SetTumorToResectionAssociation(myPair.first.toStdString(), myPair.second.toStdString());
}

void qSlicerResectionPlanningModuleWidget::OnRemoveTumorFromWidget(QPair<QString&,QString&> &myPair)
{
   this->ModuleLogic->RemoveTumorToResectionAssociation(myPair.first.toStdString(), myPair.second.toStdString());
}

void qSlicerResectionPlanningModuleWidget
::OnTumorAdded(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  std::pair<vtkMRMLModelNode*, QString> *pair =
    reinterpret_cast< std::pair<vtkMRMLModelNode*,
                                QString> *>(callData);

  // add tumor node to list
  d->SurfacesWidget->AddToTumorList(pair->second);
}

void qSlicerResectionPlanningModuleWidget
::OnTumorRemoved(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  std::pair<vtkMRMLModelNode*, QString> *pair =
    reinterpret_cast< std::pair<vtkMRMLModelNode*,
                                QString> *>(callData);

  // add tumor node to list
  d->SurfacesWidget->RemoveFromTumorList(pair->second);
}



