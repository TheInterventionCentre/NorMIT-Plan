/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionSurfaceNodeTest1.cxx

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
#include "vtkMRMLResectionSurfaceNode.h"
#include "ResectionPlanningModuleDefaultValues.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLModelNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCollection.h>
#include <vtkPoints.h>

// STD includes
#include <iostream>

//------------------------------------------------------------------------------
int vtkMRMLResectionSurfaceNodeTest1(int, char *[])
{
  vtkNew<vtkMRMLResectionSurfaceNode> node1;

  // Basic node tests
  EXERCISE_ALL_BASIC_MRML_METHODS(node1.GetPointer());

  //----------------------------------------------------------------------------
  // Adding target tumor tests

  // Check number of tumors in initialization.
  if (node1->GetNumberOfTargetTumors() != 0)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "zero tumors should be reported at initialization" << std::endl;
    return EXIT_FAILURE;
    }

  //Add target tumor
  vtkSmartPointer<vtkMRMLModelNode> tumor1 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  node1->AddTargetTumor(tumor1);
  if (node1->GetNumberOfTargetTumors() != 1)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "1 tumor should have been reported, "
              << node1->GetNumberOfTargetTumors()
              << " was reported instead." << std::endl;
    return EXIT_FAILURE;
    }

  //Add the same target tumor
  node1->AddTargetTumor(tumor1);
  if (node1->GetNumberOfTargetTumors() != 1)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "1 tumor should have been reported, "
              << node1->GetNumberOfTargetTumors()
              << " was reported instead." << std::endl;
    return EXIT_FAILURE;
    }

  // Add another target tumor
  vtkSmartPointer<vtkMRMLModelNode> tumor2 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  node1->AddTargetTumor(tumor2);
  if (node1->GetNumberOfTargetTumors() != 2)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "2 tumor should have been reported, "
              << node1->GetNumberOfTargetTumors()
              << " was reported instead." << std::endl;
    return EXIT_FAILURE;
    }
  // END: Adding target tumor tests
  //----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  // Removing target tumor tests

  // Removing tumor 1
  node1->RemoveTargetTumor(tumor1);
  if (node1->GetNumberOfTargetTumors() != 1)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "1 tumor should have been reported, "
              << node1->GetNumberOfTargetTumors()
              << " was reported instead." << std::endl;
    return EXIT_FAILURE;
    }

  // Removing tumor 2
  node1->RemoveTargetTumor(tumor2);
  if (node1->GetNumberOfTargetTumors() != 0)
    {
    std::cerr << "Number of target tumors is incorrect: "
              << "0 tumor should have been reported, "
              << node1->GetNumberOfTargetTumors()
              << " was reported instead." << std::endl;
    return EXIT_FAILURE;
    }

  // END: Removing target tumor tests
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // Changing resection margin tests

  // Test resection maring by default.
  if (node1->GetResectionMargin() != DEFAULT_RESECTION_MARGIN)
    {
    std::cerr << "Resection marin in initialization is different "
              << "from the default resection margin" << std::endl;
    return EXIT_FAILURE;
    }

  // Test changin the resection margin.
  node1->SetResectionMargin(22.22);
  if (node1->GetResectionMargin() != 22.22)
    {
    std::cerr << "Resection margin value not expected: "
              << "obtained " << node1->GetResectionMargin()
              << " expected 22.22" << std::endl;
    return EXIT_FAILURE;
    }

  // END: Changing resection margin tests
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // Changing control points

  vtkNew<vtkMRMLResectionSurfaceNode> node2;

  // Test the initialization values
  double startX = -0.5;
  double startY = -0.5;
  double endX = 0.5;
  double endY = 0.5;
  double incX = (endX - startX)/4.0;
  double incY = (endY - startY)/4.0;

  vtkPoints *nodePoints = node2->GetControlPoints();

  for(int i=0; i<4; ++i)
    {
    for(int j=0; j<4; ++j)
      {
      double genPoint[3] = {startX+i*incX, startY+j*incY, 0.0};
      double nodePoint[3];
      nodePoints->GetPoint(i*4+j,nodePoint);
      if (genPoint[0] != nodePoint[0] ||
          genPoint[1] != nodePoint[1] ||
          genPoint[2] != nodePoint[2])
        {
        std::cerr << "Initialization points in node are different "
                  << "from the expected points" << std::endl;
        return EXIT_FAILURE;
        }
      }
    }

  // Test changing the control points
  vtkSmartPointer<vtkPoints> genPoints =
    vtkSmartPointer<vtkPoints>::New();

  for(int i=0; i<16; ++i)
    {
    genPoints->InsertNextPoint(i,i,i);
    }

  node2->SetControlPoints(genPoints);

  nodePoints = node2->GetControlPoints();
  for(int i=0; i<16; ++i)
    {
    double nodePoint[3];
    nodePoints->GetPoint(i,nodePoint);

    if (nodePoint[0] != (double)i ||
        nodePoint[1] != (double)i ||
        nodePoint[2] != (double)i)
      {
      std::cerr << "Error setting new control points" << std::endl;
      return EXIT_FAILURE;
      }
    }
  // END: Changing control points
  //----------------------------------------------------------------------------

  return EXIT_SUCCESS;
}
