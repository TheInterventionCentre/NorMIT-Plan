/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager3D.cxx

  Copyright (c) 2018, The Intervention Centre, Oslo University Hospital

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

#include "vtkDistanceFilter.h"

// VTK includes
#include <vtkKdTreePointLocator.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkDemandDrivenPipeline.h>
#include <vtkObjectFactory.h>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkDistanceFilter);

//------------------------------------------------------------------------------
vtkDistanceFilter::vtkDistanceFilter()
  : TargetLastModified(-1),
    DistanceArray(NULL)
{
  this->SetNumberOfInputPorts(2);
  this->SetNumberOfOutputPorts(1);
}

//------------------------------------------------------------------------------
vtkDistanceFilter::~vtkDistanceFilter()
{

}

//------------------------------------------------------------------------------
void vtkDistanceFilter::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


//------------------------------------------------------------------------------
int vtkDistanceFilter::RequestData(vtkInformation *request,
                                   vtkInformationVector **inputVector,
                                   vtkInformationVector *outputVector)
{

  vtkPolyData *inSource = vtkPolyData::GetData(inputVector[0],0);
  vtkPolyData *inTarget = vtkPolyData::GetData(inputVector[1],0);
  vtkPolyData *output =   vtkPolyData::GetData(outputVector, 1);

    std::cout << "updated "
              << inSource
              << std::endl;




    // output->DeepCopy(inSource);

    // std::cout << "updated" << std::endl;
    // std::cout << inSource << std::endl;

    // // output->CopyStructure(inSource);
    // // output->GetPointData()->PassData(inSource->GetPointData());
    // // output->GetCellData()->PassData(inSource->GetCellData());
    // // output->BuildCells();

    // // Allocate distance array if the target polydata was modified
    // // or the array was not allocated.
    // if (this->DistanceArray == NULL || this->TargetLastModified != inTarget->GetMTime())
    //   {
    //   this->DistanceArray = vtkSmartPointer<vtkDoubleArray>::New();
    //   this->DistanceArray->SetName("Distance");
    //   this->DistanceArray->SetNumberOfComponents(1);
    //   this->DistanceArray->SetNumberOfTuples(inSource->GetNumberOfPoints());
    //   }

    // // Build the KD tree locator if the target polydata was modified
    // if (TargetLastModified != inTarget->GetMTime())
    //   {
    //   PointLocator->SetDataSet(inTarget);
    //   PointLocator->BuildLocator();
    //   this->TargetLastModified = inTarget->GetMTime(); // HERE WE UPDATE TARGETLASTMODIFIED!!
    //   }

    // for(int i=0; i<inSource->GetNumberOfPoints();++i)
    //   {
    //   double distance;
    //   double currentPoint[3];
    //   double closestPoint[3];

    //   inSource->GetPoint(i, currentPoint);

    //   vtkIdType closestPointID = this->PointLocator->FindClosestPoint(currentPoint);
    //   inTarget->GetPoint(closestPointID, closestPoint);

    //   distance = sqrt(pow(currentPoint[0]-closestPoint[0],2)+
    //                   pow(currentPoint[1]-closestPoint[1],2)+
    //                   pow(currentPoint[2]-closestPoint[2],2));

    //   this->DistanceArray->SetValue(i, distance);
    //   }

    // output->GetPointData()->AddArray(this->DistanceArray);
    // output->GetPointData()->SetActiveScalars("Distance");

    return 1;
}

//------------------------------------------------------------------------------
int vtkDistanceFilter::FillInputPortInformation( int port, vtkInformation* info )
{

	//The input should be two vtkPolyData
	if (port == 0)
	{
		info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
		return 1;
	}
	if (port == 1)
	{
		info->Set( vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData" );
		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
int vtkDistanceFilter::FillOutputPortInformation( int port, vtkInformation* info )
{

	//The input should be two vtkPolyData
	if (port == 0)
	{
		info->Set( vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData" );
		return 1;
	}

	return 0;
}
