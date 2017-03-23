/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager2DTest1.cxx

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
#include "vtkMRMLResectionDisplayableManager2D.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLInteractionNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkTestingOutputWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkObjectFactory.h>

//------------------------------------------------------------------------------
class vtkMRMLResectionDisplayableManager2DTest:
  public vtkMRMLResectionDisplayableManager2D
{
public:
  static vtkMRMLResectionDisplayableManager2DTest* New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager2DTest,
               vtkMRMLResectionDisplayableManager2D);

  void AddRepresentationTest1()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void AddRepresentationTest2()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    scene->AddNode(sliceNode);

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void AddRepresentationTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

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

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void AddRepresentationTest4()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

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

    this->Superclass::AddRepresentation(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void AddRepresentationTest5()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

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

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::AddRepresentation(resectionNode);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

  void OnMRMLSceneNodeAddedTest1()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void OnMRMLSceneNodeAddedTest2()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);


    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void OnMRMLSceneNodeAddedTest3()
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

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void OnMRMLSceneNodeAddedTest4()
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

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeAdded(node);

    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
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

    this->Superclass::OnMRMLSceneNodeAdded(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

  void SetAndObserveNodeTest1()
  {
    this->Superclass::SetAndObserveNode(NULL);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void SetAndObserveNodeTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::SetAndObserveNode(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void OnMRMLSceneNodeRemovedTest1()
  {
    this->Superclass::OnMRMLSceneNodeRemoved(NULL);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
  }

  void OnMRMLSceneNodeRemovedTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->SetRenderer(NULL);
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
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

    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);
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

    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();


    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void OnMRMLSceneNodeRemovedTest5()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest5"
                  << "------------------------------");
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

    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeAdded(node);
    this->Superclass::OnMRMLSceneNodeRemoved(node);

    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void UpdateFromMRMLSceneTest1()
  {
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRMLScene();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void UpdateFromMRMLTest1()
  {
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRML();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  }

  void OnMRMLSceneEndCloseTest1()
  {
    vtkSmartPointer<vtkRenderer> renderer =
      vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow =
      vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
      vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    this->Superclass::OnMRMLSceneEndClose();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
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
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    scene->AddNode(resectionNode);

    this->Superclass::OnMRMLSceneEndClose();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void SetSceneInternalTest1()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLSceneInternal(scene);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void ProcessMRMLNodesEventsTest1()
  {
    vtkSmartPointer<vtkMRMLSliceNode> node =
      vtkSmartPointer<vtkMRMLSliceNode>::New();

    this->Superclass::ProcessMRMLNodesEvents(node,
                                             vtkCommand::ModifiedEvent,
                                             NULL);

    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);
  }

  void ProcessMRMLNodesEventsTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::ProcessMRMLNodesEvents(resectionNode,
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
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
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
    scene->AddNode(resectionDisplayNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());
    scene->AddNode(resectionNode);

    this->Superclass::ProcessMRMLNodesEvents(resectionNode,
                                             vtkCommand::StartInteractionEvent,
                                             NULL);

    this->Superclass::ProcessMRMLNodesEvents(resectionNode,
                                             vtkCommand::ModifiedEvent,
                                             NULL);

    this->Superclass::ProcessMRMLNodesEvents(resectionNode,
                                             vtkMRMLDisplayableNode::
                                             DisplayModifiedEvent,
                                             NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

  void PrintSelfTest1()
  {
    this->Superclass::PrintSelf(std::cout, vtkIndent(0));

    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

  void UpdateGeometryTest1()
  {
    this->Superclass::UpdateGeometry(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void UpdateGeometryTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::UpdateGeometry(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);
 }

  void UpdateGeometryTest3()
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
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    scene->AddNode(resectionNode);
    this->Superclass::UpdateGeometry(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

  void UpdateVisibilityTest1()
  {
    this->Superclass::UpdateVisibility(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void UpdateVisibilityTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::UpdateVisibility(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void UpdateVisibilityTest3()
  {
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
    scene->AddNode(resectionDisplayNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());

    this->Superclass::UpdateVisibility(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);
  }

  void UpdateVisibilityTest4()
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
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
    scene->AddNode(resectionDisplayNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());
    scene->AddNode(resectionNode);

    this->Superclass::UpdateVisibility(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(0);
  }

protected:
  vtkMRMLResectionDisplayableManager2DTest(){TESTING_OUTPUT_RESET();}
  ~vtkMRMLResectionDisplayableManager2DTest(){TESTING_OUTPUT_RESET();}
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionDisplayableManager2DTest);

//------------------------------------------------------------------------------
int vtkMRMLResectionDisplayableManager2DTest1(int, char *[])
{
  TESTING_OUTPUT_INIT();

  //----------------------------------------------------------------------------
  // UNIT Tests

  vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>
    displayableManagerTest;

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddRepresentationTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddRepresentationTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddRepresentationTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddRepresentationTest4();

    displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddRepresentationTest5();


  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeAddedTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeAddedTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeAddedTest3();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeAddedTest4();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeAddedTest5();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->SetAndObserveNodeTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->SetAndObserveNodeTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeRemovedTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeRemovedTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeRemovedTest3();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeRemovedTest4();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneNodeRemovedTest5();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateFromMRMLSceneTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateFromMRMLTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneEndCloseTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->OnMRMLSceneEndCloseTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->SetSceneInternalTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->ProcessMRMLNodesEventsTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->ProcessMRMLNodesEventsTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->ProcessMRMLNodesEventsTest3();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->PrintSelfTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateGeometryTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateGeometryTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateGeometryTest3();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateVisibilityTest1();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateVisibilityTest2();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateVisibilityTest3();

  // displayableManagerTest =
  //   vtkSmartPointer<vtkMRMLResectionDisplayableManager2DTest>::New();
  // displayableManagerTest->DebugOn();
  // displayableManagerTest->UpdateVisibilityTest4();

  // END: UNIT Tests
  //----------------------------------------------------------------------------

  return EXIT_SUCCESS;

}
