/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningLRPModelReader.cxx

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
#include <QFileInfo>

// SlicerQt includes
#include "qSlicerResectionPlanningLRPModelReader.h"

// Logic includes
#include "vtkSlicerResectionPlanningLogic.h"

// MRML includes
#include "vtkMRMLLRPModelNode.h"
#include <vtkMRMLScene.h>

//------------------------------------------------------------------------------
class qSlicerResectionPlanningLRPModelReaderPrivate
{
public:
  vtkSmartPointer<vtkSlicerResectionPlanningLogic> ResectionPlanningLogic;
};

//------------------------------------------------------------------------------
qSlicerResectionPlanningLRPModelReader::
qSlicerResectionPlanningLRPModelReader(vtkSlicerResectionPlanningLogic* _resectionPlanningLogic,
                               QObject* _parent)
  : Superclass(_parent),
    d_ptr(new qSlicerResectionPlanningLRPModelReaderPrivate)
{
  this->setResectionPlanningLogic(_resectionPlanningLogic);
}

//------------------------------------------------------------------------------
qSlicerResectionPlanningLRPModelReader::~qSlicerResectionPlanningLRPModelReader()
{

}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningLRPModelReader::
setResectionPlanningLogic(vtkSlicerResectionPlanningLogic* newResectionPlanningLogic)
{
  Q_D(qSlicerResectionPlanningLRPModelReader);
  d->ResectionPlanningLogic = newResectionPlanningLogic;
}

//------------------------------------------------------------------------------
QString qSlicerResectionPlanningLRPModelReader::description() const
{
  return "LRP Model";
}

//------------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerResectionPlanningLRPModelReader::fileType() const
{
  return QString("LRPModelFile");
}

//_______________________________________________________________________________
QStringList qSlicerResectionPlanningLRPModelReader::extensions() const
{
  return QStringList() << "LRPModel (*.vtk *.vtp *.stl *.ply *.obj)";
}

//------------------------------------------------------------------------------
bool qSlicerResectionPlanningLRPModelReader::load(const IOProperties& properties)
{
  Q_D(qSlicerResectionPlanningLRPModelReader);
  Q_ASSERT(properties.contains("fileName"));

  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  if (d->ResectionPlanningLogic.GetPointer() == 0)
  {
    return false;
  }
  vtkMRMLLRPModelNode* node =
    d->ResectionPlanningLogic->AddLRPModel(fileName.toLatin1());

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
