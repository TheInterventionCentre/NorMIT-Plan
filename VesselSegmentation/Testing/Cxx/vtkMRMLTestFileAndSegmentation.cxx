/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLTestFileAndSegmentation.cxx

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
#include "vtkMRMLCoreTestingMacros.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLNode.h"
#include <vtkMRMLMarkupsFiducialNode.h>
#include "vtkMRMLScalarVolumeNode.h"
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkMatrix4x4.h>

// VTK includes
#include <vtkNew.h>

// ITK includes
#include "itkSeedVesselSegmentationImageFilter.h"
#include "itkIndex.h"
#include "itkImageFileReader.h"
#include "itkMinimumMaximumImageCalculator.h"

// Qt includes
#include <qSlicerCoreIOManager.h>
#include <qSlicerCoreApplication.h>
#include <qSlicerIO.h>

// module includes
#include "vtkSlicerVesselSegmentationLogic.h"
#include "vtkVesselSegHelper.h"

bool testLoadFileAndSegment( const char* volumeName1, const char* volumeName2, vtkSlicerVesselSegmentationLogic* logic );

int vtkMRMLTestFileAndSegmentation(int argc, char * argv[]  )
{
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
  std::cout << "Using file name segment " << fileName1 << std::endl;

  const char* fileName2 = "../Data/testImage3_largeSimilarity.nii.gz";
  if (argc > 2)
    {
    fileName2 = argv[2];
    }
  std::cout << "Using file name similarity " << fileName2 << std::endl;

  res = testLoadFileAndSegment(fileName1, fileName2, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool testLoadFileAndSegment( const char* volumeName1, const char* volumeName2, vtkSlicerVesselSegmentationLogic* logic )
{
  std::cout << "inside: testLoadFileAndSegment" << std::endl;
  //Retrieve a pointer to the mrml scene
  vtkMRMLScene *scene = logic->GetMRMLScene();

  vtkNew<vtkMRMLVolumeArchetypeStorageNode> storageNode1;
  vtkNew<vtkMRMLScalarVolumeNode> scalarNode1;

  storageNode1->SetFileName(volumeName1);
  std::cout << "Try to load the file" << std::endl;
  storageNode1->ReadData(scalarNode1.GetPointer());
  logic->SetActiveVolume(scalarNode1.GetPointer());

  std::cout << "active volume node segmentation = " << logic->GetActiveVolume() << std::endl;
  if( logic->GetActiveVolume() == NULL)
  {
    std::cout << "active volume node is null pointer" << std::endl;
    return false;
  }

  // add the fiducial node
  vtkSmartPointer<vtkMRMLMarkupsFiducialNode> fidNode = vtkSmartPointer<vtkMRMLMarkupsFiducialNode>::New();
  scene->AddNode(fidNode);
  std::cout << "added fiducial node to scene" << std::endl;

    // Define the dimension of the images
    const unsigned char Dim = 3;
    typedef float PixelType;

    // Declare the types of the images
    typedef itk::Image<PixelType,Dim> ImageType;

    typedef itk::SeedVesselSegmentationImageFilter<ImageType, ImageType> SeedVesselFilterType;

    // Create a vesselness Filter
    SeedVesselFilterType::Pointer seedVesselFilter = SeedVesselFilterType::New();

    // get image
    ImageType::Pointer preprocessedImg = vtkVesselSegHelper::ConvertVolumeNodeToItkImage(logic->GetActiveVolume());

    // Connect to input image
    seedVesselFilter->SetInput( preprocessedImg );

    //create conversion matrices
    vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
    vtkNew<vtkMatrix4x4> RAStoIJKmatrix;

    vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
    logic->GetActiveVolume()->GetIJKToRASMatrix(mat);
    IJKtoRASmatrix->DeepCopy(mat);
    RAStoIJKmatrix->DeepCopy(mat);
    RAStoIJKmatrix->Invert();

    // Seed RAS: 34.15 34.15 158.375
    // Direction seed RAS: 40.4 36.625 167.125
    // Seed IJK: 155, 118, 41
    // Direction seed IJK: 145, 116, 55

    // add 1st fiducial
    double inPos[3] = {34.15, 34.15, 158.375}; // RAS
    int fidIndex = fidNode->AddFiducial(inPos[0], inPos[1], inPos[2]);
    // add 2nd (direction) fiducial
    double inPos2[3] = {40.4, 36.625, 167.125}; // RAS
    fidIndex = fidNode->AddFiducial(inPos2[0], inPos2[1], inPos2[2]);

    itk::Index<Dim> seed;
    itk::Index<Dim> directionSeed;
    double *fidIJK1 = new double[4]; // the actual first seed
    double *fidIJK2 = new double[4]; // the direction seed

    std::vector<double*> list = logic->GetFiducialList();
    if(list.size() == 2)
    {
      double *pos2 = list.back(); // the direction seed
      list.pop_back();
      double *pos1 = list.back(); // the actual first seed
      list.pop_back();
      // convert to IJK
      RAStoIJKmatrix.GetPointer()->MultiplyPoint(pos1, fidIJK1);
      RAStoIJKmatrix.GetPointer()->MultiplyPoint(pos2, fidIJK2);
      seed[0] = fidIJK1[0];
      seed[1] = fidIJK1[1];
      seed[2] = fidIJK1[2];
      directionSeed[0] = fidIJK2[0];
      directionSeed[1] = fidIJK2[1];
      directionSeed[2] = fidIJK2[2];
    }
    else
    {
      std::cout << "Do not have 2 fiducials" << std::endl;
      return false;
    }


    std::cout << "Seed IJK: " << seed[0] << " " << seed[1] << " " << seed[2] << std::endl;
    std::cout << "Direction seed IJK: " << directionSeed[0] << " " << directionSeed[1] << " " << directionSeed[2] << std::endl;
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

    seedVesselFilter->SetSeed(seed);
    seedVesselFilter->SetDirectionSeed(directionSeed);
    seedVesselFilter->SetOutputLabel(2);

    try
    {
        seedVesselFilter->Update();
    }
    catch (itk::ExceptionObject &e)
    {
        std::cerr << e << std::endl;
        return false;
    }


    //--------------------------
    //Similarity Check
    //--------------------------

    vtkNew<vtkMRMLVolumeArchetypeStorageNode> storageNode2;
    vtkNew<vtkMRMLScalarVolumeNode> scalarNode2;

    storageNode2->SetFileName(volumeName2);
    std::cout << "Try to load the file" << std::endl;
    storageNode2->ReadData(scalarNode2.GetPointer());
    logic->SetActiveVolume(scalarNode2.GetPointer());

    std::cout << "active volume node segmentation = " << logic->GetActiveVolume() << std::endl;
    if( logic->GetActiveVolume() == NULL)
    {
      std::cout << "active volume node is null pointer" << std::endl;
      return false;
    }

    // get image
    ImageType::Pointer similarityImg = vtkVesselSegHelper::ConvertVolumeNodeToItkImage(logic->GetActiveVolume());

    typedef itk::MinimumMaximumImageCalculator <ImageType> ImageCalculatorFilterType;

    ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New ();
    imageCalculatorFilter->SetImage(similarityImg);
    imageCalculatorFilter->Compute();

    double maxPx= imageCalculatorFilter->GetMaximum();
    double minPx= imageCalculatorFilter->GetMinimum();
    double diff = 0;
    unsigned int nPix = 0;
    itk::ImageRegionIterator<ImageType> itFilterOutput(seedVesselFilter->GetOutput(), seedVesselFilter->GetOutput()->GetLargestPossibleRegion());
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
