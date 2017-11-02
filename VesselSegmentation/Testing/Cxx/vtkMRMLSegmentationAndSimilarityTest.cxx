/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLSegmentationAndSimilarityTest.cxx

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
#include <vtkMRMLMarkupsFiducialNode.h>
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

bool testLoadFileAndSegment(const char* volumeName1, const char* volumeName2, const char* volumeName3, vtkSlicerVesselSegmentationLogic* logic);

int vtkMRMLSegmentationAndSimilarityTest(int argc, char * argv[]  )
{
  itk::itkFactoryRegistration();

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  const char* fileName1 = "../Data/testImage3_large.nrrd";
  if (argc > 1)
    {
    fileName1 = argv[1];
    }
  std::cout << "Using file name segment " << fileName1 << std::endl;

  const char* fileName2 = "../Data/testImage3_similaritySlicer.nrrd";
  if (argc > 2)
    {
    fileName2 = argv[2];
    }
  std::cout << "Using file name similarity " << fileName2 << std::endl;

  const char* fileName3 = "../Data/testOutput.nrrd";
  if (argc > 3)
    {
    fileName3 = argv[3];
    }
  std::cout << "Using file output name " << fileName3 << std::endl;

  bool res = testLoadFileAndSegment(fileName1, fileName2, fileName3, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool testLoadFileAndSegment( const char* volumeName1, const char* volumeName2, const char* volumeName3, vtkSlicerVesselSegmentationLogic* logic )
{
  std::cout << "inside: testLoadFileAndSegment" << std::endl;

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
    std::cout << "testLoadFileAndSegment: no active volume." << std::endl;
    return false;
    }
  if (!activeVol->GetImageData())
    {
    std::cout << "testLoadFileAndSegment: active volume does not have data." << std::endl;
    return false;
    }

  // Define the dimension of the images
  const unsigned char Dim = 3;
  typedef float PixelType;

  // Declare the types of the images
  typedef itk::Image<PixelType,Dim> ImageType;

  //typedef itk::SeedVesselSegmentationImageFilter<ImageType, ImageType> SeedVesselFilterType;

  /*
  //create conversion matrices
  vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
  vtkNew<vtkMatrix4x4> RAStoIJKmatrix;

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  logic->GetActiveVolume()->GetIJKToRASMatrix(mat);
  IJKtoRASmatrix->DeepCopy(mat);
  RAStoIJKmatrix->DeepCopy(mat);
  RAStoIJKmatrix->Invert();
  */

  //--------------------------
  // add the seed node, set the seeds
  //--------------------------
  vtkNew<vtkMRMLVesselSegmentationSeedNode> seedNode;
  scene->AddNode(seedNode.GetPointer());
  std::cout << "added seed node to scene" << std::endl;

  // Seed IJK: 155, 118, 41
  // Direction seed IJK: 145, 116, 55

  // Seed RAS: 34.15 34.15 158.375 (older test)
  // Direction seed RAS: 40.4 36.625 167.125 (older test)

  // Seed RAS: -96.875 -73.75 25.625 (newer test)
  // Direction seed RAS: -90.625 -72.5 34.375 (newer test)

  /*
  // conversion from known IJK to RAS (for this test image)
  const double seed1[4] = {155, 118, 41, 0}; // IJK
  const double seed2[4] = {145, 116, 55, 0}; // IJK

  double *seedRAS1 = new double[4]; // the first seed
  double *seedRAS2 = new double[4]; // the direction seed

  // use the ijk matrix to convert the points
  IJKtoRASmatrix.GetPointer()->MultiplyPoint(seed1, seedRAS1);
  IJKtoRASmatrix.GetPointer()->MultiplyPoint(seed2, seedRAS2);

  std::cout << "Seed ras1 (155, 118, 41): " << seedRAS1[0] << " " << seedRAS1[1] << " " << seedRAS1[2] << " " << seedRAS1[3] << std::endl;
  std::cout << "Seed ras2 (145, 116, 55): " << seedRAS2[0] << " " << seedRAS2[1] << " " << seedRAS2[2] << " " << seedRAS2[3] << std::endl;
  */

  // add 1st seed
  double inPos[3] = {-96.875, -73.75, 25.625}; // RAS
  seedNode->SetSeed1(inPos[0], inPos[1], inPos[2]);
  // add 2nd (direction) seed
  double inPos2[3] = {-90.625, -72.5, 34.375}; // RAS
  seedNode->SetSeed2(inPos2[0], inPos2[1], inPos2[2]);

  /*
  double *seed1 = seedNode->GetSeed1();
  double *seed2 = seedNode->GetSeed2();

  std::cout << "Seed1: " << seed1[0] << " " << seed1[1] << " " << seed1[2] << std::endl;
  std::cout << "Seed2: " << seed2[0] << " " << seed2[1] << " " << seed2[2] << std::endl;

  if( seed1[0] == 0.0 && seed1[1] == 0.0 && seed1[2] == 0.0 )
  {
    std::cout << "Seed1 not set" << std::endl;
    return false;
  }
  else if( seed2[0] == 0.0 && seed2[1] == 0.0 && seed2[2] == 0.0 )
  {
    std::cout << "Seed2 not set" << std::endl;
    return false;
  }

  const double seed1_0[4] = {seed1[0], seed1[1], seed1[2], 0}; // IJK
  const double seed2_0[4] = {seed2[0], seed2[1], seed2[2], 0}; // IJK
  double *seedIJK1 = new double[4]; // the first seed
  double *seedIJK2 = new double[4]; // the direction seed

  // use the ijk matrix to convert the points
  RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed1_0, seedIJK1);
  RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed2_0, seedIJK2);

  std::cout << "Seed IJK1 (155, 118, 41): " << seedIJK1[0] << " " << seedIJK1[1] << " " << seedIJK1[2] << std::endl;
  std::cout << "Seed IJK2 (145, 116, 55): " << seedIJK2[0] << " " << seedIJK2[1] << " " << seedIJK2[2] << std::endl;

  itk::Index<Dim> seed;
  itk::Index<Dim> directionSeed;

  // the actual first seed
  seed[0] = seedIJK1[0];
  seed[1] = seedIJK1[1];
  seed[2] = seedIJK1[2];
  // the direction seed
  directionSeed[0] = seedIJK2[0];
  directionSeed[1] = seedIJK2[1];
  directionSeed[2] = seedIJK2[2];

  std::cout << "Seed: " << seed[0] << " " << seed[1] << " " << seed[2] << std::endl;
  std::cout << "Direction seed: " << directionSeed[0] << " " << directionSeed[1] << " " << directionSeed[2] << std::endl;

  if( !(seed[0] <= 156 && seed[0] >= 154) )
  {
    std::cout << "Seed[0] is not correct in IJK (155): " << seed[0] << std::endl;
    return false;
  }
  else if( !(seed[1] <= 119 && seed[1] >= 117) )
  {
    std::cout << "Seed[1] is not correct in IJK (118): " << seed[1] << std::endl;
    return false;
  }
  else if( seed[2] != 41 )
  {
    std::cout << "Seed[2] is not correct in IJK (41): " << seed[2] << std::endl;
    return false;
  }
  else if( !(directionSeed[0] <= 146 && directionSeed[0] >= 144) )
  {
    std::cout << "directionSeed[0] is not correct in IJK (145): " << directionSeed[0] << std::endl;
    return false;
  }
  else if( !(directionSeed[1] <= 117 && directionSeed[1] >= 115) )
  {
    std::cout << "directionSeed[1] is not correct in IJK (116): " << directionSeed[1] << std::endl;
    return false;
  }
  else if( directionSeed[2] != 55 )
  {
    std::cout << "directionSeed[2] is not correct in IJK (55): " << directionSeed[2] << std::endl;
    return false;
  }
  */

  // call the segmentation
  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->SegmentVessels(seedNode.GetPointer(), false);\
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  ImageType::Pointer output = logic->GetPortalITKData();

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

  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol2 = logic->GetActiveVolume();

  // check have valid image data
  if (!activeVol2)
    {
    std::cout << "testLoadFileAndSegment: no active volume." << std::endl;
    return false;
    }
  if (!activeVol2->GetImageData())
    {
    std::cout << "testLoadFileAndSegment: active volume does not have data." << std::endl;
    return false;
    }

  // get similarity image
  ImageType::Pointer similarityImg = vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(activeVol2);

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
