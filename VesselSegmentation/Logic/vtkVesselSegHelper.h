/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkVesselSegHelper.h

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

#ifndef __vtkVesselSegHelper_h
#define __vtkVesselSegHelper_h

#include <vtkObject.h>
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
 *  Class that contains helper methods to get / prepare the data needed for calling Rahul's algorithm.
 *  This includes conversion between itk and vtk images.
 */
class vtkVesselSegHelper: public vtkObject
{
    
public:
    
    static vtkVesselSegHelper *New();
    vtkTypeMacro(vtkVesselSegHelper, vtkObject);

    typedef itk::Index<3> Index3D;
    typedef itk::Vector<double,3> Vector3D;

    typedef float pixelType;
    typedef itk::Image<short, 3> LabelMapType;
    typedef itk::Image<pixelType, 3> SeedImageType;

    static void GetActiveNode(vtkMRMLScene* myMRMLScene);
    
    void PrintSelf(ostream &os, vtkIndent indent);

    static bool ConvertRAStoLPS(double *inPoint, double *outPoint);
    
    static SeedImageType::Pointer ConvertVolumeNodeToItkImage(vtkMRMLScalarVolumeNode *inVolumeNode,
                           bool applyRasToWorld=true,
                           bool applyRasToLps=true);
    static SeedImageType::Pointer ConvertVtkImageDataToItkImage(vtkImageData *inImageData,
                             vtkMatrix4x4 *inToRasMatrix=NULL,
                             vtkMatrix4x4 *inToWorldMatrix=NULL,
                             vtkMatrix4x4 *inRasToLpsMatrix=NULL);

    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(SeedImageType::Pointer itkImage);
    static vtkSmartPointer<vtkImageData> ConvertItkImageToVtkImageData(itk::Image<unsigned int, 3>::Pointer itkImage);
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertItkImageToVolumeNode(SeedImageType::Pointer itkImage, bool applyRasToLps=true);
    static vtkSmartPointer<vtkMRMLScalarVolumeNode> ConvertVtkImageDataToVolumeNode(vtkImageData *inImageData, SeedImageType::Pointer itkImage, bool applyLpsToRas);

protected:

    vtkVesselSegHelper();
    ~vtkVesselSegHelper();
    
private:
    
    
};

#endif
