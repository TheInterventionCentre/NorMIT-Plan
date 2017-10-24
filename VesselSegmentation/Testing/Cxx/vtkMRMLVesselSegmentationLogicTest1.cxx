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

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLSelectionNode.h>

#include <vtkTestingOutputWindow.h>

int vtkMRMLVesselSegmentationLogicTest1(int , char * [] )
{
  // ======================
  // Basic Setup
  // ======================

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  EXERCISE_BASIC_OBJECT_METHODS(logic.GetPointer());

  // ======================
  // Check error checking of several higher level functions
  // CallPreprocessing
  // CallSegmentationAlgorithm
  // CallMergeLabelMaps
  // CallAssignSeeds
  // ======================

  // pass in defaults
  int lowerThreshold = 100;
  int upperThreshold = 250;
  unsigned int alpha = 20;
  int beta = 160;
  unsigned int conductance = 20;
  unsigned int iterations = 30;

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->PreprocessImage(lowerThreshold, upperThreshold, alpha, beta, conductance, iterations);
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have an image
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->SegmentVesselsFromWidget(true); // isHepatic = true
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have an image
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->CallMergeLabelMaps();
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have both label maps
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  TESTING_OUTPUT_ASSERT_ERRORS_BEGIN();
  logic->SplitVesselsFromWidget(true); // isHepatic = true
  TESTING_OUTPUT_ASSERT_ERRORS(1); // should fail at error where do not have any fiducials
  TESTING_OUTPUT_ASSERT_ERRORS_END();

  return EXIT_SUCCESS;

}
