/*=========================================================================
  Program: NorMIT-Plan
  Module: itkVesselSegmentationPreProcessingFilter.h

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

#ifndef itkVesselSegmentationPreProcessingFilter_h
#define itkVesselSegmentationPreProcessingFilter_h

#include "VesselSegmentationITKuseGPU.h"
#include "itkImageToImageFilter.h"

#include "itkSigmoidImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"

#ifdef ITK_USE_GPU
 #include "itkGPUImage.h"
 #include "itkGPUGradientAnisotropicDiffusionImageFilter.h"
#else
 #include "itkGradientAnisotropicDiffusionImageFilter.h"
#endif


namespace itk
{
    /** \class ->GetOutput()
     * \brief A filter to enhance vessel objects in 3D images
     *
     * The filter will preprocess the input image data to obtain better
     * vessel segmentation results.
     *
     *
     * \author Rahul P Kumar PhD, The Intervention Centre,
     *                            Oslo University Hospital, Norway.
     *
     * \sa SigmoidImageFilter
     * \sa ResampleImageFilter
     * \sa GPUGradientAnisotropicDiffusionImageFilter
     *
     * \ingroup SeedVesselSegmentation
     */
    template< typename TInputImage, typename TOutputImage >
    class VesselSegmentationPreProcessingFilter:public ImageToImageFilter<  TInputImage, TOutputImage >
    {
    public:

        /** Standard class typedefs. */
        typedef VesselSegmentationPreProcessingFilter Self;
        
        typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
        
        typedef SmartPointer< Self >       Pointer;
        typedef SmartPointer< const Self > ConstPointer;
        
        typedef typename Superclass::InputImageType  InputImageType;
        typedef typename Superclass::OutputImageType OutputImageType;
        typedef typename InputImageType::PixelType   InputPixelType;
        typedef typename OutputImageType::PixelType  OutputPixelType;
        typedef typename OutputImageType::RegionType OutputImageRegionType;
        
        /** Image dimension = 3. */
        itkStaticConstMacro(ImageDimension, unsigned int,
                            InputImageType ::ImageDimension);
        
        /** Method for creation through the object factory. */
        itkNewMacro(Self);
        
        /** Runtime information support. */
        itkTypeMacro(VesselSegmentationPreProcessingFilter,
                     ImageToImageFilter);
#ifdef ITK_USE_GPU
        /** Type for GPU image. */
        typedef GPUImage< InputPixelType, itkGetStaticConstMacro(InputImageDimension) > GPUImageType;
#endif
        
        /** Set/Get macros for LowerThreshold */
        itkSetMacro(LowerThreshold, double);
        itkGetConstMacro(LowerThreshold, double);
        
        /** Set/Get macros for UpperThreshold */
        itkSetMacro(UpperThreshold, double);
        itkGetConstMacro(UpperThreshold, double);
        
        /** Set/Get macros for Alpha */
        itkSetMacro(Alpha, double);
        itkGetConstMacro(Alpha, double);
        
        /** Set/Get macros for Beta */
        itkSetMacro(Beta, double);
        itkGetConstMacro(Beta, double);
        
        /** Set/Get macros for Conductance */
        itkSetMacro(Conductance, double);
        itkGetConstMacro(Conductance, double);
        
        /** Set/Get macros for NumberOfInterations */
        itkSetMacro(NumberOfIterations, unsigned int);
        itkGetConstMacro(NumberOfIterations, unsigned int);
        
#ifdef ITK_USE_CONCEPT_CHECKING
        // Begin concept checking
        itkConceptMacro( DoubleConvertibleToOutputCheck,
                        ( Concept::Convertible< double, OutputPixelType > ) );
        // End concept checking
#endif
        
    protected:
        VesselSegmentationPreProcessingFilter();
        ~VesselSegmentationPreProcessingFilter() {};
        
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        
        /** Generate Data */
        void GenerateData(void) ITK_OVERRIDE;
        
    private:
        VesselSegmentationPreProcessingFilter(const Self &); //purposely not
        // implemented
        void operator=(const Self &);                        //purposely not
        // implemented
        
        typedef itk::SigmoidImageFilter< InputImageType, InputImageType >                       SigmoidFilterType;
        typedef itk::RescaleIntensityImageFilter< InputImageType, InputImageType >              RescaleFilterType;

#ifdef  ITK_USE_GPU
        // Redefine the smoothing filter to use GPU & gpuimagetype
        typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, GPUImageType > SmoothingFilterType;
#else
        typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, InputImageType > SmoothingFilterType;
#endif

        typedef itk::ResampleImageFilter< InputImageType, OutputImageType >                     ResampleFilterType;
        
        typedef itk::AffineTransform< double, 3 >  TransformType;
        
        typedef itk::LinearInterpolateImageFunction< InputImageType, double >  InterpolatorType;
        
        int m_LowerThreshold;
        int m_UpperThreshold;
        int m_Beta;

        unsigned int m_Alpha;
        unsigned int m_Conductance;
        unsigned int m_NumberOfIterations;
        
    };
}  //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVesselSegmentationPreProcessingFilter.hxx"
#endif

#endif
