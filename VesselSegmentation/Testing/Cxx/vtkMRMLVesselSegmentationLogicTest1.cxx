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

  void OnPreprocessTest1(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnPreprocessTest1"
                  << "------------------------------");
  }

  void OnPreprocessTest2(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest2"
                  << "------------------------------");
    // trigger error: no valid selection node.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLScene(scene);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnPreprocessTest2"
                  << "------------------------------");
  }

  void OnPreprocessTest3(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest3"
                  << "------------------------------");
    // trigger error: no active volume ID.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnPreprocessTest3"
                  << "------------------------------");
  }

  void OnPreprocessTest4(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest4"
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
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnPreprocessTest4"
                  << "------------------------------");
  }

  void OnPreprocessTest5(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: OnPreprocessTest5"
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
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnPreprocessTest5"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void OnSegmentVesselsTest1(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsFromWidgetTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnSegmentVesselsFromWidgetTest1"
                  << "------------------------------");
    }

  void OnSegmentVesselsTest2(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsFromWidgetTest2"
                  << "------------------------------");
    // trigger error: no valid selection node.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetMRMLScene(scene);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnSegmentVesselsFromWidgetTest2"
                  << "------------------------------");
    }

  void OnSegmentVesselsTest3(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsFromWidgetTest3"
                  << "------------------------------");
    // trigger error: no active volume ID.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnSegmentVesselsFromWidgetTest3"
                  << "------------------------------");
    }

  void OnSegmentVesselsTest4(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsFromWidgetTest4"
                  << "------------------------------");
    // trigger error: no active volume.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    selectionNode->SetActiveVolumeID("test");

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnSegmentVesselsFromWidgetTest4"
                  << "------------------------------");
    }

  void OnSegmentVesselsTest5(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: OnSegmentVesselsFromWidgetTest5"
                  << "------------------------------");
    // trigger error: active volume does not have data.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol =
      vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeVol);
    char* id = activeVol->GetID();
    selectionNode->SetActiveVolumeID(id);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnSegmentVesselsFromWidgetTest5"
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

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnPreprocessTest2(lowerThreshold,
                               upperThreshold,
                               alpha,
                               beta,
                               conductance,
                               iterations);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnPreprocessTest3(lowerThreshold,
                               upperThreshold,
                               alpha,
                               beta,
                               conductance,
                               iterations);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnPreprocessTest4(lowerThreshold,
                               upperThreshold,
                               alpha,
                               beta,
                               conductance,
                               iterations);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnPreprocessTest5(lowerThreshold,
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
  logicTest->OnSegmentVesselsTest2(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnSegmentVesselsTest3(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnSegmentVesselsTest4(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->OnSegmentVesselsTest5(isHepatic);

  // END: UNIT Tests
  //----------------------------------------------------------------------------

  /*
  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have an image
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->SegmentVesselsFromWidget(true); // isHepatic = true
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have an image
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->MergeLabelMaps();
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have both label maps
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->SplitVesselsFromWidget(true); // isHepatic = true
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have a seed
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  */

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  EXERCISE_BASIC_OBJECT_METHODS(logic.GetPointer());


  return EXIT_SUCCESS;
}
