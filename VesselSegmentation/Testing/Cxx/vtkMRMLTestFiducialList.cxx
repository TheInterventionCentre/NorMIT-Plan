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
    int fidIndex = fidNode->AddFiducial(inPos[0], inPos[1], inPos[2]);

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
    fidIndex = fidNode->AddFiducial(inPos2[0], inPos2[1], inPos2[2]);

    // check our list now contains 2 fiducial
    std::vector<double*> list2 = logic->GetFiducialList();
    int s2 = list2.size();
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
    list2.pop_back();
    int s3 = list2.size();
    std::cout << "GetFiducialList size after pop (1) = " << s3 << std::endl;
    if ( s3 != 1 )
    {
        return false;
    }

    return true;
}

