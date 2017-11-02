/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkVesselSegmentationHelper.cxx

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

/* vtk to itk (and back) conversions mostly taken from SlicerRTCommon 
 https://github.com/SlicerRt/SlicerRT/blob/master/SlicerRtCommon/SlicerRtCommon.txx */

#include "vtkSlicerVesselSegmentationLogic.h"

//VTK includes
#include <vtkObjectFactory.h>
#include <vtkSlicerVolumesLogic.h>

//#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// MRML includes
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLTransformNode.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageExport.h>
#include <vtkImageThreshold.h>
#include <vtkTransform.h>
#include <vtkImageCast.h>
#include <vtkMatrix4x4.h>
#include <vtkImageImport.h>

// ITK includes
#include <itkImageRegionIteratorWithIndex.h>
#include <itkImportImageFilter.h>
#include <itkCastImageFilter.h>
#include <vtkVesselSegmentationHelper.h>


vtkStandardNewMacro(vtkVesselSegmentationHelper);

//---------------------------------------------------------------------------
/**
 * Constructor
 */
vtkVesselSegmentationHelper::vtkVesselSegmentationHelper()
{

}

//---------------------------------------------------------------------------
vtkVesselSegmentationHelper::~vtkVesselSegmentationHelper()
{
}

//---------------------------------------------------------------------------
void vtkVesselSegmentationHelper::PrintSelf(ostream &os, vtkIndent vtkNotUsed(indent))
{
    (void)os;
}

//------------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(vtkMRMLScalarVolumeNode *inVolumeNode,
                                        bool applyRasToWorld,
                                        bool applyRasToLps)
{
  // Check for null pointer
  if (inVolumeNode == NULL)
    {
    std::cerr
      << "VolumeNodetoItkImage: Pointer to vtkMRMLScalarVolumeNode is NULL"
      << std::endl;
    return NULL;
    }

  // Obtain IJK to RAS matrix
  vtkSmartPointer<vtkMatrix4x4> inVolumeToRasTransformMatrix =
    vtkSmartPointer<vtkMatrix4x4>::New();
  inVolumeNode->GetIJKToRASMatrix(inVolumeToRasTransformMatrix);

  // Obtain RAS to World transform matrix
  vtkSmartPointer<vtkMatrix4x4> rasToWorldTransformMatrix = 0;

  if (applyRasToWorld)
    {
    rasToWorldTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    vtkMRMLTransformNode *inTransformNode = inVolumeNode->GetParentTransformNode();
    if (inTransformNode != NULL)
      {
      if (!inTransformNode->IsTransformToWorldLinear())
        {
        std::cerr
          <<  "VolumeNodeToItkImage: world transform is not linear"
          << std::endl;
        return NULL;
        }
      inTransformNode->GetMatrixTransformToWorld(rasToWorldTransformMatrix);
      }
    }

  // Obtain RSAS to LPS matrix
  vtkSmartPointer<vtkMatrix4x4> rasToLpsTransformMatrix = 0;
  if (applyRasToLps)
    {
    rasToLpsTransformMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
    rasToLpsTransformMatrix->SetElement(0,0,-1.0);
    rasToLpsTransformMatrix->SetElement(1,1,-1.0);
    rasToLpsTransformMatrix->SetElement(2,2, 1.0);
    rasToLpsTransformMatrix->SetElement(3,3, 1.0);
    }

  SeedImageType::Pointer outItkImage =
      vtkVesselSegmentationHelper::ConvertVtkImageDataToItkImage(inVolumeNode->GetImageData(),
                                              inVolumeToRasTransformMatrix,
                                              rasToWorldTransformMatrix,
                                              rasToLpsTransformMatrix);

  return outItkImage;
}

