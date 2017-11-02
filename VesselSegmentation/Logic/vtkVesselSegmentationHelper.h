/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkVesselSegmentationHelper.h

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

#ifndef __vtkVesselSegmentationHelper_h
#define __vtkVesselSegmentationHelper_h


#include <vtkSmartPointer.h>

#include <itkImage.h>
#include <itkPoint.h>
#include <itkIndex.h>
#include "itkSeedVesselSegmentationImageFilter.h"

class vtkMRMLScene;
class vtkMRMLScalarVolumeNode;
class vtkImageData;
class vtkMatrix4x4;

/**
 * \ingroup VesselSegmentation
 *
 * \brief Class that contains helper methods to get / prepare the data needed for segmentation.
 *  This includes conversion between itk and vtk images through static functions.
 */
class vtkVesselSegmentationHelper
{
    
public:

    typedef itk::Index<3> Index3D;
    typedef itk::Vector<double,3> Vector3D;

    typedef float pixelType;
    typedef itk::Image<short, 3> LabelMapType;
    typedef itk::Image<pixelType, 3> SeedImageType;

    /**
     * Standard vtk object function to print the properties of the object.
     *
     * @param os output stream where the properties should be printed to.
     * @param indent indentation value.
     */
    void PrintSelf(ostream &os, vtkIndent indent);

    /**
     * Convert from a volume node to an ITK image.
     *
     * @param pointer to a vtkMRMLScalarVolumeNode.
     * @return SeedImageType::Pointer.
     */
    static SeedImageType::Pointer ConvertVolumeNodeToItkImage(vtkMRMLScalarVolumeNode *inVolumeNode,
                           bool applyRasToWorld=true,
                           bool applyRasToLps=true);
    /**
     * Convert from VTK image data to an ITK image.
     *
     * @param pointer to vtkImageData.
     * @return SeedImageType::Pointer.
     */
    static SeedImageType::Pointer ConvertVtkImageDataToItkImage(vtkImageData *inImageData,
                             vtkMatrix4x4 *inToRasMatrix=NULL,
                             vtkMatrix4x4 *inToWorldMatrix=NULL,
                             vtkMatrix4x4 *inRasToLpsMatrix=NULL);

    /**
     * Convert from an ITK image to an VTK image data.
     *
     * @param SeedImageType::Pointer.
     * @return smart pointer to vtkImageData.
     */
    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(SeedImageType::Pointer itkImage);

    /**
     * Convert from an ITK image to an VTK image data.
     *
     * @param itk::Image<unsigned int, 3>::Pointer.
     * @return smart pointer to vtkImageData.
     */
    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(itk::Image<unsigned int, 3>::Pointer itkImage);

    /**
     * Convert from an ITK image to a volume node.
     *
     * @param SeedImageType::Pointer.
     * @return smart pointer to vtkMRMLScalarVolumeNode.
     */
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertItkImageToVolumeNode(SeedImageType::Pointer itkImage, bool applyRasToLps=true);

    /**
     * Convert from VTK image data to a volume node.
     *
     * @param pointer to vtkImageData.
     * @param SeedImageType::Pointer for matching ITK image.
     * @param Bool if should apply LPS to RAS conversion.
     * @return smart pointer to vtkMRMLScalarVolumeNode.
     */
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertVtkImageDataToVolumeNode(vtkImageData *inImageData, SeedImageType::Pointer itkImage, bool applyLpsToRas);

protected:
    vtkVesselSegmentationHelper();
    ~vtkVesselSegmentationHelper();
    
private:
    
    
};

#endif
