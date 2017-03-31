/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningModuleWidget.cxx

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

// This module includes
#include "qSlicerResectionPlanningModuleWidget.h"
#include "ui_qSlicerResectionPlanningModuleWidget.h"
#include "vtkSlicerResectionPlanningLogic.h"
#include "qSlicerResectionPlanningModule.h"

// MRML includes
#include <vtkMRMLNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>

// VTK includes
#include <vtkEventQtSlotConnect.h>

// Qt includes
#include <QDebug>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerResectionPlanningModuleWidgetPrivate:
  public Ui_qSlicerResectionPlanningModuleWidget
{
public:
  qSlicerResectionPlanningModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModuleWidgetPrivate::
qSlicerResectionPlanningModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerResectionPlanningModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerResectionPlanningModuleWidget::
qSlicerResectionPlanningModuleWidget(QWidget* parent)
  : Superclass( parent )
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

  this->Module = dynamic_cast<qSlicerResectionPlanningModule*>(this->module());
  this->ModuleLogic = vtkSlicerResectionPlanningLogic::SafeDownCast(this->Module->logic());

  this->activeResectionID = "";

  // hiding these widgets for now, since not using their functionality yet
  d->TumorsWidget->hide();
  d->VolumesWidget->hide();

  // connections to lower level widgets (TUMORS)
  QObject::connect(d->TumorsWidget,
                   SIGNAL(AddTumorToResectionButtonClicked(QString&)),
                   this,
                   SLOT(OnAddTumorToResection(QString&)));

  QObject::connect(d->TumorsWidget,
                   SIGNAL(RemoveTumorToResectionButtonClicked(QString&)),
                   this,
                   SLOT(OnRemoveTumorToResection(QString&)));

  // connections to lower level widgets (SURFACES)
  QObject::connect(d->SurfacesWidget,
                   SIGNAL(AddSurfaceButtonClicked()),
                   this,
                   SLOT(OnAddResection()));

  QObject::connect(d->SurfacesWidget,
                   SIGNAL(RemoveSurface(QString&)),
                   this,
                   SLOT(OnRemoveResection(QString&)));

  QObject::connect(d->SurfacesWidget,
                   SIGNAL(CurrentResectionSurfaceChanged(QString&)),
                   this,
                   SLOT(OnActiveResectionChanged(QString&)));

  // connections to lower level widgets (VOLUMES)
  QObject::connect(d->VolumesWidget,
                   SIGNAL(VolumesButtonClicked()),
                   this,
                   SLOT(OnVolumesButtonClicked()));

  // connections to the logic (TUMORS)
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorModelAdded,
                       this, SLOT(OnTumorAdded(vtkObject*,
                                               unsigned long,
                                               void*,void*)));

  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::TumorModelRemoved,
                       this, SLOT(OnTumorRemoved(vtkObject*,
                                                 unsigned long,
                                                 void*,void*)));

  // connections to the logic (SURFACES)
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                       d->SurfacesWidget,
                       SLOT(OnResectionAdded(vtkObject*, unsigned long,
                                             void*,void*)));
  Connections->Connect(this->ModuleLogic,
                       vtkSlicerResectionPlanningLogic::ResectionNodeRemoved,
                       d->SurfacesWidget,
                       SLOT(OnResectionRemoved(vtkObject*,
                                               unsigned long,
                                               void*,void*)));

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::nodeSelectionChanged(vtkMRMLNode* node)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

}
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  this->Superclass::setMRMLScene(scene);

  std::cout << "Widget - Set MRML scene called " << std::endl;
}

std::list<std::string> qSlicerResectionPlanningModuleWidget::GetTumorsAssociatedWithResection(std::string &resectionID)
{
  std::list<std::string> tumorList;

  std::cout << "Widget - length of resection-tumor map " << resectionToTumorMap.size() << '\n';

  typedef std::multimap<std::string, std::string>::iterator iterator;
  std::pair<iterator, iterator> iterpair = this->resectionToTumorMap.equal_range(resectionID);

  iterator it = iterpair.first;
  for (; it != iterpair.second; ++it) {
  std::string tumor = it->second;
  tumorList.push_back(tumor);
  }
  return tumorList;
}

