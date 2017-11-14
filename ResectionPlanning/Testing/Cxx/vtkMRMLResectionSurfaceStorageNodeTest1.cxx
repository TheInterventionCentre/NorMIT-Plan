/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionSurfaceStorageNodeTest1.cxx

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
#include "vtkMRMLResectionSurfaceStorageNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"
#include "vtkMRMLResectionSurfaceStorageNodeTest1Config.h"
#include "vtkMRMLResectionSurfaceNode.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkObjectFactory.h>
#include <vtkPlaneSource.h>
#include <vtkNew.h>

// STD includes
#include <string>
#include <sstream>

//------------------------------------------------------------------------------
class vtkMRMLResectionSurfaceStorageNodeTest:
  public vtkMRMLResectionSurfaceStorageNode
{
public:
  static vtkMRMLResectionSurfaceStorageNodeTest *New();
  vtkTypeMacro(vtkMRMLResectionSurfaceStorageNodeTest,
               vtkMRMLResectionSurfaceStorageNode);


  //----------------------------------------------------------------------------
  void CanReadInReferenceNodeTest1()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    if (!this->CanReadInReferenceNode(resection))
      {
      vtkErrorMacro("Error in CanReadInReferenceNode");
      }
  }

  //----------------------------------------------------------------------------
  void CanReadInReferenceNodeTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplay =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();

    if (!this->CanReadInReferenceNode(resectionDisplay))
      {
      vtkErrorMacro("Error in CanReadInReferenceNode");
      }
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest1()
  {
    // Calling with no node
    this->ReadDataInternal(NULL);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> resectionDisplay =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();

    // Calling with node of wrong type
    this->ReadDataInternal(resectionDisplay);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest3()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // No filename set!

    // Calling with right type of node
    this->ReadDataInternal(resection);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest4()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // No filename set!
    this->SetFileName(NULL);

    // Calling with right type of node
    this->ReadDataInternal(resection);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest5()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // Fake filename
    this->SetFileName("AFileThatShouldNotExists.vtk");

    // Calling with right type of node
    this->ReadDataInternal(resection);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest6()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // Correct filename (contains wrong data)
    this->SetFileName(SAMPLE_MODEL_FILE);

    // Calling with right type of node
    this->ReadDataInternal(resection);
  }

  //----------------------------------------------------------------------------
  void ReadDataInternalTest7()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> resection =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // Correct filename (contains wrong data)
    this->SetFileName(RESECTION_SURFACE_FILE);

    // Calling with right type of node
    this->ReadDataInternal(resection);
  }


  //----------------------------------------------------------------------------
  void WriteDataInternalTest1()
  {
    // Call with no reference node
    this->WriteDataInternal(NULL);
  }

  //----------------------------------------------------------------------------
  void WriteDataInternalTest2()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode> displayNode =
      vtkSmartPointer<vtkMRMLResectionSurfaceDisplayNode>::New();

    // Call with wrong type of node
    this->WriteDataInternal(displayNode);
  }

  //----------------------------------------------------------------------------
  void WriteDataInternalTest3()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    // Call with node not containing any polydata
    this->WriteDataInternal(node);
  }

  //----------------------------------------------------------------------------
  void WriteDataInternalTest4()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    vtkSmartPointer<vtkPlaneSource> planeSource =
      vtkSmartPointer<vtkPlaneSource>::New();
    planeSource->SetXResolution(3);
    planeSource->SetYResolution(3);

    node->SetPolyDataConnection(planeSource->GetOutputPort());

    // Call with node not containing any polydata
    this->WriteDataInternal(node);
  }

  //----------------------------------------------------------------------------
  void WriteDataInternalTest5()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    vtkSmartPointer<vtkPlaneSource> planeSource =
      vtkSmartPointer<vtkPlaneSource>::New();
    planeSource->SetXResolution(3);
    planeSource->SetYResolution(3);

    node->SetPolyDataConnection(planeSource->GetOutputPort());

    // Set a fake name
    this->SetFileName("/this/should/not/exist.vtk");

    // Call with node not containing any polydata
    this->WriteDataInternal(node);
  }


  //----------------------------------------------------------------------------
  void WriteDataInternalTest6()
  {
    vtkSmartPointer<vtkMRMLResectionSurfaceNode> node =
      vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();

    vtkSmartPointer<vtkPlaneSource> planeSource =
      vtkSmartPointer<vtkPlaneSource>::New();
    planeSource->SetXResolution(3);
    planeSource->SetYResolution(3);
    planeSource->Update();

    node->SetPolyDataConnection(planeSource->GetOutputPort());
    node->SetControlPoints(planeSource->GetOutput()->GetPoints());

    std::stringstream stream;
    stream << TEMP_DIR << "/" << "WriteDataInternalTest5OutputResection.vtk";

    // Set correct file name
    this->SetFileName(stream.str().c_str());

    // Call with node not containing any polydata
    this->WriteDataInternal(node);
  }

  //----------------------------------------------------------------------------
  void InitializeSupportedReadFileTypesTest1()
  {
    this->InitializeSupportedReadFileTypes();
  }

  //----------------------------------------------------------------------------
  void InitializeSupportedWriteFileTypesTest1()
  {
    this->InitializeSupportedWriteFileTypes();
  }

protected:
  vtkMRMLResectionSurfaceStorageNodeTest(){};
  ~vtkMRMLResectionSurfaceStorageNodeTest(){};

};

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLResectionSurfaceStorageNodeTest);

//------------------------------------------------------------------------------
int vtkMRMLResectionSurfaceStorageNodeTest1(int, char *[])
{
  vtkNew<vtkMRMLResectionSurfaceStorageNodeTest> nodeTest1;

  // Basic node tests
  EXERCISE_ALL_BASIC_MRML_METHODS(nodeTest1.GetPointer());

  TESTING_OUTPUT_INIT();

  //----------------------------------------------------------------------------
  // CanInReferenceNode tests

  nodeTest1->CanReadInReferenceNodeTest1();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  nodeTest1->CanReadInReferenceNodeTest2();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  // End: CanInReferenceNode tests
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // ReadDataInternal

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest1();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest2();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest3();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(2);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest4();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(2);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest5();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(2);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest6();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest1->ReadDataInternalTest7();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  // End: ReadDataInternal
  //---------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // WriteDataInternal

  vtkNew<vtkMRMLResectionSurfaceStorageNodeTest> nodeTest2;

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest1();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest2();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest3();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest4();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest5();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(1);

  TESTING_OUTPUT_RESET();
  nodeTest2->WriteDataInternalTest6();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  // End: WriteDataInternal
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // InitializeSupportedReadFileTypes tests

  vtkNew<vtkMRMLResectionSurfaceStorageNodeTest> nodeTest3;

  TESTING_OUTPUT_RESET();

  nodeTest3->InitializeSupportedReadFileTypesTest1();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  // End: InitializeSupportedReadFileTypes tests
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // InitializeSupportedWriteFileTypes tests

  vtkNew<vtkMRMLResectionSurfaceStorageNodeTest> nodeTest4;

  TESTING_OUTPUT_RESET();

  nodeTest4->InitializeSupportedWriteFileTypesTest1();
  TESTING_OUTPUT_ASSERT_WARNINGS_ERRORS(0);

  // End: InitializeSupportedWriteFileTypes tests
  //----------------------------------------------------------------------------

  return EXIT_SUCCESS;
}
