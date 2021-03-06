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
#include <vtkMRMLViewNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLLabelMapVolumeNode.h>

#include <vtkTestingOutputWindow.h>

//------------------------------------------------------------------------------
class vtkSlicerVesselSegmentationLogicTest:
  public vtkSlicerVesselSegmentationLogic
{
public:
  static vtkSlicerVesselSegmentationLogicTest* New();
  vtkTypeMacro(vtkSlicerVesselSegmentationLogicTest,
      vtkSlicerVesselSegmentationLogic);

  void SetMRMLSceneInternalTest1()
  {
    vtkDebugMacro("BEGIN: SetMRMLSceneInternalTest1"
                  << "------------------------------");
    // trigger error:

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLScene(scene);

    // checks we don't already have any errors
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SetMRMLSceneInternal(scene);
    TESTING_OUTPUT_ASSERT_ERRORS(0); // check only have 1 error
    TESTING_OUTPUT_RESET(); // reset to clear errors + warnings
    vtkDebugMacro("END: SetMRMLSceneInternalTest1"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void GetActiveVolumeTest1()
  {
    vtkDebugMacro("BEGIN: GetActiveVolumeTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    // checks you get at least 1 error and resets
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: GetActiveVolumeTest1"
                  << "------------------------------");
  }

  void GetActiveVolumeTest2()
  {
    vtkDebugMacro("BEGIN: GetActiveVolumeTest2"
                  << "------------------------------");
    // trigger error: no valid selection node.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLScene(scene);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    GetActiveVolume();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: GetActiveVolumeTest2"
                  << "------------------------------");
  }

  void GetActiveVolumeTest3()
  {
    vtkDebugMacro("BEGIN: GetActiveVolumeTest3"
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
    vtkDebugMacro("END: GetActiveVolumeTest3"
                  << "------------------------------");
  }

  void GetActiveVolumeTest4()
  {
    vtkDebugMacro("BEGIN: GetActiveVolumeTest4"
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
    vtkDebugMacro("END: GetActiveVolumeTest4"
                  << "------------------------------");
  }

  void GetActiveVolumeTest5()
  {
    vtkDebugMacro("BEGIN: GetActiveVolumeTest5"
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
    vtkDebugMacro("END: GetActiveVolumeTest5"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SetAndPropagateActiveVolumeTest1()
    {
    vtkDebugMacro("BEGIN: SetAndPropagateActiveVolumeTest1"
                  << "------------------------------");
    // trigger error: cannot get mrml app logic

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
    SetAndPropagateActiveVolume(activeVol);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SetAndPropagateActiveVolumeTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void SetAndPropagateActiveLabelTest1()
    {
    vtkDebugMacro("BEGIN: SetAndPropagateActiveLabelTest1"
                  << "------------------------------");
    // trigger error: cannot get mrml app logic

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    vtkSmartPointer<vtkMRMLLabelMapVolumeNode> activeLabel =
      vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();

    this->Superclass::SetMRMLScene(scene);
    scene->AddNode(selectionNode);
    scene->AddNode(activeLabel);
    char* id = activeLabel->GetID();
    selectionNode->SetActiveVolumeID(id);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SetAndPropagateActiveLabel(activeLabel);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SetAndPropagateActiveLabelTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void PreprocessTest1(int lowerThreshold,
                         int upperThreshold,
                         unsigned int alpha,
                         int beta,
                         unsigned int conductance,
                         unsigned int iterations)
  {
    vtkDebugMacro("BEGIN: PreprocessTest1"
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: PreprocessTest1"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SegmentVesselsFromWidgetTest1(bool isHepatic)
  {
    vtkDebugMacro("BEGIN: SegmentVesselsFromWidgetTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVesselsFromWidget(isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SegmentVesselsFromWidgetTest1"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SegmentVesselsFromWidgetTest2(bool isHepatic)
  {
    vtkDebugMacro("BEGIN: SegmentVesselsFromWidgetTest2"
                  << "------------------------------");
    // trigger error: Do not have seed pre-requisites for segmentation.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLScene(scene);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    scene->AddNode(seedNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVesselsFromWidget(isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SegmentVesselsFromWidgetTest2"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SegmentVesselsTest1(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: SegmentVesselsTest1"
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SegmentVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: SegmentVesselsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void MergeLabelMapsTest1()
    {
    vtkDebugMacro("BEGIN: MergeLabelMapsTest1"
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    MergeLabelMaps();
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: MergeLabelMapsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void SplitVesselsFromWidgetTest1(bool isHepatic)
  {
    vtkDebugMacro("BEGIN: SplitVesselsFromWidgetTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SplitVesselsFromWidget(isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SplitVesselsFromWidgetTest1"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SplitVesselsFromWidgetTest2(bool isHepatic)
  {
    vtkDebugMacro("BEGIN: SplitVesselsFromWidgetTest2"
                  << "------------------------------");
    // trigger error: Do not have seed pre-requisites for splitting.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLScene(scene);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
        vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    scene->AddNode(seedNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SplitVesselsFromWidget(isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SplitVesselsFromWidgetTest2"
                  << "------------------------------");
  }

  //------------------------------------------------------------------------------
  void SplitVesselsTest1(bool isHepatic)
    {
    vtkDebugMacro("BEGIN: SplitVesselsTest1"
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    SplitVessels(seedNode, isHepatic);
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: SplitVesselsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void UpdateModelsTest1()
    {
    vtkDebugMacro("BEGIN: UpdateModelsTest1"
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    UpdateModels();
    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();
    vtkDebugMacro("END: UpdateModelsTest1"
                  << "------------------------------");
    }

  //------------------------------------------------------------------------------
  void Reset3DViewTest1()
    {
    vtkDebugMacro("BEGIN: Reset3DViewTest1"
                  << "------------------------------");
    // trigger error: no MRML scene.

    this->Superclass::SetMRMLScene(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    Reset3DView();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: Reset3DViewTest1"
                  << "------------------------------");
    }

    void Reset3DViewTest2()
    {
    vtkDebugMacro("BEGIN: Reset3DViewTest2"
                  << "------------------------------");
    // trigger error: No view node.

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLScene(scene);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    Reset3DView();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: Reset3DViewTest2"
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
  // Particularly error checking of several higher level functions:
  // CallPreprocessing
  // CallSegmentationAlgorithm
  // CallMergeLabelMaps
  // CallAssignSeeds
  // ======================

  vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest> logicTest;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SetMRMLSceneInternalTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->GetActiveVolumeTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->GetActiveVolumeTest2();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->GetActiveVolumeTest3();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->GetActiveVolumeTest4();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->GetActiveVolumeTest5();


  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SetAndPropagateActiveVolumeTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SetAndPropagateActiveLabelTest1();


  // pass in defaults
  int lowerThreshold = 100;
  int upperThreshold = 250;
  unsigned int alpha = 20;
  int beta = 160;
  unsigned int conductance = 20;
  unsigned int iterations = 30;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->PreprocessTest1(lowerThreshold,
                               upperThreshold,
                               alpha,
                               beta,
                               conductance,
                               iterations);

  bool isHepatic = true;

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SegmentVesselsFromWidgetTest1(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SegmentVesselsFromWidgetTest2(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SegmentVesselsTest1(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->MergeLabelMapsTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SplitVesselsFromWidgetTest1(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SplitVesselsFromWidgetTest2(isHepatic);

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->SplitVesselsTest1(isHepatic);


  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->UpdateModelsTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->Reset3DViewTest1();

  logicTest = vtkSmartPointer<vtkSlicerVesselSegmentationLogicTest>::New();
  logicTest->DebugOn();
  logicTest->Reset3DViewTest2();


  // END: UNIT Tests
  //----------------------------------------------------------------------------

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  EXERCISE_BASIC_OBJECT_METHODS(logic.GetPointer());


  return EXIT_SUCCESS;
}
