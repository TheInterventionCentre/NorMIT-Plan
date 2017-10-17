/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager3DTest1.cxx

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
#include "vtkMRMLResectionDisplayableManager3D.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLViewNode.h>
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
class vtkMRMLResectionDisplayableManager3DTest:
  public vtkMRMLResectionDisplayableManager3D
{
public:
  static vtkMRMLResectionDisplayableManager3DTest* New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager3DTest,
               vtkMRMLResectionDisplayableManager3D);

  void OnMRMLSceneNodeAddedTest1()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest1"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest1"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeAddedTest2()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest2"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest2"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeAddedTest3()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest3"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest3"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeAddedTest4()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest4"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

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
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest4"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeAddedTest5()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest3"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

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
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeAdded(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest5"
                  << "------------------------------");
  }

  void AddWidgetTest1()
  {
    vtkDebugMacro("BEGIN: AddWidgetTest1"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddWidget(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: AddWidgetTest1"
                  << "------------------------------");
  }

  void AddWidgetTest2()
  {
    vtkDebugMacro("BEGIN: AddWidgetTest2"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS(2);
    TESTING_OUTPUT_RESET();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddWidget(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: AddWidgetTest2"
                  << "------------------------------");
  }

  void AddWidgetTest3()
  {
    vtkDebugMacro("BEGIN: AddWidgetTest3"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_RESET();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddWidget(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: AddWidgetTest3"
                  << "------------------------------");
  }

  void AddWidgetTest4()
  {
    vtkDebugMacro("BEGIN: AddWidgetTest4"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

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
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::AddWidget(node);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

    vtkDebugMacro("END: AddWidgetTest4"
                  << "------------------------------");
  }

  void AddWidgetTest5()
  {
    vtkDebugMacro("BEGIN: AddWidgetTest3"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

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
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddWidget(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: AddWidgetTest5"
                  << "------------------------------");
  }

  void SetAndObserveNodeTest1()
  {
    vtkDebugMacro("BEGIN: SetAndObserveNodeTest1"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::SetAndObserveNode(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: SetAndObserveNodeTest1"
                  << "------------------------------");
  }

  void SetAndObserveNodeTest2()
  {
    vtkDebugMacro("BEGIN: SetAndObserveNodeTest2"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::SetAndObserveNode(node);
    vtkDebugMacro("END: SetAndObserveNodeTest2"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeRemovedTest1()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest1"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeRemoved(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest1"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeRemovedTest2()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest2"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->SetRenderer(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest2"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeRemovedTest3()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest3"
                  << "------------------------------");
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

    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest3"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeRemovedTest4()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest4"
                  << "------------------------------");
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

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::OnMRMLSceneNodeRemoved(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest4"
                  << "------------------------------");
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

    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest5"
                  << "------------------------------");
  }

  void UpdateFromMRMLSceneTest1()
  {
    vtkDebugMacro("BEGIN: UpdateFromMRMLSceneTest1"
                  << "------------------------------");
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRMLScene();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

    vtkDebugMacro("END: UpdateFromMRMLSceneTest1"
                  << "------------------------------");
  }

  void UpdateFromMRMLTest1()
  {
    vtkDebugMacro("BEGIN: UpdateFromMRMLTest1"
                  << "------------------------------");
    this->Superclass::SetRenderer(NULL);
    this->Superclass::SetMRMLScene(NULL);
    this->Superclass::SetAndObserveMRMLDisplayableNode(NULL);
    this->Superclass::UpdateFromMRML();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

    vtkDebugMacro("END: UpdateFromMRMLTest1"
                  << "------------------------------");
  }

  void OnMRMLSceneEndCloseTest1()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneEndCloseTest1"
                  << "------------------------------");
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

    vtkDebugMacro("END: OnMRMLSceneEndCloseTest1"
                  << "------------------------------");
  }

  void OnMRMLSceneEndCloseTest2()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneEndCloseTest2"
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

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    scene->AddNode(resectionNode);

    this->Superclass::OnMRMLSceneEndClose();
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

    vtkDebugMacro("END: OnMRMLSceneEndCloseTest2"
                  << "------------------------------");
  }

  void SetSceneInternalTest1()
  {
    vtkDebugMacro("BEGIN: SetSceneInternalTest1"
                  << "------------------------------");
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLSceneInternal(scene);
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

    vtkDebugMacro("END: SetSceneInternalTest1"
                  << "------------------------------");
  }

  void SetSceneInternalTest2()
  {
    vtkDebugMacro("BEGIN: SetSceneInternalTest2"
                  << "------------------------------");
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
    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

    vtkDebugMacro("END: SetSceneInternalTest2"
                  << "------------------------------");
  }


  void ProcessMRMLNodesEventsTest1()
  {
    vtkDebugMacro("BEGIN: ProcessMRMLNodesEvents1"
                  << "------------------------------");

    vtkSmartPointer<vtkMRMLViewNode> node =
      vtkSmartPointer<vtkMRMLViewNode>::New();

    this->Superclass::ProcessMRMLNodesEvents(node,
                                             vtkCommand::ModifiedEvent,
                                             NULL);

    TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

    vtkDebugMacro("END: ProcessMRMLNodesEvents1"
                  << "------------------------------");
  }

  void ProcessMRMLNodesEventsTest2()
  {
    vtkDebugMacro("BEGIN: ProcessMRMLNodesEvents2"
                  << "------------------------------");

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::ProcessMRMLNodesEvents(resectionNode,
                                             vtkCommand::ModifiedEvent,
                                             NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();

    vtkDebugMacro("END: ProcessMRMLNodesEvents2"
                  << "------------------------------");
  }

  void ProcessMRMLNodesEventsTest3()
  {
    vtkDebugMacro("BEGIN: ProcessMRMLNodesEvents3"
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

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

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

    vtkDebugMacro("END: ProcessMRMLNodesEvents3"
                  << "------------------------------");
  }

  void PrintSelfTest1()
  {
    vtkDebugMacro("BEGIN: PrintSelf1"
                  << "------------------------------");

    this->Superclass::PrintSelf(std::cout, vtkIndent(0));

    TESTING_OUTPUT_ASSERT_ERRORS(0);

    vtkDebugMacro("END: PrintSelfTest1"
                  << "------------------------------");
  }

  void UpdateGeometryTest1()
  {
    vtkDebugMacro("BEGIN: UpdateGeometryTest1"
                  << "------------------------------");

    this->Superclass::UpdateGeometry(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS(1);

    vtkDebugMacro("END: UpdateGeometryTest1"
                  << "------------------------------");
  }

  void UpdateGeometryTest2()
  {
    vtkDebugMacro("BEGIN: UpdateGeometryTest2"
                  << "------------------------------");

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::UpdateGeometry(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);

    vtkDebugMacro("END: UpdateGeometryTest2"
                  << "------------------------------");
  }

  void UpdateGeometryTest3()
  {
    vtkDebugMacro("BEGIN: UpdateGeometryTest3"
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

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    scene->AddNode(resectionNode);
    this->Superclass::UpdateGeometry(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);

    vtkDebugMacro("END: UpdateGeometryTest3"
                  << "------------------------------");
  }

    void UpdateGeometryTest4()
  {
    vtkDebugMacro("BEGIN: UpdateGeometryTest4"
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

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
    scene->AddNode(resectionDisplayNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());
    scene->AddNode(resectionNode);
    this->Superclass::UpdateGeometry(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(0);

    vtkDebugMacro("END: UpdateGeometryTest4"
                  << "------------------------------");
  }

  void UpdateVisibilityTest1()
  {
    vtkDebugMacro("BEGIN: UpdateVisibilityTest1"
                  << "------------------------------");

    this->Superclass::UpdateVisibility(NULL);

    TESTING_OUTPUT_ASSERT_ERRORS(1);

    vtkDebugMacro("END: UpdateVisibilityTest1"
                  << "------------------------------");
  }

  void UpdateVisibilityTest2()
  {
    vtkDebugMacro("BEGIN: UpdateVisibilityTest2"
                  << "------------------------------");

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::UpdateVisibility(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(1);

    vtkDebugMacro("END: UpdateVisibilityTest2"
                  << "------------------------------");
  }

  void UpdateVisibilityTest3()
  {
    vtkDebugMacro("BEGIN: UpdateVisibilityTest3"
                  << "------------------------------");

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

    vtkDebugMacro("END: UpdateVisibilityTest3"
                  << "------------------------------");
  }

  void UpdateVisibilityTest4()
  {
    vtkDebugMacro("BEGIN: UpdateVisibilityTest4"
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

    vtkSmartPointer<vtkMRMLInteractionNode> interaction =
      vtkSmartPointer<vtkMRMLInteractionNode>::New();
    scene->AddNode(interaction);

    vtkSmartPointer<vtkMRMLSelectionNode> selection =
      vtkSmartPointer<vtkMRMLSelectionNode>::New();
    scene->AddNode(selection);

    vtkSmartPointer<vtkMRMLViewNode> viewNode =
      vtkSmartPointer<vtkMRMLViewNode>::New();
    scene->AddNode(viewNode);

    this->Superclass::SetRenderer(renderer);
    this->Superclass::SetMRMLScene(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(viewNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();
    scene->AddNode(resectionDisplayNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    resectionNode->SetAndObserveDisplayNodeID(resectionDisplayNode->GetID());
    scene->AddNode(resectionNode);

    this->Superclass::UpdateVisibility(resectionNode);

    TESTING_OUTPUT_ASSERT_ERRORS(0);

    vtkDebugMacro("END: UpdateVisibilityTest4"
                  << "------------------------------");
  }

protected:
  vtkMRMLResectionDisplayableManager3DTest(){TESTING_OUTPUT_RESET();}
  ~vtkMRMLResectionDisplayableManager3DTest(){TESTING_OUTPUT_RESET();}
};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionDisplayableManager3DTest);

//------------------------------------------------------------------------------
int vtkMRMLResectionDisplayableManager3DTest1(int, char *[])
{
  TESTING_OUTPUT_INIT();

  //----------------------------------------------------------------------------
  // UNIT Tests

  vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>
    displayableManagerTest;

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeAddedTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeAddedTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeAddedTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeAddedTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeAddedTest5();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddWidgetTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddWidgetTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddWidgetTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddWidgetTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->AddWidgetTest5();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->SetAndObserveNodeTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->SetAndObserveNodeTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest4();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest5();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateFromMRMLSceneTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateFromMRMLTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneEndCloseTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->OnMRMLSceneEndCloseTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->SetSceneInternalTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->SetSceneInternalTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->ProcessMRMLNodesEventsTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->ProcessMRMLNodesEventsTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->ProcessMRMLNodesEventsTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->PrintSelfTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->GlobalWarningDisplayOn();
  displayableManagerTest->UpdateGeometryTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateGeometryTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateGeometryTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateVisibilityTest1();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateVisibilityTest2();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateVisibilityTest3();

  displayableManagerTest =
    vtkSmartPointer<vtkMRMLResectionDisplayableManager3DTest>::New();
  displayableManagerTest->DebugOn();
  displayableManagerTest->UpdateVisibilityTest4();

  // END: UNIT Tests
  //----------------------------------------------------------------------------

  return EXIT_SUCCESS;
}
