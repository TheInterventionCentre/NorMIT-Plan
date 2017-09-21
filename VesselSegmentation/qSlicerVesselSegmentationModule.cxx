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

//MRMLDisplayableManager includes
#include <vtkMRMLSliceViewDisplayableManagerFactory.h>

// VesselSegmentation Logic includes
#include <vtkSlicerVesselSegmentationLogic.h>

// VesselSegmentation includes
#include "qSlicerVesselSegmentationModule.h"
#include "qSlicerVesselSegmentationModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerVesselSegmentationModule, qSlicerVesselSegmentationModule);

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
    
  vtkMRMLSliceViewDisplayableManagerFactory *factory =
  vtkMRMLSliceViewDisplayableManagerFactory::GetInstance();
    
  //Register displayable managers
  factory->RegisterDisplayableManager("vtkMRMLVesselSegmentationDisplayableManager");
    
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
