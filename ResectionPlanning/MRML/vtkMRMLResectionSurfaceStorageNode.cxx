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
  if (!refNode)
    {
    vtkErrorMacro("No reference node.");
    return 0;
    }


  // cast the input node
  vtkMRMLResectionSurfaceNode *resectionSurfaceNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(refNode);

  if (!resectionSurfaceNode)
    {
    vtkErrorMacro("Reference node is not of type vtkMRMLResectionSurfaceNode.");
    return 0;
    }

  vtkNew<vtkMRMLResectionSurfaceNode> tempSurfaceNode;

  int result = 1;
  // Call the superclass to load the data rather than reimplement?
  result = this->Superclass::ReadDataInternal(tempSurfaceNode.GetPointer());

  if (!result)
    {
    vtkErrorMacro("Internal reading error");
    return 0;
    }

  vtkSmartPointer<vtkPoints> controlPoints = vtkSmartPointer<vtkPoints>::New();

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName.empty())
    {
    vtkErrorMacro("ReadDataInternal: File name not specified");
    return 0;
    }

  vtkPolyData* tempPolyData = tempSurfaceNode->GetPolyData();
  if (!tempSurfaceNode.GetPointer())
    {
    vtkErrorMacro("No polydata present in the loaded node.");
    return 0;
    }

  vtkDataArray *dataArray =
    tempPolyData->GetFieldData()->GetArray("ControlPoints");
  if (!dataArray)
    {
    vtkErrorMacro("Resection polydata does not contain control points");
    return 0;
    }

  controlPoints->SetData(dataArray);

  resectionSurfaceNode->SetControlPoints(controlPoints);

  return result;
}

//----------------------------------------------------------------------------
int vtkMRMLResectionSurfaceStorageNode::WriteDataInternal(vtkMRMLNode *refNode)
{
  if (!refNode)
    {
    vtkErrorMacro("No reference node");
    return 0;
    }

// downcast the input node
  vtkMRMLResectionSurfaceNode *resectionSurfaceNode =
    vtkMRMLResectionSurfaceNode::SafeDownCast(refNode);

  if (!resectionSurfaceNode)
    {
    vtkErrorMacro("Reference node is not of type vtkMRMLResectionSurfaceNode");
    return 0;
    }

  vtkPoints *controlPoints = resectionSurfaceNode->GetControlPoints();
  if (!controlPoints)
    {
    vtkErrorMacro("Resection node does not contain any control points.");
    return 0;
    }
  controlPoints->GetData()->SetName("ControlPoints");

  // add the extra info (control points) to the polydata
  vtkPolyData *resectionPolyData = resectionSurfaceNode->GetPolyData();
  if (!resectionPolyData)
    {
    vtkErrorMacro("Resection node does not contain any polydata");
    return 0;
    }
  resectionPolyData->GetFieldData()->AddArray(controlPoints->GetData());

  int result = this->Superclass::WriteDataInternal(refNode);

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
