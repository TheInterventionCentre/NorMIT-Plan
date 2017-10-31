/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationLogicTest1.cxx

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

#include "vtkSlicerVesselSegmentationLogic.h"
#include "vtkMRMLVesselSegmentationSeedNode.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScalarVolumeNode.h>

#include <vtkTestingOutputWindow.h>

//------------------------------------------------------------------------------
class vtkSlicerVesselSegmentationLogicTest:
  public vtkSlicerVesselSegmentationLogic
{
public:
  static vtkSlicerVesselSegmentationLogicTest* New();
  vtkTypeMacro(vtkSlicerVesselSegmentationLogicTest,
      vtkSlicerVesselSegmentationLogic);

  void OnGetActiveVolumeTest1()
  {
    vtkDebugMacro("BEGIN: OnGetActiveVolumeTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnGetActiveVolumeTest1"
                  << "------------------------------");
  }

  void OnGetActiveVolumeTest2()
  {
    vtkDebugMacro("BEGIN: OnGetActiveVolumeTest2"
                  << "------------------------------");
    // trigger error: no valid selection node.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLScene(scene);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnGetActiveVolumeTest2"
                  << "------------------------------");
  }

  void OnGetActiveVolumeTest3()
  {
    vtkDebugMacro("BEGIN: OnGetActiveVolumeTest3"
                  << "------------------------------");
    // trigger error: no active volume ID.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnGetActiveVolumeTest3"
                  << "------------------------------");
  }

  void OnGetActiveVolumeTest4()
  {
    vtkDebugMacro("BEGIN: OnGetActiveVolumeTest4"
                  << "------------------------------");
    // trigger error: no active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    selectionNode->SetActiveVolumeID("test");

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnGetActiveVolumeTest4"
                  << "------------------------------");
  }

  void OnGetActiveVolumeTest5()
  {
    vtkDebugMacro("BEGIN: OnGetActiveVolumeTest5"
                  << "------------------------------");
    // trigger error: active volume does not have data.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnGetActiveVolumeTest5"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void OnPreprocessTest1(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest1"
                  << "------------------------------");
    // trigger error: could not get active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: OnPreprocessTest1"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void OnSegmentVesselsTest1(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsTest1"
                  << "------------------------------");
    // trigger error: could not get active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: OnSegmentVesselsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void OnMergeLabelMapsTest1()
    {
    vtkDebugMacro("BEGIN: OnMergeLabelMapsTest1"
                  << "------------------------------");
    // trigger error: could not get active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    MergeLabelMaps();
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: OnMergeLabelMapsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void OnSplitVesselsTest1(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSplitVesselsTest1"
                  << "------------------------------");
    // trigger error: could not get active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    SplitVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: OnSplitVesselsTest1"
                  << "------------------------------");
    }


protected:
  vtkSlicerVesselSegmentationLogicTest(){TESTING_OUTPUT_RESET();}
  ~vtkSlicerVesselSegmentationLogicTest(){TESTING_OUTPUT_RESET();}
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVesselSegmentationLogicTest);


//------------------------------------------------------------------------------
int vtkMRMLVesselSegmentationLogicTest1(int , char * [] )
{
  TESTING_OUTPUT_INIT();
  //----------------------------------------------------------------------------
  // UNIT Tests
  // ======================
  // Error checking of several higher level functions:
  // CallPreprocessing
  // CallSegmentationAlgorithm
  // CallMergeLabelMaps
  // CallAssignSeeds
  // ======================

  vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest> logicTest;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnGetActiveVolumeTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnGetActiveVolumeTest2();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnGetActiveVolumeTest3();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnGetActiveVolumeTest4();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnGetActiveVolumeTest5();


  // pass in defaults
  int lowerThreshold = 100;
  int upperThreshold = 250;
  unsigned int alpha = 20;
  int beta = 160;
  unsigned int conductance = 20;
  unsigned int iterations = 30;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnPreprocessTest1(lowerThreshold,
                               upperThreshold,
                               alpha,
                               beta,
                               conductance,
                               iterations);

  bool isHepatic = true;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnSegmentVesselsTest1(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnMergeLabelMapsTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnSplitVesselsTest1(isHepatic);


  // END: UNIT Tests
  //----------------------------------------------------------------------------

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  EXERCISE_BASIC_OBJECT_METHODS(logic.GetPointer());


  return EXIT_SUCCESS;
}
