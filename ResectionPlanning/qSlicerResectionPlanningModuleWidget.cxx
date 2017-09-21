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
  this->setMRMLScene(0);
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::setup()
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  d->setupUi(this);

  QObject::connect(d->SurfacesWidget,
                   SIGNAL(addSurfaceButtonClicked()),
                   this,
                   SLOT(onAddResection()));

  QObject::connect(d->SurfacesWidget,
                   SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode *)),
                   this,
                   SLOT(onRemoveResection(vtkMRMLResectionSurfaceNode*)));

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::enter()
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  this->Superclass::enter();

  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
                    this, SLOT(onNodeAddedEvent(vtkObject*, vtkObject*)));

  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeRemovedEvent,
                     this, SLOT(onNodeRemovedEvent(vtkObject*, vtkObject*)));
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::exit()
{
  this->Superclass::exit();
  this->qvtkDisconnectAll();
}

//-----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic *qSlicerResectionPlanningModuleWidget::
resectionPlanningLogic()
{
  if (this->logic() == NULL)
    {
    return NULL;
    }
  return vtkSlicerResectionPlanningLogic::SafeDownCast(this->logic());
}


//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::nodeSelectionChanged(vtkMRMLNode* vtkNotUsed(node))
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

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::onAddResection()
{
  if (!this->resectionPlanningLogic())
    {
    std::cerr << "Error: No module logic." << std::endl;
    return;
    }

  this->resectionPlanningLogic()->AddResectionSurface();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::
onRemoveResection(vtkMRMLResectionSurfaceNode *node)
{
  if (!node)
    {
    std::cerr << "No node passed." << std::endl;
    return;
    }

  if (!this->resectionPlanningLogic())
    {
    std::cerr << "Error: No module logic." << std::endl;
    return;
    }

  this->resectionPlanningLogic()->RemoveResection(node);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::
onNodeAddedEvent(vtkObject*, vtkObject *node)
{
  Q_D(qSlicerResectionPlanningModuleWidget);

  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (resectionNode)
    {
    d->SurfacesWidget->addResection(resectionNode);
    }
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningModuleWidget::
onNodeRemovedEvent(vtkObject* scene, vtkObject *node)
{
  Q_UNUSED(scene);
  Q_D(qSlicerResectionPlanningModuleWidget);

  if (!this->mrmlScene() || this->mrmlScene()->IsBatchProcessing())
    {
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(node);
  if (resectionNode)
    {
    d->SurfacesWidget->removeResection(resectionNode);
    }
}
