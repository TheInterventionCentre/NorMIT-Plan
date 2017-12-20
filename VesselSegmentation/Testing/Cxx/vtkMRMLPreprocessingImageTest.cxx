/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLMergeLabelsAndSplitTest.cxx

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
#include <vtkMatrix4x4.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLApplicationLogic.h>

// VTK includes
#include <vtkNew.h>

// ITK IO factory includes
#include <itkConfigure.h>
#include <itkFactoryRegistration.h>

// ITK includes
#include "itkSeedVesselSegmentationImageFilter.h"
#include <itkIndex.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageIOFactory.h>
#include <itkMinimumMaximumImageCalculator.h>

// Qt includes
#include <qSlicerCoreIOManager.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerIO.h>

// module includes
#include "vtkSlicerVesselSegmentationLogic.h"
#include "vtkMRMLVesselSegmentationSeedNode.h"
#include "vtkVesselSegmentationHelper.h"

bool testPreprocessAndCompare(const char* volumeName1, const char* volumeName2,
    const char* volumeName3, vtkSlicerVesselSegmentationLogic* logic);

int vtkMRMLPreprocessingImageTest(int argc, char * argv[]  )
{
  itk::itkFactoryRegistration();

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  const char* fileName1 = "../Data/original_cropped.nrrd";
  if (argc > 1)
    {
    fileName1 = argv[1];
    }
  std::cout << "Using file name background " << fileName1 << std::endl;

  const char* fileName2 = "../Data/preprocessedImage.nrrd";
  if (argc > 2)
    {
    fileName2 = argv[2];
    }
  std::cout << "Using file name hepatic " << fileName2 << std::endl;

  const char* fileName3 = "../Data/testPreprocessedOutput.nrrd";
  if (argc > 3)
    {
    fileName3 = argv[3];
    }
  std::cout << "Using file name portal " << fileName3 << std::endl;

  bool res = testPreprocessAndCompare(fileName1, fileName2, fileName3, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool testPreprocessAndCompare(const char* volumeName1, const char* volumeName2,
    const char* volumeName3, vtkSlicerVesselSegmentationLogic* logic)
{
  std::cout << "inside: testPreprocessAndCompare" << std::endl;

  //Retrieve a pointer to the mrml scene
  vtkMRMLScene *scene = logic->GetMRMLScene();

  std::cout << "There are\n"
            << itk::ObjectFactoryBase::CreateAllInstance( "itkImageIOBase" ).size()
            << " IO objects available to the ImageFileReader.\n" << std::endl;

  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> storageNode1 =
      vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New();
  vtkSmartPointer<vtkMRMLScalarVolumeNode> scalarNode1 =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  storageNode1->SetFileName(volumeName1);
  std::cout << "Try to load the file" << std::endl;
  storageNode1->ReadData(scalarNode1.GetPointer());
  std::cout << "Past read data" << std::endl;

  vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
    vtkSmartPointer<vtkMRMLSelectionNode>::New();

  vtkSmartPointer<vtkMRMLApplicationLogic> appLogic =
    vtkSmartPointer<vtkMRMLApplicationLogic>::New();

  logic->SetMRMLApplicationLogic(appLogic);

  scene->AddNode(selectionNode);
  scene->AddNode(scalarNode1);
  char* id = scalarNode1->GetID();
  selectionNode->SetActiveVolumeID(id);

  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = logic->GetActiveVolume();

  // check have valid image data
  if (!activeVol)
    {
    std::cout << "testPreprocessAndCompare: no active volume." << std::endl;
    return false;
    }
  if (!activeVol->GetImageData())
    {
    std::cout << "testPreprocessAndCompare: active volume does not have data." << std::endl;
    return false;
    }

  // Define the dimension of the images
  const unsigned char Dim = 3;
  typedef float PixelType;

  // Declare the types of the images
  typedef itk::Image<PixelType,Dim> ImageType;

  // merge the label maps
  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->PreprocessImage(100,250,20,160,25,30);
  TESTING_OUTPUT_ASSERT_ERRORS(0); // check have no errors
  TESTING_OUTPUT_RESET(); // reset to clear errors + warnings

  std::cout << "Getting preprocessed ITK data to write out" << std::endl;
  ImageType::Pointer output = logic->GetPreprocessedITKData();

  // check we have the preprocessed image
  if(output.IsNull())
    {
    std::cout << "testPreprocessAndCompare: no preprocessed image." << std::endl;
    return false;
    }

  typedef itk::ImageFileWriter<ImageType> FileWriterType;

  FileWriterType::Pointer writer = FileWriterType::New();
  writer->SetFileName(volumeName3);
  writer->SetInput(output);

  try
  {
      writer->Update();
  }
  catch (itk::ExceptionObject &e)
  {
      std::cerr << e << std::endl;
      return EXIT_FAILURE;
  }
  std::cout << "Wrote out merged data" << std::endl;

  //--------------------------
  //Similarity Check
  //--------------------------

  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> storageNode2 =
      vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New();
  vtkSmartPointer<vtkMRMLScalarVolumeNode> scalarNode2 =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  storageNode2->SetFileName(volumeName2);
  std::cout << "Try to load the file" << std::endl;
  storageNode2->ReadData(scalarNode2.GetPointer());
  std::cout << "Past read data" << std::endl;

  scene->AddNode(scalarNode2);
  char* id2 = scalarNode2->GetID();
  selectionNode->SetActiveVolumeID(id2);

  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol4 = logic->GetActiveVolume();

  // check have valid image data
  if (!activeVol4)
    {
    std::cout << "testPreprocessAndCompare: no active volume." << std::endl;
    return false;
    }
  if (!activeVol4->GetImageData())
    {
    std::cout << "testPreprocessAndCompare: active volume does not have data." << std::endl;
    return false;
    }

  // get similarity image
  ImageType::Pointer similarityImg = vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(activeVol4);

  typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;

  ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
  imageCalculatorFilter->SetImage(similarityImg);
  imageCalculatorFilter->Compute();

  double maxPx= imageCalculatorFilter->GetMaximum();
  double minPx= imageCalculatorFilter->GetMinimum();
  double diff = 0;
  unsigned int nPix = 0;
  itk::ImageRegionIterator<ImageType> itFilterOutput(output, output->GetLargestPossibleRegion());
  itk::ImageRegionIterator<ImageType> itSimilarityInput(similarityImg, similarityImg->GetLargestPossibleRegion());

  for(itFilterOutput.GoToBegin(), itSimilarityInput.GoToBegin(); !itSimilarityInput.IsAtEnd(); ++itFilterOutput, ++itSimilarityInput)
  {
      double err = (double)(itSimilarityInput.Get()) - (double)(itFilterOutput.Get());
      diff += err*err;
      nPix++;
  }

  if (nPix > 0)
  {
      double NormRMSError = sqrt( diff / (double)nPix )/(maxPx-minPx);
      std::cout << "Normalised RMS Error = " << NormRMSError << std::endl;

      if (vnl_math_isnan(NormRMSError))
      {
          std::cout << "Normalised RMS Error is NaN! nPix: " << nPix << std::endl;
          return false;
      }
      if (NormRMSError > 0.001)
      {
          std::cout << "Normalised RMS Error exceeds threshold (" << 0.001 << ")" << std::endl;
          return false;
      }
  }
  else
  {
      std::cout << "No pixels in output!" << std::endl;
      return false;
  }

  return true;
}
