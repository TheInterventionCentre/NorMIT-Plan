/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkLineWidget3Test1.cxx

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


// Description:
// This tests the creation and placement of a vtkLineWidget3 within
// the bounds of another object.

// This module includes
#include "vtkLineWidget3.h"

// VTK includes
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkMath.h>
#include <vtkTimerLog.h>
#include <vtkSphereSource.h>
#include <vtkProperty.h>
#include <vtkPNGReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageDifference.h>

// STD includes
#include <iostream>

//------------------------------------------------------------------------------
int vtkLineWidget3Test2(int argc, char *argv[])
{

  //Check the arguments
  if (argc != 2)
    {
    std::cerr << "This test expects 1 argument" << std::endl;
    return EXIT_FAILURE;
    }

  // Initialize random number generator
  vtkMath::RandomSeed(static_cast<int>( vtkTimerLog::GetUniversalTime()));

  //Crate the sphere
  vtkNew<vtkSphereSource> sphereSource;
  sphereSource->SetRadius(vtkMath::Random( 0.1,1.0));
  sphereSource->SetCenter(vtkMath::Random(-1.0,1.0),
                          vtkMath::Random(-1.0,1.0),
                          vtkMath::Random(-1.0,1.0));
  sphereSource->Update();
  vtkNew<vtkPolyDataMapper> sphereMapper;
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkNew<vtkActor> sphereActor;
  sphereActor->SetMapper(sphereMapper.GetPointer());

  // Rendering and interaction
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer.GetPointer());
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow.GetPointer());
  sphereActor->GetProperty()->SetOpacity(0.3);

  renderer->AddActor(sphereActor.GetPointer());

  // Create the line widget
  vtkNew<vtkLineWidget3> lineWidget;
  lineWidget->SetInteractor(renderWindowInteractor.GetPointer());
  lineWidget->SetHandleSizeFactor(10.0);
  lineWidget->SetLineSizeFactor(5.0);
  lineWidget->AutoSizeOn();
  lineWidget->On();
  lineWidget->PlaceWidget(sphereSource->GetOutput()->GetBounds());

  renderWindow->Render();

  // Take a screenshot
  vtkNew<vtkWindowToImageFilter> screenshot;
  screenshot->SetInput(renderWindow.GetPointer());
  screenshot->SetInputBufferTypeToRGB();
  screenshot->ReadFrontBufferOff();
  screenshot->Update();

  // Read the reference image
  vtkNew<vtkPNGReader> pngReader;
  pngReader->SetFileName(argv[1]);
  pngReader->Update();

  // Compute the difference between reference and the screenshot
  vtkNew<vtkImageDifference> imageDifference;
  imageDifference->SetInputConnection(0, screenshot->GetOutputPort());
  imageDifference->SetInputConnection(1, pngReader->GetOutputPort());
  imageDifference->AllowShiftOn();
  imageDifference->AveragingOn();

  // Compare the extents
  int ext1[6], ext2[6];
  pngReader->GetOutput()->GetExtent(ext1);
  screenshot->GetOutput()->GetExtent(ext2);
  if (ext1[0] != ext2[0] || ext1[1] != ext2[1] || ext1[2] != ext2[2] ||
      ext1[3] != ext2[3] || ext1[4] != ext2[4] || ext1[5] != ext2[5])
    {
    std::cerr << "Test and regression images are of different size."
              << std::endl;
    return EXIT_FAILURE;
    }

  imageDifference->Update();

  if (imageDifference->GetError() > 100.0)
    {
    std::cerr << "Test and regression images are different e="
              << imageDifference->GetError() << std::endl;
    return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;
}
