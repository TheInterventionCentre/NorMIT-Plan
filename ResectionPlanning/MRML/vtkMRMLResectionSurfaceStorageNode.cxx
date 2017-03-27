/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionSurfaceStorageNode.cxx

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

#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceStorageNode.h"
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkFieldData.h>
#include <vtksys/SystemTools.hxx>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

//----------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLResectionSurfaceStorageNode);

//----------------------------------------------------------------------------
vtkMRMLResectionSurfaceStorageNode::vtkMRMLResectionSurfaceStorageNode()
{
  //this->DefaultWriteFileExtension = "vtk";
}

vtkMRMLResectionSurfaceStorageNode::~vtkMRMLResectionSurfaceStorageNode()
{

}

void vtkMRMLResectionSurfaceStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLStorageNode::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
bool vtkMRMLResectionSurfaceStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLResectionSurfaceNode");
}

//----------------------------------------------------------------------------
int vtkMRMLResectionSurfaceStorageNode::ReadDataInternal(vtkMRMLNode *refNode)
{
  std::cout << "RP-StorageNode - ReadDataInternal: node = " << refNode << std::endl;
  //refNode->Print(std::cout);

  int result = 1;
  // Call the superclass to load the data rather than reimplement?
  result = this->Superclass::ReadDataInternal(refNode);

  // cast the input node
  //vtkMRMLResectionSurfaceNode *resectionSurfaceNode =  vtkMRMLResectionSurfaceNode::SafeDownCast(refNode);
  vtkMRMLResectionSurfaceNode *resectionSurfaceNode =  dynamic_cast < vtkMRMLResectionSurfaceNode *> (refNode);
  vtkPoints* controlPoints = NULL;

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName.empty())
  {
    vtkErrorMacro("ReadDataInternal: File name not specified");
    return 0;
  }

  vtkPolyData* tempPolyData = resectionSurfaceNode->GetPolyData();
  assert(tempPolyData);
  vtkDataSet* vtkData = vtkDataSet::SafeDownCast(tempPolyData);

  std::cout << "Field data: " << vtkData->GetFieldData() << std::endl;
  if(vtkData != NULL)
  {
    // Get the data back out
    //http://www.vtk.org/Wiki/VTK/Examples/Cxx/PolyData/FieldData
    vtkDataArray* temp = vtkData->GetFieldData()->GetArray("controlPoints");
    controlPoints = vtkPoints::New();
    controlPoints->SetData(temp);
  }

  if(controlPoints != NULL)
  {
    // Do something with the retrieved array
    // to set it back to the variables needed in vtkMRMLResectionSurfaceNode
    std::cout << "RP -StorageNode - ReadDataInternal: field data not nullptr " << std::endl;

    resectionSurfaceNode->SetControlPoints(controlPoints);
  }
  // if have not found any extra polydata to represent the control points
  else if(controlPoints == NULL)
  {
    std::cout << "RP-StorageNode - ReadDataInternal: field data nullptr " << std::endl;
    // do something else?
  }

  return result;
}

//----------------------------------------------------------------------------
int vtkMRMLResectionSurfaceStorageNode::WriteDataInternal(vtkMRMLNode *refNode)
{
  // downcast the input node
  vtkMRMLResectionSurfaceNode *resectionSurfaceNode = vtkMRMLResectionSurfaceNode::SafeDownCast(refNode);

  vtkPoints *controlPoints = resectionSurfaceNode->GetControlPoints();

  // put the name into the polydata
  controlPoints->GetData()->SetName("controlPoints"); //resectionSurfaceNode->GetNodeTagName()
  // add the extra info (control points) to the polydata
  vtkPolyData *tempPolyData = resectionSurfaceNode->GetPolyData();
  tempPolyData->GetFieldData()->AddArray(controlPoints->GetData());

  int result = this->Superclass::WriteDataInternal(refNode);

  std::cout << "RP -StorageNode - WriteDataInternal: " << result << std::endl;

  return result;
}

//----------------------------------------------------------------------------
void vtkMRMLResectionSurfaceStorageNode::InitializeSupportedReadFileTypes()
{
  this->Superclass::InitializeSupportedReadFileTypes();
}

//----------------------------------------------------------------------------
void vtkMRMLResectionSurfaceStorageNode::InitializeSupportedWriteFileTypes()
{
    this->Superclass::InitializeSupportedWriteFileTypes();
}
