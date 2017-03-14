/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningModuleWidget.cxx
Copyright (c) 2017 The Intervention Centre, Oslo University Hospital
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

// Qt includes
#include <QDebug>

#include <vtkEventQtSlotConnect.h>

#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>

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

  QObject::connect(d->VolumesWidget,
                     SIGNAL(VolumesButtonClicked(QString&)),
                     this,
                     SLOT(OnVolumesButtonClicked(QString&)));


  // connections to the logic
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorModelAdded,
                       this, SLOT(OnTumorAdded(vtkObject*,unsigned long,void*,void*)));
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorModelRemoved,
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

void qSlicerResectionPlanningModuleWidget::OnVolumesButtonClicked(QString& test)
{
  std::cout << "Widget - Volumes button clicked " << std::endl;
}

void qSlicerResectionPlanningModuleWidget
::OnTumorAdded(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  std::pair<std::string, std::string> *pair =
    reinterpret_cast< std::pair<std::string,std::string> *>(callData);

  QString Qid = QString::fromStdString(pair->first);
  QString Qname = QString::fromStdString(pair->second);

  QPair<QString&,QString&> myPair(Qid, Qname);

  // add tumor node to list
  d->SurfacesWidget->AddToTumorList(myPair);
}

void qSlicerResectionPlanningModuleWidget
::OnTumorRemoved(vtkObject* vtkNotUsed(object),
                   unsigned long vtkNotUsed(event),
                   void * vtkNotUsed(clientData),
                   void *callData)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  std::pair<std::string, std::string> *pair =
    reinterpret_cast< std::pair<std::string,std::string> *>(callData);

  QString Qid = QString::fromStdString(pair->first);
  QString Qname = QString::fromStdString(pair->second);

  QPair<QString&,QString&> myPair(Qid, Qname);

  // remove tumor node to list
  d->SurfacesWidget->RemoveFromTumorList(myPair);
}
