/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationDisplayableManagerTest1.cxx

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

// This modules includes
#include "vtkMRMLVesselSegmentationDisplayableManager2D.h"
#include "vtkMRMLVesselSegmentationSeedNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLDisplayableNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkTestingOutputWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>

//------------------------------------------------------------------------------
class vtkMRMLVesselSegmentationDisplayableManager2DTest:
  public vtkMRMLVesselSegmentationDisplayableManager2D
{
public:
  static vtkMRMLVesselSegmentationDisplayableManager2DTest* New();
  vtkTypeMacro(vtkMRMLVesselSegmentationDisplayableManager2DTest,
      vtkMRMLVesselSegmentationDisplayableManager2D);

  void AddRepresentationTest1()
  {
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(NULL);
    // expect error: no seed node passed
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void AddRepresentationTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(node);
    // expect error: no slice node
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void AddRepresentationTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(sliceNode);
    scene->AddNode(selectionNode);
    scene->AddNode(interactionNode);

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(node);
    // expect error: no renderer
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void AddRepresentationTest4()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(sliceNode);
    scene->AddNode(selectionNode);
    scene->AddNode(interactionNode);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    double *pos = new double[3];
    pos[0] = 1;
    pos[1] = 1;
    pos[2] = 1;
    node->SetCurrentSeedState(1);
    node->SetSeed1(pos);
    scene->AddNode(node);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest1()
  {
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expect error: no node passed
    this->Superclass::OnMRMLSceneNodeAdded(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expected error: no MRML scene has been set
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(selectionNode);
    scene->AddNode(interactionNode);

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expected error: no renderer
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest4()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(selectionNode);
    scene->AddNode(interactionNode);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expected error: no slice node present
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest5()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void SetAndObserveResectionNodeTest1()
  {
    TESTING_OUTPUT_RESET();
    this->Superclass::SetAndObserveSeedNode(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void SetAndObserveResectionNodeTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    TESTING_OUTPUT_RESET();
    this->Superclass::SetAndObserveSeedNode(node);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void SetAndObserveSliceNodeTest1()
  {
    TESTING_OUTPUT_RESET();
    this->Superclass::SetAndObserveSliceNode(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void SetAndObserveSliceNodeTest2()
  {
    vtkSmartPointer<vtkMRMLSliceNode> node =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    TESTING_OUTPUT_RESET();
    this->Superclass::SetAndObserveSliceNode(node);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest1()
  {
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    this->SetRenderer(NULL);
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest3()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest4()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void UpdateFromMRMLTest1()
  {
    TESTING_OUTPUT_RESET();
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRML();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
  }

  void UpdateFromMRMLTest2()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode);

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    TESTING_OUTPUT_RESET();
    this->Superclass::UpdateFromMRML();
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void UpdateFromMRMLSceneTest1()
  {
    TESTING_OUTPUT_RESET();
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRMLScene();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
  }

  void UpdateFromMRMLSceneTest2()
  {
    TESTING_OUTPUT_RESET();
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRMLScene();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
  }

  void UpdateFromMRMLSceneTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    TESTING_OUTPUT_RESET();
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    this->Superclass::UpdateFromMRMLScene();
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneEndCloseTest1()
  {
    TESTING_OUTPUT_RESET();
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::OnMRMLSceneEndClose();
    TESTING_OUTPUT_ASSERT_ERRORS_END();
  }

  void OnMRMLSceneEndCloseTest2()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    scene->AddNode(seedNode);

    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneEndClose();
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void SetMRMLSceneInternalTest1()
  {
    vtkSmartPointer<vtkMRMLScene> mrmlScene =
      vtkSmartPointer<vtkMRMLScene>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::SetMRMLSceneInternal(mrmlScene);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void ProcessMRMLNodesEventsTest1()
  {

    vtkSmartPointer<vtkMRMLSliceNode> node =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::ProcessMRMLNodesEvents(node,
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void ProcessMRMLNodesEventsTest2()
  {

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::ProcessMRMLNodesEvents(seedNode,
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    // expect error: Seed node is not currently handled by the displayable manager
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void ProcessMRMLNodesEventsTest3()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    double *pos = new double[3];
    pos[0] = 1;
    pos[1] = 1;
    pos[2] = 1;
    seedNode->SetCurrentSeedState(1);
    seedNode->SetSeed1(pos);
    scene->AddNode(seedNode);

    this->Superclass::AddRepresentation(seedNode);

    TESTING_OUTPUT_RESET();
    this->Superclass::ProcessMRMLNodesEvents(seedNode,
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }

  void PrintSelfTest1()
  {
    TESTING_OUTPUT_RESET();

    this->Superclass::PrintSelf(std::cout, vtkIndent(0));

    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
  }


protected:
  vtkMRMLVesselSegmentationDisplayableManager2DTest(){}
  ~vtkMRMLVesselSegmentationDisplayableManager2DTest(){}
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLVesselSegmentationDisplayableManager2DTest);

//------------------------------------------------------------------------------
int vtkMRMLVesselSegmentationDisplayableManager2DTest1(int, char *[])
{
  //----------------------------------------------------------------------------
  // UNIT Tests

  TESTING_OUTPUT_INIT();

  vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>
    displayableManagerTest;

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->AddRepresentationTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->AddRepresentationTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->AddRepresentationTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->AddRepresentationTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeAddedTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeAddedTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeAddedTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeAddedTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeAddedTest5();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->SetAndObserveResectionNodeTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->SetAndObserveResectionNodeTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->SetAndObserveSliceNodeTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->SetAndObserveSliceNodeTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateFromMRMLTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateFromMRMLTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateFromMRMLSceneTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateFromMRMLSceneTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneEndCloseTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->OnMRMLSceneEndCloseTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->SetMRMLSceneInternalTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->ProcessMRMLNodesEventsTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->ProcessMRMLNodesEventsTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->ProcessMRMLNodesEventsTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->PrintSelfTest1();

  // END: UNIT Tests
  //----------------------------------------------------------------------------

  TESTING_OUTPUT_RESET();

  return EXIT_SUCCESS;

}




