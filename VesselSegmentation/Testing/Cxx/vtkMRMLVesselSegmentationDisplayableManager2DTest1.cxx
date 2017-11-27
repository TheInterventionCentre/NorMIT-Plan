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
    std::cout << "AddRepresentationTest1, expect: no seed node passed" << std::endl;
  }

  void AddRepresentationTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(node.GetPointer());
    // expect error: no slice node
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
    std::cout << "AddRepresentationTest2, expect: no slice node" << std::endl;
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

    scene->AddNode(sliceNode.GetPointer());
    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::AddRepresentation(node.GetPointer());
    // expect error: no renderer
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
    std::cout << "AddRepresentationTest3, expect: no renderer" << std::endl;
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

    scene->AddNode(sliceNode.GetPointer());
    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    node->SetSeed1(1,1,1);
    //scene->AddNode(node.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    TESTING_OUTPUT_RESET();
    bool test = this->Superclass::AddRepresentation(node.GetPointer());
    std::cout << "AddRepresentationTest4 - boolean: " << test << std::endl;
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
    std::cout << "AddRepresentationTest4, expect: no errors" << std::endl;
  }

  void UpdateVisibilityOnSliceTest1()
  {
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_RESET();
    this->Superclass::UpdateVisibilityOnSlice(NULL);
    // expect error: no seed node passed
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
    std::cout << "UpdateVisibilityOnSliceTest1, expect: no seed node passed" << std::endl;
  }

  void UpdateVisibilityOnSliceTest2()
  {
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_RESET();
    this->Superclass::UpdateVisibilityOnSlice(node.GetPointer());
    // expect error: no slice node
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
    std::cout << "UpdateVisibilityOnSliceTest2, expect: no slice node" << std::endl;
  }

  void UpdateVisibilityOnSliceTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(sliceNode.GetPointer());
    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::UpdateVisibilityOnSlice(node.GetPointer());
    // expect error: Seed node is not currently handled by the displayable manager
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
    std::cout << "AddRepresentationTest3, expect: Seed node is not currently... " << std::endl;
  }

  void UpdateVisibilityOnSliceTest4()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    vtkSmartPointer<vtkMRMLSelectionNode> selectionNode =
        vtkSmartPointer<vtkMRMLSelectionNode>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interactionNode =
        vtkSmartPointer<vtkMRMLInteractionNode>::New();

    scene->AddNode(sliceNode.GetPointer());
    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    node->SetSeed1(1,1,1);
    //scene->AddNode(node.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    this->Superclass::AddRepresentation(node.GetPointer());

    TESTING_OUTPUT_RESET();
    this->Superclass::UpdateVisibilityOnSlice(node.GetPointer());
    TESTING_OUTPUT_ASSERT_ERRORS(0);
    TESTING_OUTPUT_RESET();
    std::cout << "UpdateVisibilityOnSliceTest4, expect: no errors" << std::endl;
  }

  void OnMRMLSceneNodeAddedTest1()
  {
    std::cout << "OnMRMLSceneNodeAddedTest1 " << std::endl;
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
    this->Superclass::OnMRMLSceneNodeAdded(node.GetPointer());
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

    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expected error: no renderer
    this->Superclass::OnMRMLSceneNodeAdded(node.GetPointer());
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

    scene->AddNode(selectionNode.GetPointer());
    scene->AddNode(interactionNode.GetPointer());

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    // expected error: no slice node present
    this->Superclass::OnMRMLSceneNodeAdded(node.GetPointer());
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeAddedTest5()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction.GetPointer());

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection.GetPointer());

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeAdded(node.GetPointer());
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
    this->Superclass::SetAndObserveSeedNode(node.GetPointer());
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
    this->Superclass::SetAndObserveSliceNode(node.GetPointer());
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
    this->Superclass::OnMRMLSceneNodeRemoved(node.GetPointer());
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest3()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(node.GetPointer());
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void OnMRMLSceneNodeRemovedTest4()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

    this->Superclass::GlobalWarningDisplayOn();
    TESTING_OUTPUT_RESET();
    this->Superclass::OnMRMLSceneNodeRemoved(node.GetPointer());
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
    scene->AddNode(interaction.GetPointer());

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection.GetPointer());

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

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
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
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
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
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
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction.GetPointer());

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection.GetPointer());

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    scene->AddNode(seedNode.GetPointer());

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
    this->Superclass::ProcessMRMLNodesEvents(node.GetPointer(),
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
    this->Superclass::ProcessMRMLNodesEvents(seedNode.GetPointer(),
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    // expect error: Seed node is not currently handled by the displayable manager
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();
  }

  void ProcessMRMLNodesEventsTest3()
  {
    std::cout << "ProcessMRMLNodesEventsTest3" << std::endl;

    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer.GetPointer());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());

    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction.GetPointer());

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection.GetPointer());

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode.GetPointer());

    this->Superclass::SetRenderer(renderer.GetPointer());
    this->Superclass::SetMRMLSceneInternal(scene.GetPointer());
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode.GetPointer());

    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
      vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
    seedNode->SetSeed1(1,1,1);
    //scene->AddNode(seedNode.GetPointer());

    this->Superclass::AddRepresentation(seedNode.GetPointer());

    TESTING_OUTPUT_RESET();
    this->Superclass::ProcessMRMLNodesEvents(seedNode.GetPointer(),
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

  /*
  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->AddRepresentationTest4();
  */

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateVisibilityOnSliceTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateVisibilityOnSliceTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateVisibilityOnSliceTest3();

  /*
  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->UpdateVisibilityOnSliceTest4();
  */

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

  /*
  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->ProcessMRMLNodesEventsTest3();
  */

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLVesselSegmentationDisplayableManager2DTest>::New();
  displayableManagerTest->PrintSelfTest1();

  // END: UNIT Tests
  //----------------------------------------------------------------------------

  TESTING_OUTPUT_RESET();

  return EXIT_SUCCESS;

}




