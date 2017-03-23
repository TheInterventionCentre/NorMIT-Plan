/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionSurfaceDisplayableManager2DTest1.cxx

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

// This module includes
#include "vtkMRMLResectionDisplayableManager2D.h"
#include "vtkMRMLResectionSurfaceNode.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLSliceNode.h>
#include <vtkMRMLApplicationLogic.h>
#include <vtkMRMLDisplayableManagerGroup.h>
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

  void OnMRMLSceneNodeAddedTest1()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest1"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest1"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeAddedTest2()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeAddedTest2"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLSceneInternal(scene);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
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

    vtkSmartPointer<vtkMRMLSliceNode> sliceNode =
      vtkSmartPointer<vtkMRMLSliceNode>::New();
    scene->AddNode(sliceNode);

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    vtkDebugMacro("END: OnMRMLSceneNodeAddedTest3"
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

  void AddRepresentationTest1()
  {
    vtkDebugMacro("BEGIN: AddRepresentationTest1"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddRepresentation(NULL);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: AddRepresentationTest1"
                  << "------------------------------");
  }

  void AddRepresentationTest2()
  {
    vtkDebugMacro("BEGIN: AddRepresentationTest2"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: AddRepresentationTest2"
                  << "------------------------------");
  }

  void AddRepresentationTest3()
  {
    vtkDebugMacro("BEGIN: AddRepresentationTest3"
                  << "------------------------------");
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

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->Superclass::AddRepresentation(node);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
    vtkDebugMacro("END: AddRepresentationTest3"
                  << "------------------------------");
  }

  void AddRepresentationTest4()
  {
    vtkDebugMacro("BEGIN: AddRepresentationTest4"
                      << "------------------------------");
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

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);
    this->Superclass::SetRenderer(renderer);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::AddRepresentation(node);

    vtkDebugMacro("END: AddRepresentationTest4"
                  << "------------------------------");
  }

  void OnMRMLSceneNodeRemovedTest1()
  {
    vtkDebugMacro("BEGIN: OnMRMLSceneNodeRemovedTest1"
                  << "------------------------------");
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    this->SetRenderer(NULL);
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

    this->Superclass::SetRenderer(renderer);
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    this->Superclass::OnMRMLSceneNodeAdded(node);
    this->Superclass::OnMRMLSceneNodeRemoved(node);

    vtkDebugMacro("END: OnMRMLSceneNodeRemovedTest4"
                  << "------------------------------");
  }


protected:
  vtkMRMLResectionDisplayableManager2DTest(){};
  ~vtkMRMLResectionDisplayableManager2DTest(){};

};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionDisplayableManager2DTest);


//------------------------------------------------------------------------------
int vtkMRMLResectionDisplayableManager2DTest1(int, char *[])
{
  //------------------------------------------------------------------------------
  // UNIT tests

  vtkNew<vtkMRMLResectionDisplayableManager2DTest> displayableManagerTest;
  displayableManagerTest->DebugOn();

  displayableManagerTest->OnMRMLSceneNodeAddedTest1();
  displayableManagerTest->OnMRMLSceneNodeAddedTest2();
  displayableManagerTest->OnMRMLSceneNodeAddedTest3();

  displayableManagerTest->SetAndObserveNodeTest1();
  displayableManagerTest->SetAndObserveNodeTest2();

  displayableManagerTest->AddRepresentationTest1();
  displayableManagerTest->AddRepresentationTest2();
  displayableManagerTest->AddRepresentationTest3();
  displayableManagerTest->AddRepresentationTest4();

  displayableManagerTest->OnMRMLSceneNodeRemovedTest1();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest2();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest3();
  displayableManagerTest->OnMRMLSceneNodeRemovedTest4();

  // END: UNIT tests
  //------------------------------------------------------------------------------

  return EXIT_SUCCESS;
}
