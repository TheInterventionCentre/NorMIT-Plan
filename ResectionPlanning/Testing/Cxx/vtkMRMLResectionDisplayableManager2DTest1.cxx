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
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
  }

  void OnMRMLSceneNodeAddedTest2()
  {
    TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
    vtkSmartPointer<vtkMRMLScene> scene =
      vtkSmartPointer<vtkMRMLScene>::New();
    this->Superclass::SetMRMLSceneInternal(scene);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::OnMRMLSceneNodeAdded(node);
    TESTING_OUTPUT_ASSERT_ERRORS(1);
    TESTING_OUTPUT_ASSERT_ERRORS_END();
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

    this->Superclass::SetMRMLSceneInternal(scene);
    this->Superclass::SetAndObserveMRMLDisplayableNode(sliceNode);

    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
    this->Superclass::OnMRMLSceneNodeAdded(node);
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
  vtkNew<vtkMRMLResectionDisplayableManager2DTest> displayableManagerTest;
  displayableManagerTest->OnMRMLSceneNodeAddedTest1();
  displayableManagerTest->OnMRMLSceneNodeAddedTest2();
  displayableManagerTest->OnMRMLSceneNodeAddedTest3();


//----------------------------------------------------------------------------
  // Test setting MRML scene

  vtkNew<vtkMRMLScene> scene1;
  vtkNew<vtkMRMLResectionDisplayableManager2D> dm2d1;

  dm2d1->SetMRMLScene(0);
  if (dm2d1->GetMRMLScene())
    {
    std::cerr << "Error setting null MRML scene. "
              << "Expected null pointer" << std::endl;
    return EXIT_FAILURE;
    }

  dm2d1->SetMRMLScene(scene1.GetPointer());
  if (dm2d1->GetMRMLScene() != scene1.GetPointer())
    {
    std::cerr << "Error setting MRML scene. "
              << "Expected pointer differs from the one passed" << std::endl;
    return EXIT_FAILURE;
    }

  // END: Test setting MRML scene
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // Test adding a new node to the scene with no slice view

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();

  vtkSmartPointer<vtkMRMLScene> scene_2 =
    vtkSmartPointer<vtkMRMLScene>::New();

  vtkNew<vtkMRMLResectionSurfaceNode> resectionNode1;
  vtkNew<vtkMRMLResectionDisplayableManager2D> dm2d2;
  dm2d2->DebugOn();
  dm2d2->SetMRMLScene(scene_2);
  scene_2->AddNode(resectionNode1.GetPointer());

  TESTING_OUTPUT_ASSERT_ERRORS(1);
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  // END: Teset adding a new node to the scene with no slice view
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // Test adding a new resection node to the scene

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;

  renderWindow->SetSize(600, 600);
  renderWindow->SetMultiSamples(0);
  renderWindow->AddRenderer(renderer.GetPointer());
  renderWindow->SetInteractor(renderWindowInteractor.GetPointer());

  vtkSmartPointer<vtkMRMLScene> scene =
    vtkSmartPointer<vtkMRMLScene>::New();

  vtkSmartPointer<vtkMRMLApplicationLogic> applicationLogic =
    vtkSmartPointer<vtkMRMLApplicationLogic>::New();
  applicationLogic->SetMRMLScene(scene);

  // Add slice node
  vtkNew<vtkMRMLSliceNode> viewNode;
  scene->AddNode(viewNode.GetPointer());

  vtkNew<vtkMRMLDisplayableManagerGroup> displayableManagerGroup;
  displayableManagerGroup->SetRenderer(renderer.GetPointer());
  displayableManagerGroup->SetMRMLDisplayableNode(viewNode.GetPointer());

  vtkNew<vtkMRMLResectionDisplayableManager2D> resectionDM2D;
  resectionDM2D->SetMRMLApplicationLogic(applicationLogic.GetPointer());
  displayableManagerGroup->AddDisplayableManager(resectionDM2D.GetPointer());

  vtkNew<vtkMRMLResectionSurfaceNode> resectionNode;
  scene->AddNode(resectionNode.GetPointer());

  // END: test adding a n ew node to the scene with no slice view
  //----------------------------------------------------------------------------


  return EXIT_SUCCESS;
}
