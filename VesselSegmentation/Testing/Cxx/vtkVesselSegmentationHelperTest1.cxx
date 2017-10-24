/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkVesselSegmentationHelperTest1.cxx

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

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkImageData.h>

// ITK IO factory includes
#include <itkConfigure.h>
#include <itkFactoryRegistration.h>
#include <itkImageIOFactory.h>

// ITK includes
#include <itkImageFileReader.h>

// Qt includes
#include <qSlicerCoreIOManager.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerIO.h>

// module includes
#include "vtkVesselSegmentationHelper.h"
#include "vtkSlicerVesselSegmentationLogic.h"

bool testImageConversions( const char* volumeName1, vtkSlicerVesselSegmentationLogic* logic );

int vtkVesselSegmentationHelperTest1(int argc, char * argv[]  )
{
  itk::itkFactoryRegistration();

  bool res = false;

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  const char* fileName1 = "../Data/testImage3_large.nii.gz";
  if (argc > 1)
    {
    fileName1 = argv[1];
    }
  std::cout << "Using file name " << fileName1 << std::endl;

  res = testImageConversions(fileName1, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool testImageConversions( const char* volumeName1, vtkSlicerVesselSegmentationLogic* logic )
{
  std::cout << "inside: testConvertImage" << std::endl;

  std::cout << "There are\n"
            << itk::ObjectFactoryBase::CreateAllInstance( "itkImageIOBase" ).size()
            << " IO objects available to the ImageFileReader.\n" << std::endl;

  vtkNew<vtkMRMLVolumeArchetypeStorageNode> storageNode1;
  vtkNew<vtkMRMLScalarVolumeNode> scalarNode1;

  storageNode1->SetFileName(volumeName1);
  std::cout << "Try to load the file" << std::endl;
  storageNode1->ReadData(scalarNode1.GetPointer());
  logic->SetActiveVolume(scalarNode1.GetPointer());

  std::cout << "active volume node = " << logic->GetActiveVolume() << std::endl;
  if( logic->GetActiveVolume() == NULL)
  {
    std::cout << "active volume node is null pointer" << std::endl;
    return false;
  }

  vtkVesselSegmentationHelper::SeedImageType::Pointer itkConvertedImage = vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(logic->GetActiveVolume());
  if (itkConvertedImage.IsNull() == true )
  {
    std::cout << "Conversion from volume node to ITK not successful" << std::endl;
    return false;
  }

  vtkSmartPointer<vtkImageData> vtkConvertedData = vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(itkConvertedImage);
  if (vtkConvertedData.GetPointer() == NULL)
  {
    std::cout << "Conversion from ITK image to VTK data not successful" << std::endl;
    return false;
  }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> volumeNodeConverted = vtkVesselSegmentationHelper::ConvertItkImageToVolumeNode(itkConvertedImage);
  if (volumeNodeConverted.GetPointer()->GetImageData() == NULL)
  {
    std::cout << "Conversion from ITK image to volume node not successful" << std::endl;
    return false;
  }

  return true;
}

