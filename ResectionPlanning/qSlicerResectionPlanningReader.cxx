/*
 * qSlicerResectionPlanningReader.cxx
 *
 *  Created on: Mar 27, 2017
 *      Author: louise
 */


// Qt includes
#include <QFileInfo>

// SlicerQt includes
#include "qSlicerResectionPlanningReader.h"

// Logic includes
#include "vtkSlicerResectionPlanningLogic.h"

// MRML includes
#include "vtkMRMLResectionSurfaceNode.h"
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerResectionPlanningReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerResectionPlanningLogic> ResectionPlanningLogic;
};

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Models
qSlicerResectionPlanningReader::qSlicerResectionPlanningReader(vtkSlicerResectionPlanningLogic* _resectionPlanningLogic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerResectionPlanningReaderPrivate)
{
  this->setResectionPlanningLogic(_resectionPlanningLogic);
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningReader::~qSlicerResectionPlanningReader()
{
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningReader::setResectionPlanningLogic(vtkSlicerResectionPlanningLogic* newResectionPlanningLogic)
{
  Q_D(qSlicerResectionPlanningReader);
  d->ResectionPlanningLogic = newResectionPlanningLogic;
}

//-----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic* qSlicerResectionPlanningReader::resectionPlanningLogic()const
{
  Q_D(const qSlicerResectionPlanningReader);
  return d->ResectionPlanningLogic;
}

//-----------------------------------------------------------------------------
QString qSlicerResectionPlanningReader::description()const
{
  return "Resection";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerResectionPlanningReader::fileType()const
{
  return QString("ResectionFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerResectionPlanningReader::extensions()const
{
  return QStringList()
    << "Resection (*.vtk *.vtp *.stl *.ply *.obj)";
}

//-----------------------------------------------------------------------------
bool qSlicerResectionPlanningReader::load(const IOProperties& properties)
{
  Q_D(qSlicerResectionPlanningReader);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  if (d->ResectionPlanningLogic.GetPointer() == 0)
  {
    return false;
  }
  vtkMRMLResectionSurfaceNode* node =
    d->ResectionPlanningLogic->AddResectionSurface(fileName.toLatin1());
  if (!node)
  {
    return false;
  }
  this->setLoadedNodes( QStringList(QString(node->GetID())) );
  if (properties.contains("name"))
  {
    std::string uname = this->mrmlScene()->GetUniqueNameByString(
      properties["name"].toString().toLatin1());
    node->SetName(uname.c_str());
  }
  return true;
}
