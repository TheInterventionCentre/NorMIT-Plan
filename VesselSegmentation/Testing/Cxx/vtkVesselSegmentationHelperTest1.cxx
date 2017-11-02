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
#include <vtkMRMLSelectionNode.h>

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

#include <vtkTestingOutputWindow.h>


class vtkVesselSegmentationHelperTest:
  public vtkVesselSegmentationHelper
{
public:
  static vtkVesselSegmentationHelperTest* New();
  vtkTypeMacro(vtkVesselSegmentationHelperTest,
      vtkVesselSegmentationHelper);

  void ConvertVolumeNodeToItkImageTest1()
  {
    vtkDebugMacro("BEGIN: ConvertVolumeNodeToItkImageTest1"
                  << "------------------------------");
    // trigger error: Pointer to vtkMRMLScalarVolumeNode is NULL

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertVolumeNodeToItkImage(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertVolumeNodeToItkImageTest1"
                  << "------------------------------");
  }

  void ConvertVtkImageDataToItkImageTest1()
  {
    vtkDebugMacro("BEGIN: ConvertVtkImageDataToItkImageTest1"
                  << "------------------------------");
    // trigger error: Pointer to vtkImageData is NULL

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertVtkImageDataToItkImage(NULL, NULL, NULL, NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertVtkImageDataToItkImageTest1"
                  << "------------------------------");
  }

  void ConvertItkImageToVtkImageDataTest1()
  {
    vtkDebugMacro("BEGIN: ConvertItkImageToVtkImageDataTest1"
                  << "------------------------------");
    // trigger error: itkImage (input image) is null

    itk::Image<unsigned int, 3>::Pointer itkImage = NULL;

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertItkImageToVtkImageData(itkImage);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertItkImageToVtkImageDataTest2"
                  << "------------------------------");
  }

  void ConvertItkImageToVtkImageDataTest2()
  {
    vtkDebugMacro("BEGIN: ConvertItkImageToVtkImageDataTest2"
                  << "------------------------------");
    // trigger error: itkImage (input image) is null

    SeedImageType::Pointer itkImage = NULL;

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertItkImageToVtkImageData(itkImage);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertItkImageToVtkImageDataTest2"
                  << "------------------------------");
  }

  void ConvertItkImageToVolumeNodeTest1()
  {
    vtkDebugMacro("BEGIN: ConvertItkImageToVolumeNodeTest1"
                  << "------------------------------");
    // trigger error: itkImage empty pointer

    SeedImageType::Pointer itkImage = NULL;

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertItkImageToVolumeNode(itkImage, false);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertItkImageToVolumeNodeTest2"
                  << "------------------------------");
  }

  void ConvertVtkImageDataToVolumeNodeTest1()
  {
    vtkDebugMacro("BEGIN: ConvertVtkImageDataToVolumeNodeTest1"
                  << "------------------------------");
    // trigger error: Pointer to vtkImageData is NULL

    vtkImageData *vtkData = NULL;
    SeedImageType::Pointer itkImage = NULL;

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    ConvertVtkImageDataToVolumeNode(vtkData, itkImage, false);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: ConvertVtkImageDataToVolumeNodeTest2"
                  << "------------------------------");
  }

protected:
  vtkVesselSegmentationHelperTest(){TESTING_OUTPUT_RESET();}
  ~vtkVesselSegmentationHelperTest(){TESTING_OUTPUT_RESET();}
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkVesselSegmentationHelperTest);

//------------------------------------------------------------------------------
bool loadImageAndTestConversions( const char* volumeName1, vtkSlicerVesselSegmentationLogic* logic );

//------------------------------------------------------------------------------
int vtkVesselSegmentationHelperTest1(int argc, char * argv[]  )
{
  TESTING_OUTPUT_INIT();
  //----------------------------------------------------------------------------
  // UNIT Tests

  vtkSmartPointer<vtkVesselSegmentationHelperTest> logicTest;

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertVolumeNodeToItkImageTest1();

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertVtkImageDataToItkImageTest1();

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertItkImageToVtkImageDataTest1();

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertItkImageToVtkImageDataTest2();

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertItkImageToVolumeNodeTest1();

  logicTest = vtkSmartPointer<vtkVesselSegmentationHelperTest>::New();
  logicTest->DebugOn();
  logicTest->ConvertVtkImageDataToVolumeNodeTest1();

  // END: UNIT Tests
  //----------------------------------------------------------------------------

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

  res = loadImageAndTestConversions(fileName1, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

bool loadImageAndTestConversions( const char* volumeName1, vtkSlicerVesselSegmentationLogic* logic )
{
  std::cout << "inside: testConvertImage" << std::endl;

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

  std::cout << "active volume node = " << activeVol << std::endl;
  if( activeVol == NULL)
  {
    std::cout << "active volume node is null pointer" << std::endl;
    return false;
  }

  vtkVesselSegmentationHelper::SeedImageType::Pointer itkConvertedImage = vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(activeVol);
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

