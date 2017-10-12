/*=========================================================================
  Program: NorMIT-Plan
  Module: itkConnectedRegionDistanceMapImageFilter.hxx

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

#ifndef itkConnectedRegionDistanceMapImageFilter_hxx
#define itkConnectedRegionDistanceMapImageFilter_hxx

#include "itkConnectedRegionDistanceMapImageFilter.h"
#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkProgressAccumulator.h"
#include "itkConstNeighborhoodIterator.h"

namespace itk
{
    /**
     * Constructor
     */
    template< typename TInputImage, typename TOutputImage >
    ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::ConnectedRegionDistanceMapImageFilter()
    {
        // set defaults for member variables
        m_SigmoidFilter    = SigmoidType::New();
        m_ConnectedFilter  = ConnectedType::New();
        m_IsoContourFilter = IsoContourType::New();
        m_ChamferFilter    = ChamferType::New();
        
        m_Seed[0] = 0;
        m_Seed[1] = 0;
        m_Seed[2] = 0;
        
        m_IntensityMinimum = NumericTraits< InputPixelType >::NonpositiveMin();
        m_IntensityMaximum = NumericTraits< InputPixelType >::max();
        m_Threshold        = NumericTraits< InputPixelType >::NonpositiveMin();
        
        m_Alpha = 20;
        m_Beta  = NumericTraits< InputPixelType >::max();
        
        m_GenerateThresholdOutput = false;
        
        m_BypassSigmoid = false;
        
        typename ThresholdImageType::Pointer thresholdImage = ThresholdImageType::New();
        this->ProcessObject::SetNumberOfRequiredOutputs(2);
        this->ProcessObject::SetNthOutput( 1, thresholdImage.GetPointer() );
        
    }
    
    template< typename TInputImage, typename TOutputImage >
    typename ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::DataObjectPointer
    ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::MakeOutput(DataObjectPointerArraySizeType idx)
    {
        if ( idx == 1 )
        {
            return ThresholdImageType::New().GetPointer();
        }
        return Superclass::MakeOutput(idx);
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::GenerateData()
    {
        typename InputImageType::ConstPointer input = this->GetInput();
        
        // Allocate the output
        this->GetOutput()->SetBufferedRegion( this->GetOutput()->GetRequestedRegion() );
        this->GetOutput()->Allocate();
        
        typename OutputImageType::Pointer output = this->GetOutput();
        
        typedef typename OutputImageType::RegionType OutputRegionType;
        OutputRegionType outputRegion = output->GetRequestedRegion();
        
        typename ThresholdImageType::Pointer thresholdImage;
        
        if ( m_GenerateThresholdOutput )
        {
            thresholdImage = dynamic_cast< ThresholdImageType * >( this->ProcessObject::GetOutput(1) );
            thresholdImage->SetBufferedRegion( thresholdImage->GetRequestedRegion() );
            thresholdImage->Allocate(true); // initialize buffer to zero
        }
        
        // calculate the largest possible distance in the output image.
        // this maximum is the distance from one corner of the image to the other.
        OutputSizeType      outputSize = outputRegion.GetSize();
        OutputSizeValueType maximumDistance = 0;
        
        for ( unsigned int i = 0; i < ImageDimension; i++ )
        {
            maximumDistance += outputSize[i] * outputSize[i];
        }
        // cast to float and back because there's no sqrt defined for unsigned long
        // double,
        // which is the general SizeValueType.
        maximumDistance = static_cast< OutputSizeValueType >( std::sqrt( static_cast< double >( maximumDistance ) ) );
        
        ThreadIdType numberOfThreads = this->GetNumberOfThreads();
        
        typedef ConstNeighborhoodIterator< InputImageType > ConstNeighborhoodIteratorType;
        typename ConstNeighborhoodIteratorType::RadiusType neighborhoodRadius;
        neighborhoodRadius.Fill(1);
        ConstNeighborhoodIteratorType itn(neighborhoodRadius, input, input->GetRequestedRegion());
        
        itn.SetLocation(m_Seed);
        InputPixelType intensityLocalMin = itn.GetPixel(0);
        for (unsigned i = 0; i < itn.Size(); i++)
        {
            if (intensityLocalMin > itn.GetPixel(i))
            {
                intensityLocalMin = itn.GetPixel(i);
            }
        }
        
        m_Beta = intensityLocalMin - m_Alpha;
        
        // Create a process accumulator for tracking the progress of this minipipeline
        ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
        progress->SetMiniPipelineFilter( this );
        
        if (!m_BypassSigmoid)
        {
            progress->RegisterInternalFilter( m_SigmoidFilter, 0.25f );
            progress->RegisterInternalFilter( m_ConnectedFilter, 0.25f );
            progress->RegisterInternalFilter( m_IsoContourFilter, 0.25f );
            progress->RegisterInternalFilter( m_ChamferFilter, 0.25f );
            
            m_SigmoidFilter->SetInput(input);
            m_SigmoidFilter->SetOutputMinimum(   m_IntensityMinimum  );
            m_SigmoidFilter->SetOutputMaximum(   m_IntensityMaximum  );
            m_SigmoidFilter->SetAlpha(  m_Alpha  );
            m_SigmoidFilter->SetBeta(  m_Beta );
            m_SigmoidFilter->InPlaceOn();
            
            m_ConnectedFilter->SetInput( m_SigmoidFilter->GetOutput() );
        }
        else
        {
            progress->RegisterInternalFilter( m_ConnectedFilter, (1.0f / 3.0f) );
            progress->RegisterInternalFilter( m_IsoContourFilter, (1.0f / 3.0f) );
            progress->RegisterInternalFilter( m_ChamferFilter, (1.0f / 3.0f) );

            m_ConnectedFilter->SetInput( input );
        }
        
        m_ConnectedFilter->SetLower( m_Threshold );
        m_ConnectedFilter->SetUpper( m_IntensityMaximum );
        m_ConnectedFilter->SetReplaceValue( 100 );
        m_ConnectedFilter->SetSeed( m_Seed );
        
        thresholdImage = dynamic_cast< ThresholdImageType * >( this->ProcessObject::GetOutput(1) );
        if ( m_GenerateThresholdOutput )
        {
            m_ConnectedFilter->Update();
            
            ImageRegionIterator< ThresholdImageType > thresholdit;
            thresholdit = ImageRegionIterator< ThresholdImageType >(thresholdImage, outputRegion);
            ImageRegionIterator< ThresholdImageType > connectit;
            connectit = ImageRegionIterator< ThresholdImageType >(m_ConnectedFilter->GetOutput(), outputRegion);
            
            for (thresholdit.GoToBegin(), connectit.GoToBegin(); !connectit.IsAtEnd(); ++thresholdit ,++connectit)
            {
                thresholdit.Set(connectit.Get());
            }
        }
        
        // set up the isocontour filter
        m_IsoContourFilter->SetInput( m_ConnectedFilter->GetOutput() );
        m_IsoContourFilter->SetFarValue( maximumDistance + 1 );
        m_IsoContourFilter->SetNumberOfThreads( numberOfThreads );
        InputPixelType levelSetValue = 50; // ( m_InsideValue + m_OutsideValue ) / 2;
        m_IsoContourFilter->SetLevelSetValue( levelSetValue );
        
        // set up the chamfer filter
        m_ChamferFilter->SetInput( m_IsoContourFilter->GetOutput() );
        m_ChamferFilter->SetMaximumDistance( maximumDistance );
        m_ChamferFilter->SetNumberOfThreads( numberOfThreads );
        
        // graft our output to the chamfer filter to force the proper regions
        // to be generated
        m_ChamferFilter->GraftOutput( output );
        
        // create the distance map
        m_ChamferFilter->Update();
        
        // graft the output of the chamfer filter back onto this filter's
        // output. this is needed to get the appropriate regions passed
        // back.
        this->GraftOutput( m_ChamferFilter->GetOutput() );
        
        typedef itk::ImageRegionIterator< OutputImageType > IteratorType;
        IteratorType itOutput( output, outputRegion );
        
        for (itOutput.GoToBegin(); !itOutput.IsAtEnd(); ++itOutput)
        {
            if (itOutput.Get() < NumericTraits<OutputPixelType>::ZeroValue() )
            {
                itOutput.Set( NumericTraits<OutputPixelType>::ZeroValue() );
            }
        }
    }
    
    /** Get the Threshold Image Output */
    template< typename TInputImage, typename TOutputImage >
    typename ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::ThresholdImageType *
    ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::GetThresholdOutput()
    {
        return dynamic_cast< ThresholdImageType * >( this->ProcessObject::GetOutput(1) );
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    ConnectedRegionDistanceMapImageFilter < TInputImage, TOutputImage >
    ::PrintSelf(std::ostream & os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
        
        os << indent << "SigmoidImageFilter (used internally): "
        << m_SigmoidFilter << std::endl;
        os << indent << "ConnectedThresholdImageFilter (used internally): "
        << m_ConnectedFilter << std::endl;
        os << indent << "IsoContourDistanceImageFilter (used internally): "
        << m_IsoContourFilter << std::endl;
        os << indent << "FastChamferDistanceImageFilter (used internally): "
        << m_ChamferFilter << std::endl;
        os << indent << "Seed:  " << m_Seed << std::endl;
        os << indent << "IntensityMinimum:  " << m_IntensityMinimum << std::endl;
        os << indent << "IntensityMaximum:  " << m_IntensityMaximum << std::endl;
        os << indent << "Threshold:  " << m_Threshold << std::endl;
        os << indent << "Alpha:  " << m_Alpha << std::endl;
        os << indent << "Beta:  " << m_Beta << std::endl;
        os << indent << "GenerateThresholdOutput:  " << m_GenerateThresholdOutput << std::endl;
    }
}  // end namespace itk
#endif
