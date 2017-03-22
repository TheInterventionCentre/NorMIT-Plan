/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningModule.cxx

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

// Qt includes
#include <QtPlugin>

// ResectionPlanning Logic includes
#include <vtkSlicerResectionPlanningLogic.h>

// ResectionPlanning includes
#include "qSlicerResectionPlanningModule.h"
#include "qSlicerResectionPlanningModuleWidget.h"

// MRMLDisplayableManager includes
#include <vtkMRMLThreeDViewDisplayableManagerFactory.h>

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerResectionPlanningModule, qSlicerResectionPlanningModule);

//-----------------------------------------------------------------------------
/// \ingroup ResectionPlanning
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
  return "This module provides functionality to plan liver resection procedures.";
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningModule::acknowledgementText() const
{
  return "This work was partially funded by The Research Council of Norway (NorMIT project)";
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

  // Register displayable managers
  // 3D
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->
    RegisterDisplayableManager("vtkMRMLResectionDisplayableManager3D");
  vtkMRMLThreeDViewDisplayableManagerFactory::GetInstance()->
    RegisterDisplayableManager("vtkMRMLResectionInitializationDisplayableManager3D");
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

//-----------------------------------------------------------------------------
QStringList qSlicerResectionPlanningModule::associatedNodeTypes() const
{
  return QStringList()
    << "vtkMRMLResectionSurfaceNode"
    << "vtkMRMLResectionSurfaceDisplayNode";
}