//------------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkVesselSegmentationHelper::ConvertVtkImageDataToItkImage(vtkImageData *inImageData,
                                          vtkMatrix4x4 *inToRasMatrix,
                                          vtkMatrix4x4 *rasToWorldMatrix,
                                          vtkMatrix4x4 *rasToLpsMatrix)
{
  // Check for null pointer
  if (inImageData == NULL)
    {
    std::cerr
      << "vtkImageDataToItkImage: Pointer to vtkImageData is NULL"
      << std::endl;
    return NULL;
    }

  // Set orientation (if transformations are given)
  vtkSmartPointer<vtkTransform> coordinatesTransform =
    vtkSmartPointer<vtkTransform>::New();
  coordinatesTransform->Identity();
  coordinatesTransform->PostMultiply();

  // Check for transforms
  if (inToRasMatrix != NULL)
    {
    coordinatesTransform->Concatenate(inToRasMatrix);
    }
  if (rasToWorldMatrix != NULL)
    {
    coordinatesTransform->Concatenate(rasToWorldMatrix);
    }
  if (rasToLpsMatrix != NULL)
    {
    coordinatesTransform->Concatenate(rasToLpsMatrix);
    }

  // Set image spacing
  double spacing[3]={0.0};
  coordinatesTransform->GetScale(spacing);
  if (rasToLpsMatrix != NULL)
    {
    spacing[0] = spacing[0] < 0 ? -spacing[0] : spacing[0];
    spacing[1] = spacing[1] < 0 ? -spacing[1] : spacing[1];
    spacing[2] = spacing[2] < 0 ? -spacing[2] : spacing[2];
    }

  // Set image origin
  double origin[3]={0.0};
  coordinatesTransform->GetPosition(origin);

  // Set direction
  vtkSmartPointer<vtkMatrix4x4> inVolumeToWorldTransformMatrix =
    vtkSmartPointer<vtkMatrix4x4>::New();
  coordinatesTransform->GetMatrix(inVolumeToWorldTransformMatrix);

  itk::Matrix<double,3,3> directionMatrix;
  unsigned int col = 0;
  for (col=0; col<3; col++)
    {
    double len = 0;
    unsigned int row = 0;
    for (row=0; row<3; row++)
      {
      len += inVolumeToWorldTransformMatrix->GetElement(row, col) *
        inVolumeToWorldTransformMatrix->GetElement(row, col);
      }
    if (len == 0.0)
      {
      len = 1.0;
      }
    len = sqrt(len);
    for (row=0; row<3; row++)
      {
      directionMatrix[row][col] =
        inVolumeToWorldTransformMatrix->GetElement(row, col)/len;
      }
    }

  // Set image extent
  int *extent = inImageData->GetExtent();

  vtkVesselSegmentationHelper::SeedImageType::SizeType inSize;
  inSize[0] = extent[1] - extent[0] + 1;
  inSize[1] = extent[3] - extent[2] + 1;
  inSize[2] = extent[5] - extent[4] + 1;
  vtkVesselSegmentationHelper::SeedImageType::IndexType start = {0};
  vtkVesselSegmentationHelper::SeedImageType::RegionType region;
  region.SetSize(inSize);
  region.SetIndex(start);

  std::cout
  << "vtkImageDataToItkImage: start import filter"
  << std::endl;

  vtkVesselSegmentationHelper::SeedImageType::Pointer imgReturn;
  int scalarType = inImageData->GetScalarType();


  if(scalarType == VTK_FLOAT)
    {
       std::cout
       << "vtkImageDataToItkImage: FLOAT"
       << std::endl;

       typedef float pixelType;
       typedef itk::ImportImageFilter<pixelType, 3> ImportFilterType;
       ImportFilterType::Pointer importFilter = ImportFilterType::New();
       importFilter->SetSpacing(spacing);
       importFilter->SetOrigin(origin);
       importFilter->SetDirection(directionMatrix);
       importFilter->SetRegion(region);

       // Import (itk filter will not have ownership of memory!)
       float *pointerToData = static_cast<float*>(inImageData->GetScalarPointer());
       unsigned int dataSize = inSize[0] * inSize[1] * inSize[2];
       importFilter->SetImportPointer(pointerToData, dataSize, false);
       importFilter->Update();

       // already float, no need to cast
       imgReturn = importFilter->GetOutput();

    }
  else if(scalarType == VTK_SHORT)
    {
      std::cout
      << "vtkImageDataToItkImage: SHORT"
      << std::endl;

      typedef short pixelType;
      typedef itk::Image<pixelType, 3>  InputImageType;

      typedef itk::ImportImageFilter<pixelType, 3> ImportFilterType;
      ImportFilterType::Pointer importFilter = ImportFilterType::New();
      importFilter->SetSpacing(spacing);
      importFilter->SetOrigin(origin);
      importFilter->SetDirection(directionMatrix);
      importFilter->SetRegion(region);

      // Import (itk filter will not have ownership of memory!)
      short *pointerToData = static_cast<short*>(inImageData->GetScalarPointer());
      unsigned int dataSize = inSize[0] * inSize[1] * inSize[2];
      importFilter->SetImportPointer(pointerToData, dataSize, false);
      importFilter->Update();

      typedef itk::CastImageFilter<InputImageType, vtkVesselSegmentationHelper::SeedImageType> CastFilterType;
      CastFilterType::Pointer castFilter = CastFilterType::New();
      castFilter->SetInput(importFilter->GetOutput());
      castFilter->Update();
      imgReturn = castFilter->GetOutput();

    }
  else if(scalarType == VTK_UNSIGNED_CHAR)
    {
       std::cout
       << "vtkImageDataToItkImage: UNSIGNED CHAR"
       << std::endl;

       typedef unsigned char pixelType;
       typedef itk::Image<pixelType, 3>  InputImageType;

       typedef itk::ImportImageFilter<pixelType, 3> ImportFilterType;
       ImportFilterType::Pointer importFilter = ImportFilterType::New();
       importFilter->SetSpacing(spacing);
       importFilter->SetOrigin(origin);
       importFilter->SetDirection(directionMatrix);
       importFilter->SetRegion(region);

       // Import (itk filter will not have ownership of memory!)
       unsigned char *pointerToData = static_cast<unsigned char*>(inImageData->GetScalarPointer());
       unsigned int dataSize = inSize[0] * inSize[1] * inSize[2];
       importFilter->SetImportPointer(pointerToData, dataSize, false);
       importFilter->Update();

       typedef itk::CastImageFilter<InputImageType, vtkVesselSegmentationHelper::SeedImageType> CastFilterType;
       CastFilterType::Pointer castFilter = CastFilterType::New();
       castFilter->SetInput(importFilter->GetOutput());
       castFilter->Update();
       imgReturn = castFilter->GetOutput();

    }
  else if(scalarType == VTK_UNSIGNED_INT)
    {
       std::cout
       << "vtkImageDataToItkImage: UNSIGNED INT"
       << std::endl;

       typedef unsigned int pixelType;
       typedef itk::Image<pixelType, 3>  InputImageType;

       typedef itk::ImportImageFilter<pixelType, 3> ImportFilterType;
       ImportFilterType::Pointer importFilter = ImportFilterType::New();
       importFilter->SetSpacing(spacing);
       importFilter->SetOrigin(origin);
       importFilter->SetDirection(directionMatrix);
       importFilter->SetRegion(region);

       // Import (itk filter will not have ownership of memory!)
       unsigned int *pointerToData = static_cast<unsigned int*>(inImageData->GetScalarPointer());
       unsigned int dataSize = inSize[0] * inSize[1] * inSize[2];
       importFilter->SetImportPointer(pointerToData, dataSize, false);
       importFilter->Update();

       typedef itk::CastImageFilter<InputImageType, vtkVesselSegmentationHelper::SeedImageType> CastFilterType;
       CastFilterType::Pointer castFilter = CastFilterType::New();
       castFilter->SetInput(importFilter->GetOutput());
       castFilter->Update();
       imgReturn = castFilter->GetOutput();

    }
  else
    {
    std::cerr
    << "vtkImageDataToItkImage: was not one of the scalar types"
    << std::endl;
    return NULL;
    }
  std::cout
  << "vtkImageDataToItkImage: end import filter"
  << std::endl;

  return imgReturn;

}


