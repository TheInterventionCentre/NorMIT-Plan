/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLTestFiducialList.cxx

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

// MRML includes
#include "vtkMRMLCoreTestingMacros.h"
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLVolumeArchetypeStorageNode.h>
#include <vtkNew.h>

#include <qSlicerCoreIOManager.h>
#include <qSlicerApplication.h>

// VTK includes
#include <vtkNew.h>

// ITK includes
#include <itkConfigure.h>
#include <itkFactoryRegistration.h>

// module includes
#include "vtkSlicerVesselSegmentationLogic.h"

bool testAddFiducial( const char* volumeName, vtkSlicerVesselSegmentationLogic* logic );

//-----------------------------------------------------------------------------
int vtkMRMLTestFiducialList( int argc, char * argv[] )
{
  bool res = false;
  std::cout << "test fiducial list - being called" << std::endl;

  vtkNew<vtkMRMLScene> scene;
  vtkNew<vtkSlicerVesselSegmentationLogic> logic;

  logic->SetMRMLScene(scene.GetPointer());

  std::cout << "Created scene and logic" << std::endl;

  const char* fileName = "/Users/louise/Documents/build/Slicer-master/ITKv4/Modules/External/SeedVesselSegmentation/test/Data/testImage3_large.nii.gz";
  if (argc > 1)
    {
    fileName = argv[1];
    }
  std::cout << "Using file name " << fileName << std::endl;

  res = testAddFiducial(fileName, logic.GetPointer());

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

//-----------------------------------------------------------------------------
bool testAddFiducial( const char* volumeName, vtkSlicerVesselSegmentationLogic* logic )
{
    std::cout << "inside: testAddFiducial" << std::endl;
    //Retrieve a pointer to the mrml scene
    vtkMRMLScene *scene = logic->GetMRMLScene();

    vtkNew<vtkMRMLVolumeArchetypeStorageNode> storageNode;
    vtkNew<vtkMRMLScalarVolumeNode> scalarNode;

    storageNode->SetFileName(volumeName);
    std::cout << "Try to load the file" << std::endl;
    storageNode->ReadData(scalarNode.GetPointer());
    logic->SetActiveVolume(scalarNode.GetPointer());

    std::cout << "active volume node = " << logic->GetActiveVolume() << std::endl;
    if( logic->GetActiveVolume() == NULL)
    {
      std::cout << "active volume node is null pointer" << std::endl;
      return false;
    }

    // add the fiducial node
    vtkSmartPointer<vtkMRMLMarkupsFiducialNode> fidNode = vtkSmartPointer<vtkMRMLMarkupsFiducialNode>::New();
    scene->AddNode(fidNode);
    std::cout << "added fiducial node to scene" << std::endl;

    // add 1st fiducial
    double inPos[3] = {10.33, 8.55, 0};
    fidNode->AddFiducial(inPos[0], inPos[1], inPos[2]);

    // check our list now contains 1 fiducial
    std::vector<double*> list = logic->GetFiducialList();
    int s = list.size();
    std::cout << "GetFiducialList size (1) = " << s << std::endl;
    if ( s != 1 )
    {
        return false;
    }

    // check last coordinate
    double *pos = logic->GetLastFiducialCoordinate();
    std::cout << "GetLastFiducialCoordinate (10.33,8.55) = " << pos[0] << " " << pos[1] << std::endl;
    if ((pos[0] != 10.33) && (pos[1] != 8.55))
    {
        return false;
    }

    // add 2nd fiducial
    double inPos2[3] = {1.5, 1.5, 0};
    fidNode->AddFiducial(inPos2[0], inPos2[1], inPos2[2]);

    // check our list now contains 2 fiducial
    list = logic->GetFiducialList();
    int s2 = list.size();
    std::cout << "GetFiducialList size (2) = " << s2 << std::endl;
    if ( s2 != 2 )
    {
        return false;
    }

    // check last coordinate
    double *pos2 = logic->GetLastFiducialCoordinate();
    std::cout << "GetLastFiducialCoordinate (1.5,1.5) = " << pos2[0] << " " << pos2[1] << std::endl;
    if ((pos2[0] != 1.5) && (pos2[1] != 1.5))
    {
        return false;
    }

    // check can pop one off list
    list.pop_back();
    int s3 = list.size();
    std::cout << "GetFiducialList size after pop (1) = " << s3 << std::endl;
    if ( s3 != 1 )
    {
        return false;
    }

    // check can pop off list to have it be empty
    list.pop_back();
    int s4 = list.size();
    std::cout << "GetFiducialList size after pop (1) = " << s3 << std::endl;
    if ( s4 != 0 )
    {
        return false;
    }

    return true;
}
