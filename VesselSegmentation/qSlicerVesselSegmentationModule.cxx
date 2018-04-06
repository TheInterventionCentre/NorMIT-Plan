/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerVesselSegmentationModule.cxx

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

//MRMLDisplayableManager includes
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>

// VesselSegmentation Logic includes
#include <vtkSlicerVesselSegmentationLogic.h>

// VesselSegmentation includes
#include "qSlicerVesselSegmentationModule.h"
#include "qSlicerVesselSegmentationModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerVesselSegmentationModule, qSlicerVesselSegmentationModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup VesselSegmentation
class qSlicerVesselSegmentationModulePrivate
{
public:
  qSlicerVesselSegmentationModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModulePrivate::qSlicerVesselSegmentationModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationModule methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModule::qSlicerVesselSegmentationModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVesselSegmentationModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationModule::~qSlicerVesselSegmentationModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVesselSegmentationModule::helpText() const
{
  return "This module provides functionality for segmenting vessels.";
}

//-----------------------------------------------------------------------------
QString qSlicerVesselSegmentationModule::acknowledgementText() const
{
  return "This work was partially funded by The Research Council of Norway (NorMIT project)";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselSegmentationModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Louise Oram & Rahul Kumar (Intervention centre, Oslo University Hospital)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVesselSegmentationModule::icon() const
{
  return QIcon(":/Icons/VesselSegmentation.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselSegmentationModule::categories() const
{
  return QStringList() << "NorMIT-Plan";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVesselSegmentationModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVesselSegmentationModule::setup()
{
  this->Superclass::setup();

  // Register displayable managers 2D
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance()->
    RegisterDisplayableManager("vtkMRMLVesselSegmentationDisplayableManager2D");

}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerVesselSegmentationModule
::createWidgetRepresentation()
{
  return new qSlicerVesselSegmentationModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVesselSegmentationModule::createLogic()
{
  return vtkSlicerVesselSegmentationLogic::New();
}