//------------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(itk::Image<unsigned int, 3>::Pointer itkImage)
{
  if ( itkImage.IsNull() )
    {
    std::cerr
    << "ItkImageToVtkImageData: itkImage (input image) is null"
    << std::endl;
    return NULL;
    }

  itk::Image<unsigned int, 3>::RegionType region = itkImage->GetBufferedRegion();
  itk::Image<unsigned int, 3>::SizeType imageSize = region.GetSize();
  //itk::Image<unsigned int, 3>::SpacingType imageSpacing = itkImage->GetSpacing();
  //itk::Image<unsigned int, 3>::PointType origin = itkImage->GetOrigin();

  int extent[6]={0, (int) imageSize[0]-1,
                 0, (int) imageSize[1]-1,
                 0, (int) imageSize[2]-1};

  vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();

  imageImport->SetDataScalarType(VTK_UNSIGNED_INT);
  imageImport->SetNumberOfScalarComponents(1);
  imageImport->SetDataSpacing(1,1,1);
  imageImport->SetDataOrigin(0,0,0);
  imageImport->SetWholeExtent(extent);
  imageImport->SetDataExtentToWholeExtent();
  void *dataPointer = static_cast<void*>(itkImage->GetBufferPointer());
  imageImport->SetImportVoidPointer(dataPointer);
  imageImport->Update();

  vtkSmartPointer<vtkImageData> vtkImage = imageImport->GetOutput();

  std::cout
  << "ItkImageToVtkImageData: vtkImage reference count " << vtkImage->GetReferenceCount()
  << std::endl;

  return vtkImage;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkImageData>
vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(SeedImageType::Pointer itkImage)
{
  if ( itkImage.IsNull() )
    {
    std::cerr
    << "ItkImageToVtkImageData: itkImage (input image) is null"
    << std::endl;
    return NULL;
    }

  SeedImageType::RegionType region = itkImage->GetBufferedRegion();
  SeedImageType::SizeType imageSize = region.GetSize();
  //SeedImageType::SpacingType imageSpacing = itkImage->GetSpacing();
  //SeedImageType::PointType origin = itkImage->GetOrigin();

  int extent[6]={0, (int) imageSize[0]-1,
                 0, (int) imageSize[1]-1,
                 0, (int) imageSize[2]-1};

  vtkSmartPointer<vtkImageImport> imageImport = vtkSmartPointer<vtkImageImport>::New();

  imageImport->SetDataScalarType(VTK_FLOAT);
  imageImport->SetNumberOfScalarComponents(1);
  imageImport->SetDataSpacing(1,1,1);
  imageImport->SetDataOrigin(0,0,0);
  imageImport->SetWholeExtent(extent);
  imageImport->SetDataExtentToWholeExtent();
  void *dataPointer = static_cast<void*>(itkImage->GetBufferPointer());
  imageImport->SetImportVoidPointer(dataPointer);
  imageImport->Update();

  vtkSmartPointer<vtkImageData> vtkImage = imageImport->GetOutput();

  std::cout
  << "ItkImageToVtkImageData: vtkImage reference count " << vtkImage->GetReferenceCount()
  << std::endl;

  return vtkImage;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkMRMLScalarVolumeNode>
vtkVesselSegmentationHelper::
ConvertItkImageToVolumeNode(SeedImageType::Pointer itkImage,
                            bool applyLpsToRas)
{
  if (itkImage.IsNull())
    {
    std::cerr << "ConvertItkImageToVolumeNode: itkImage empty pointer"
              << std::endl;
    return NULL;
    }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> outVolumeNode =
    vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  // Get input image properties
  SeedImageType::RegionType itkRegion =
    itkImage->GetLargestPossibleRegion();
  SeedImageType::PointType itkOrigin =
    itkImage->GetOrigin();
  SeedImageType::SpacingType itkSpacing =
    itkImage->GetSpacing();
  SeedImageType::DirectionType itkDirections =
    itkImage->GetDirection();

  vtkSmartPointer<vtkImageData> outImageData = vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(itkImage);

  if (!outImageData)
  {
    std::cerr << "ConvertItkImageToVolumeNode: vtk conversion returned no data"
              << std::endl;
    return NULL;
  }

  // Make image properties accessible for VTK
  double origin[3] = {itkOrigin[0], itkOrigin[1], itkOrigin[2]};
  double spacing[3] = {itkSpacing[0], itkSpacing[1], itkSpacing[2]};

  outVolumeNode->SetAndObserveImageData(outImageData);

  // Apply ITK geometry to volume node
  outVolumeNode->SetOrigin(origin);
  outVolumeNode->SetSpacing(spacing);

  double directions[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
  for (unsigned int col=0; col<3; col++)
    {
    for (unsigned int row=0; row<3; row++)
      {
      directions[row][col] = itkDirections[row][col];
      }
    }
  outVolumeNode->SetIJKToRASDirections(directions);

  // Apply LPS to RAS conversion if requested
  if (applyLpsToRas)
    {
    //  LPS (ITK)to RAS (Slicer) transform matrix
    vtkSmartPointer<vtkMatrix4x4> lps2RasTransformMatrix =
      vtkSmartPointer<vtkMatrix4x4>::New();
    lps2RasTransformMatrix->SetElement(0,0,-1.0);
    lps2RasTransformMatrix->SetElement(1,1,-1.0);
    lps2RasTransformMatrix->SetElement(2,2, 1.0);
    lps2RasTransformMatrix->SetElement(3,3, 1.0);

    vtkSmartPointer<vtkMatrix4x4> outVolumeImageToLpsWorldTransformMatrix =
      vtkSmartPointer<vtkMatrix4x4>::New();
    outVolumeNode->GetIJKToRASMatrix(outVolumeImageToLpsWorldTransformMatrix);

    vtkSmartPointer<vtkTransform> imageToWorldTransform =
      vtkSmartPointer<vtkTransform>::New();
    imageToWorldTransform->Identity();
    imageToWorldTransform->PostMultiply();
    imageToWorldTransform->Concatenate(outVolumeImageToLpsWorldTransformMatrix);
    imageToWorldTransform->Concatenate(lps2RasTransformMatrix);

    outVolumeNode->SetIJKToRASMatrix(imageToWorldTransform->GetMatrix());
    }

  std::cout
  << "ConvertItkImageToVolumeNode: vtkImage reference count " << outImageData->GetReferenceCount()
  << std::endl;

  return outVolumeNode;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkMRMLScalarVolumeNode>
vtkVesselSegmentationHelper::
ConvertVtkImageDataToVolumeNode(vtkImageData *inImageData, SeedImageType::Pointer itkImage, bool applyLpsToRas)
{
  // Check for null pointer
  if (inImageData == NULL)
    {
    std::cerr
      << "vtkImageDataToItkImage: Pointer to vtkImageData is NULL"
      << std::endl;
    return NULL;
    }

  vtkSmartPointer<vtkMRMLScalarVolumeNode> outVolumeNode =
    vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();

  // Get input image properties
  SeedImageType::RegionType itkRegion =
    itkImage->GetLargestPossibleRegion();
  SeedImageType::PointType itkOrigin =
    itkImage->GetOrigin();
  SeedImageType::SpacingType itkSpacing =
    itkImage->GetSpacing();
  SeedImageType::DirectionType itkDirections =
    itkImage->GetDirection();


  // Make image properties accessible for VTK
  double origin[3] = {itkOrigin[0], itkOrigin[1], itkOrigin[2]};
  double spacing[3] = {itkSpacing[0], itkSpacing[1], itkSpacing[2]};

  outVolumeNode->SetAndObserveImageData(inImageData);

  // Apply ITK geometry to volume node
  outVolumeNode->SetOrigin(origin);
  outVolumeNode->SetSpacing(spacing);

  double directions[3][3] = {{1.0,0.0,0.0},{0.0,1.0,0.0},{0.0,0.0,1.0}};
  for (unsigned int col=0; col<3; col++)
    {
    for (unsigned int row=0; row<3; row++)
      {
      directions[row][col] = itkDirections[row][col];
      }
    }
  outVolumeNode->SetIJKToRASDirections(directions);

  // Apply LPS to RAS conversion if requested
  if (applyLpsToRas)
    {
    //  LPS (ITK)to RAS (Slicer) transform matrix
    vtkSmartPointer<vtkMatrix4x4> lps2RasTransformMatrix =
      vtkSmartPointer<vtkMatrix4x4>::New();
    lps2RasTransformMatrix->SetElement(0,0,-1.0);
    lps2RasTransformMatrix->SetElement(1,1,-1.0);
    lps2RasTransformMatrix->SetElement(2,2, 1.0);
    lps2RasTransformMatrix->SetElement(3,3, 1.0);

    vtkSmartPointer<vtkMatrix4x4> outVolumeImageToLpsWorldTransformMatrix =
      vtkSmartPointer<vtkMatrix4x4>::New();
    outVolumeNode->GetIJKToRASMatrix(outVolumeImageToLpsWorldTransformMatrix);

    vtkSmartPointer<vtkTransform> imageToWorldTransform =
      vtkSmartPointer<vtkTransform>::New();
    imageToWorldTransform->Identity();
    imageToWorldTransform->PostMultiply();
    imageToWorldTransform->Concatenate(outVolumeImageToLpsWorldTransformMatrix);
    imageToWorldTransform->Concatenate(lps2RasTransformMatrix);

    outVolumeNode->SetIJKToRASMatrix(imageToWorldTransform->GetMatrix());
    }

  return outVolumeNode;
}

