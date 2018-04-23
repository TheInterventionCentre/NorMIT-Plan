/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLLRPModelStorageNode.cxx

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
#include "vtkMRMLLRPModelNode.h"
#include "vtkMRMLLRPModelStorageNode.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

//------------------------------------------------------------------------------
vtkMRMLNodeNewMacro(vtkMRMLLRPModelStorageNode);

//------------------------------------------------------------------------------
vtkMRMLLRPModelStorageNode::vtkMRMLLRPModelStorageNode()
{
  this->DefaultWriteFileExtension = "vtk";
}

//------------------------------------------------------------------------------
vtkMRMLLRPModelStorageNode::~vtkMRMLLRPModelStorageNode()
{

}

//------------------------------------------------------------------------------
void vtkMRMLLRPModelStorageNode::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
bool vtkMRMLLRPModelStorageNode::CanReadInReferenceNode(vtkMRMLNode *refNode)
{
  return refNode->IsA("vtkMRMLLRPModelNode");
}

//------------------------------------------------------------------------------
int vtkMRMLLRPModelStorageNode::ReadDataInternal(vtkMRMLNode *refNode)
{
  if (!refNode)
    {
    vtkErrorMacro("No reference node.");
    return 0;
    }

  vtkMRMLLRPModelNode *lrpModelNode =
    vtkMRMLLRPModelNode::SafeDownCast(refNode);

  if (!lrpModelNode)
    {
    vtkErrorMacro("Reference node is not of type vtkMRMLLRPNode.");
    return 0;
    }

  int retval = this->Superclass::ReadDataInternal(refNode);


  vtkPolyData* lrpPolyData = lrpModelNode->GetPolyData();

  // Check the type of model
  std::string anatomicalName;
  vtkStringArray* anatomicalType =
    vtkStringArray::SafeDownCast(lrpPolyData->GetFieldData()->GetAbstractArray("Name"));
  if (!anatomicalType)
    {
    anatomicalName = "Unknown";
    }
  else
    {
    anatomicalName = anatomicalType->GetValue(1);
    }

  if (anatomicalName == "Parenchyma" || anatomicalName == "parenchyma")
    {
    lrpModelNode->SetTypeOfAnatomicalStructure(1);
    }
  else if (anatomicalName == "Portal" || anatomicalName == "portal")
    {
    lrpModelNode->SetTypeOfAnatomicalStructure(2);
    }
  else if (anatomicalName == "Hepatic" || anatomicalName == "hepatic")
    {
    lrpModelNode->SetTypeOfAnatomicalStructure(3);
    }
  else if (anatomicalName == "Tumor" || anatomicalName == "tumor")
    {
    lrpModelNode->SetTypeOfAnatomicalStructure(4);
    }
  else
    {
    lrpModelNode->SetTypeOfAnatomicalStructure(0);
    }

  // Remove field data
  vtkFieldData* lrpFieldData = lrpPolyData->GetFieldData();
  for(int i=0; i<lrpFieldData->GetNumberOfArrays(); ++i)
    {
    lrpFieldData->RemoveArray(0);
    }

  // Remove cell data
  vtkCellData* lrpCellData = lrpPolyData->GetCellData();
  for(int i=0; i<lrpCellData->GetNumberOfArrays(); ++i)
    {
    lrpCellData->RemoveArray(0);
    }

  // Remove point data
  vtkPointData* lrpPointData = lrpPolyData->GetPointData();
  for(int i=0; i<lrpPointData->GetNumberOfArrays(); ++i)
    {
    lrpPointData->RemoveArray(0);
    }

  return retval;
}

//------------------------------------------------------------------------------
int vtkMRMLLRPModelStorageNode::WriteDataInternal(vtkMRMLNode *refNode)
{
  if (!refNode)
    {
    vtkErrorMacro("No reference node.");
    return 0;
    }

  vtkMRMLLRPModelNode *lrpModel =
    vtkMRMLLRPModelNode::SafeDownCast(refNode);

  if (!lrpModel)
    {
    vtkErrorMacro("Reference node is not of type vtkMRMLLRPModelNode.");
    return 0;
    }

  // Get the polyData
  vtkPolyData *polyData = lrpModel->GetPolyData();
  if (!polyData)
    {
    vtkErrorMacro("The node does not contain any polydata.");
    return 0;
    }

  float rgba[4];
  std::string anatomicalName;
  if (lrpModel->GetTypeOfAnatomicalStructure() == 1)
    {
    rgba[0]=238; rgba[1]=192; rgba[2]=178; rgba[3]=76;
    anatomicalName="Parenchyma";
    }
  else if (lrpModel->GetTypeOfAnatomicalStructure() == 2)
    {
    rgba[0]=206; rgba[1]=000; rgba[2]=003; rgba[3]=255;
    anatomicalName="Portal";
    }
  else if (lrpModel->GetTypeOfAnatomicalStructure() == 3)
    {
    rgba[0]=000; rgba[1]=151; rgba[2]=206; rgba[3]=255;
    anatomicalName="Hepatic";
    }
  else if (lrpModel->GetTypeOfAnatomicalStructure() == 4)
    {
    rgba[0]=255; rgba[1]=255; rgba[2]=102; rgba[3]=255;
    anatomicalName="Tumor";
    }
  else
    {
    rgba[0]=128; rgba[1]=128; rgba[2]=128; rgba[3]=128;
    anatomicalName="Unknown";
    }

  // Generate and insert the colors
  vtkSmartPointer<vtkUnsignedCharArray> cellData =
    vtkSmartPointer<vtkUnsignedCharArray>::New();
  cellData->SetNumberOfComponents(4);
  cellData->SetNumberOfTuples(polyData->GetNumberOfCells());
  for(int i=0; i<polyData->GetNumberOfCells(); ++i)
    {
    cellData->InsertNextTuple(rgba);
    }

  polyData->GetCellData()->SetScalars(cellData);

  // Generate and insert the name of the anatomical structure
  vtkSmartPointer<vtkStringArray> name =
    vtkSmartPointer<vtkStringArray>::New();
  name->SetName("Name");
  name->SetNumberOfComponents(1);
  name->SetNumberOfTuples(1);
  name->InsertNextValue(anatomicalName.c_str());

  polyData->GetFieldData()->AddArray(name);

  int retval = this->Superclass::WriteDataInternal(lrpModel);

  polyData->GetFieldData()->RemoveArray("Name");
  polyData->GetCellData()->RemoveArray(polyData->GetCellData()->GetNumberOfArrays()-1);

  return retval;

}

//------------------------------------------------------------------------------
void vtkMRMLLRPModelStorageNode::InitializeSupportedReadFileTypes()
{
  this->Superclass::InitializeSupportedReadFileTypes();
}

//------------------------------------------------------------------------------
void vtkMRMLLRPModelStorageNode::InitializeSupportedWriteFileTypes()
{
  this->Superclass::InitializeSupportedWriteFileTypes();
}
