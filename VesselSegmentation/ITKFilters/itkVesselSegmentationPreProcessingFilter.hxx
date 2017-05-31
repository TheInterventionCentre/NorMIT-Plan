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
#ifndef itkVesselSegmentationPreProcessingFilter_hxx
#define itkVesselSegmentationPreProcessingFilter_hxx

#include "itkVesselSegmentationPreProcessingFilter.h"

namespace itk
{
    /**
     * Constructor
     */
    template< typename TInputImage, typename TOutputImage >
    VesselSegmentationPreProcessingFilter< TInputImage, TOutputImage >
    ::VesselSegmentationPreProcessingFilter()
    {
        // set defaults for member variables 
        m_LowerThreshold = 100.0;
        m_UpperThreshold = 250.0;
        m_Alpha          = 20.0;
        m_Beta           = 160.0;
        m_Conductance    = 20.0;
        
        m_NumberOfIterations = 30;
        
    }
    
    template< typename TInputImage, typename TOutputImage >
    void VesselSegmentationPreProcessingFilter< TInputImage, TOutputImage >
    ::GenerateData()
    {
        
        std::cout << "1/3: nonLinearIntensityRemap - Sigmoid" << std::endl;
        
        typename SigmoidFilterType::Pointer sigmoidFilter = SigmoidFilterType::New();
        sigmoidFilter->SetInput( this->GetInput() );
        sigmoidFilter->SetOutputMinimum( m_LowerThreshold );
        sigmoidFilter->SetOutputMaximum( m_UpperThreshold );
        sigmoidFilter->SetAlpha( m_Alpha );
        sigmoidFilter->SetBeta( m_Beta );
        
        typename RescaleFilterType::Pointer rescaleIntensity = RescaleFilterType::New();
        rescaleIntensity->SetInput( sigmoidFilter->GetOutput() );
        rescaleIntensity->SetOutputMinimum(0.0);
        rescaleIntensity->SetOutputMaximum(255.0);
        rescaleIntensity->Update();
        typename InputImageType::Pointer remappedImage = rescaleIntensity->GetOutput();
        
        
        std::cout << "2/3: SmoothImage" << std::endl;
        
        const typename InputImageType::SpacingType& sp = remappedImage->GetSpacing();
        double min_Spacing = sp[0];
        if (min_Spacing > sp[1])
        {
            min_Spacing = sp[1];
        }
        if (min_Spacing > sp[2])
        {
            min_Spacing = sp[2];
        }
        
        double timeStep = min_Spacing/(powf(2, 4)) - 0.0001; //4 = Dimension + 1; timeStep < min_Spacing/(powf(2, 4))
        
        typename GPUSmoothingFilterType::Pointer gpuSmoothing = GPUSmoothingFilterType::New();
        gpuSmoothing->SetInput( remappedImage );
        gpuSmoothing->SetTimeStep( timeStep );
        gpuSmoothing->SetNumberOfIterations(  m_NumberOfIterations );
        gpuSmoothing->SetConductanceParameter( m_Conductance );
        
        typename RescaleFilterType::Pointer rescaleIntensity2 = RescaleFilterType::New();
        rescaleIntensity2->SetInput( gpuSmoothing->GetOutput() );
        rescaleIntensity2->SetOutputMinimum(0.0);
        rescaleIntensity2->SetOutputMaximum(255.0);
        rescaleIntensity2->Update();
        typename InputImageType::Pointer smoothedImage =  rescaleIntensity2->GetOutput();
        
        
        std::cout << "3/3: ResampleImage" << std::endl;
        
        typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
        typename TransformType::Pointer transform = TransformType::New();
        typename InterpolatorType::Pointer interpolator = InterpolatorType::New();
        
        transform->SetIdentity();
        resampleFilter->SetTransform( transform );
        resampleFilter->SetInterpolator( interpolator );
        resampleFilter->SetDefaultPixelValue( NumericTraits<OutputPixelType>::ZeroValue() );
        
        if (sp[2] > 1.5)
        {
            min_Spacing = 1.5;
        }
        
        typename InputImageType::SpacingType newSp;
        newSp[0] = min_Spacing;
        newSp[1] = min_Spacing;
        newSp[2] = min_Spacing;
        std::cout<<"original spacing :"<< sp << std::endl;
        std::cout<<"new spacing      :"<< newSp << std::endl;
        resampleFilter->SetOutputSpacing( newSp );
        resampleFilter->SetOutputOrigin( smoothedImage->GetOrigin() );
        resampleFilter->SetOutputDirection( smoothedImage->GetDirection() );
        
        typename InputImageType::SizeType   newSize;
        newSize[0] = int( ( double(smoothedImage->GetLargestPossibleRegion().GetSize()[0]) * double(smoothedImage->GetSpacing()[0]) ) / double(newSp[0]) );  // number of pixels along X
        newSize[1] = int( ( double(smoothedImage->GetLargestPossibleRegion().GetSize()[1]) * double(smoothedImage->GetSpacing()[1]) ) / double(newSp[1]) );  // number of pixels along Y
        newSize[2] = int( ( double(smoothedImage->GetLargestPossibleRegion().GetSize()[2]) * double(smoothedImage->GetSpacing()[2]) ) / double(newSp[2]) );  // number of pixels along Z
        
        std::cout<<"New Size : "<< newSize << std::endl;
        resampleFilter->SetSize( newSize );
        
        resampleFilter->SetInput( smoothedImage );
        resampleFilter->Update();
        
        // Allocate the output
        this->GetOutput()->SetRequestedRegion(resampleFilter->GetOutput()->GetLargestPossibleRegion());
        //this->GetOutput()->SetRequestedRegion(rescaleIntensity2->GetOutput()->GetLargestPossibleRegion());
        this->GetOutput()->SetBufferedRegion( this->GetOutput()->GetRequestedRegion() );
        this->GetOutput()->Allocate();
        
        // graft the output of the resample filter back onto this filter's
        // output. this is needed to get the appropriate regions passed
        // back.
        this->GraftOutput( resampleFilter->GetOutput() );
        //this->GraftOutput(rescaleIntensity2->GetOutput());
        
        std::cout<<std::endl<<"Done PreProcessing."<<std::endl;
    }
    
    template< typename TInputImage, typename TOutputImage >
    void VesselSegmentationPreProcessingFilter< TInputImage, TOutputImage >
    ::PrintSelf(std::ostream & os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
        
        os << indent << "LowerThreshold:  " << m_LowerThreshold << std::endl;
        os << indent << "UpperThreshold:  " << m_UpperThreshold << std::endl;
        os << indent << "Alpha:  " << m_Alpha  << std::endl;
        os << indent << "Beta:  " << m_Beta  << std::endl;
        os << indent << "Conductance:  " << m_Conductance  << std::endl;
        os << indent << "NumberOfIterations:  " << m_NumberOfIterations  << std::endl;
    }
}  // end namespace itk
#endif
