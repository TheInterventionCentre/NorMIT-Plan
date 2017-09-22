/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkVesselSegmentationPreProcessingFilter_h
#define itkVesselSegmentationPreProcessingFilter_h

#include "itkImageToImageFilter.h"

#include "itkSigmoidImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkLinearInterpolateImageFunction.h"

#ifdef ITK_USE_GPU
#include "itkGPUImage.h"
#include "itkGPUGradientAnisotropicDiffusionImageFilter.h"
#endif
#include "itkGradientAnisotropicDiffusionImageFilter.h"

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
        itkSetMacro(NumberOfIterations, int);
        itkGetConstMacro(NumberOfIterations, int);
        
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

#ifdef ITK_USE_GPU
        // Redefine the smoothing filter to use GPU
        typedef itk::GPUGradientAnisotropicDiffusionImageFilter< InputImageType, GPUImageType > SmoothingFilterType;
#else
        typedef itk::GradientAnisotropicDiffusionImageFilter< InputImageType, InputImageType > SmoothingFilterType;
#endif
        typedef itk::ResampleImageFilter< InputImageType, OutputImageType >                     ResampleFilterType;
        
        typedef itk::AffineTransform< double, 3 >  TransformType;
        
        typedef itk::LinearInterpolateImageFunction< InputImageType, double >  InterpolatorType;
        
        double m_LowerThreshold;
        double m_UpperThreshold;
        double m_Alpha;
        double m_Beta;
        double m_Conductance;
        
        int m_NumberOfIterations;
        
    };
}  //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVesselSegmentationPreProcessingFilter.hxx"
#endif

#endif