//-----------------------------------------------------------------------------
// functions connected to TUMORS WIDGET signals
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::OnAddTumorToResection(QString &tumorID)
{
  // Q_D(qSlicerResectionPlanningModuleWidget);

  // std::cout << "Widget - OnAddTumorToResection " << tumorID.toStdString() << '\n';

  // if(this->activeResectionID.length() < 1)
  //   {
  //   // get the active resection from the surfaces widget
  //   QString resectionID = d->SurfacesWidget->GetCurrentResectionID();
  //   if(resectionID == NULL)
  //     {
  //     std::cout << "Widget - cannot associate resection to tumor " << tumorID.toStdString() << '\n';
  //     return;
  //     }
  //   else
  //     {
  //     this->activeResectionID = resectionID.toStdString();
  //     }
  //   }
  // std::cout << "Widget - Resection: " << this->activeResectionID << ", associated to tumor: " << tumorID.toStdString() << '\n';

  // this->resectionToTumorMap.insert(std::pair<std::string, std::string>(this->activeResectionID, tumorID.toStdString()));

  // // TODO: Communicate up to LOGIC
}

void qSlicerResectionPlanningModuleWidget::OnRemoveTumorToResection(QString &tumorID)
{
  // Q_D(qSlicerResectionPlanningModuleWidget);

  // // get the active resection from the surfaces widget
  // QString resectionID = d->SurfacesWidget->GetCurrentResectionID();
  // if(resectionID == NULL)
  //   {
  //   return;
  //   }

  // typedef std::multimap<std::string, std::string>::iterator iterator;
  // std::pair<iterator, iterator> iterpair = this->resectionToTumorMap.equal_range(resectionID.toStdString());

  // iterator it = iterpair.first;
  // for (; it != iterpair.second; ++it) {
  // if (it->second == tumorID.toStdString()) {
  // std::cout << "Widget - Resection: " << it->first << ", removed association to tumor: " << it->second << '\n';
  // this->resectionToTumorMap.erase(it);
  // break;
  // }
  // }

  // TODO: Communicate up to LOGIC
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::OnAddResection()
{
  if (!this->ModuleLogic)
    {
    std::cerr << "Error: No module logic." << std::endl;
    return;
    }

  this->ModuleLogic->AddResectionSurface();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::
OnRemoveResection(QString &resectionID)
{
  std::cout << "Widget - Resection: " << resectionID.toStdString() << " removed " << '\n';

  if (!this->ModuleLogic)
    {
    std::cerr << "Error: No module logic." << std::endl;
    return;
    }

  this->ModuleLogic->RemoveResection(resectionID.toStdString().c_str());
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::
OnActiveResectionChanged(QString &resectionID)
{
  this->activeResectionID = resectionID.toStdString();
  std::cout << "Widget - Resection: " << this->activeResectionID << " removed " << '\n';

  // TODO: Communicate up to LOGIC
}

//-----------------------------------------------------------------------------
// functions connected to VOLUMES WIDGET signals
//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::OnVolumesButtonClicked()
{
  std::cout << "Widget - Volumes button clicked " << std::endl;
}

//-----------------------------------------------------------------------------
// functions connected to LOGIC events
//-----------------------------------------------------------------------------
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

  std::cout << "Widget - add tumor to list " << pair->second << std::endl;

  // add tumor node to list in TUMORS WIDGET
  d->TumorsWidget->AddToTumorList(Qid, Qname);
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

  std::cout << "Widget - remove tumor from list " << pair->second << std::endl;

  // remove tumor node to list in TUMORS WIDGET
  d->TumorsWidget->RemoveFromTumorList(Qid, Qname);
}
