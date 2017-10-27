
/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLVesselSegmentationSeedNodeTest1.cxx

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
#include "vtkMRMLVesselSegmentationSeedNode.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLNode.h>

// VTK includes
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkCollection.h>
#include <vtkPoints.h>

// STD includes
#include <iostream>

//------------------------------------------------------------------------------
int vtkMRMLVesselSegmentationSeedNodeTest1(int, char *[])
{
  vtkNew<vtkMRMLVesselSegmentationSeedNode> node1;
  vtkNew<vtkMRMLVesselSegmentationSeedNode> node2;

  // Basic node tests
  EXERCISE_ALL_BASIC_MRML_METHODS(node1.GetPointer());

  //----------------------------------------------------------------------------
  // Adding seeds tests
  node1->SetSeed1(1.1,2.2,3.3);

  double *node1_seed1 = node1->GetSeed1();

  if( node1_seed1[0] != 1.1 || node1_seed1[1] != 2.2 || node1_seed1[2] != 3.3 )
  {
    std::cerr << "Seed1 did not match with what was set" << std::endl;
    return EXIT_FAILURE;
  }

  node1->SetSeed2(4.4,5.5,6.6);

  double *node1_seed2 = node1->GetSeed2();

  if( node1_seed2[0] != 4.4 || node1_seed2[1] != 5.5 || node1_seed2[2] != 6.6 )
  {
    std::cerr << "Seed2 did not match with what was set" << std::endl;
    return EXIT_FAILURE;
  }

  // try getting a seed when nothing has been set (should be 0,0,0)
  double *node2_seed1 = node2->GetSeed1();
  //std::cout << "Node not set contains (should be 0,0,0): " << node2_seed1[0] << " " << node2_seed1[1] << " " << node2_seed1[2] << std::endl;

  if( node2_seed1[0] != 0.0 && node2_seed1[1] != 0.0 && node2_seed1[2] != 0.0 )
  {
    std::cerr << "Seed1 was not initialized correctly" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


