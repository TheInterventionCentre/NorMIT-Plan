/*=========================================================================
  Program: NorMIT-Plan
  Module: itkVesselSegmentationPreProcessingFilter.hxx

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

        typename SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
        smoothing->SetInput( remappedImage );
        smoothing->SetTimeStep( timeStep );
        smoothing->SetNumberOfIterations(  m_NumberOfIterations );
        smoothing->SetConductanceParameter( m_Conductance );
        
        typename RescaleFilterType::Pointer rescaleIntensity2 = RescaleFilterType::New();
        rescaleIntensity2->SetInput( smoothing->GetOutput() );
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
