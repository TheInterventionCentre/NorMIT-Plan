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
#ifndef itkMultiScaleModifiedVesselnessMeasureImageFilter_hxx
#define itkMultiScaleModifiedVesselnessMeasureImageFilter_hxx

#include "itkMultiScaleModifiedVesselnessMeasureImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenAnalysis.h"
#include "itkImageRegionIterator.h"
#include "itkNumericTraits.h"
#include "itkFixedArray.h"
#include "itkMath.h"

namespace itk
{
    /**
     * Constructor
     */
    template< typename TInputImage, typename TOutputImage >
    MultiScaleModifiedVesselnessMeasureImageFilter < TInputImage, TOutputImage >
    ::MultiScaleModifiedVesselnessMeasureImageFilter()
    {
        // set defaults for member variables 
        m_Gamma = 4.0;
        
        m_SigmaMinimum = 0.5;
        m_SigmaMaximum = 2.0;
        
        m_NumberOfSigmaSteps = 4;
    }
    
    template< typename TInputImage, typename TOutputImage >
    void MultiScaleModifiedVesselnessMeasureImageFilter < TInputImage, TOutputImage >
    ::GenerateData()
    {
        if ( ImageDimension != 3 )
        {
            itkExceptionMacro( "The filter only takes in images with dimension of 3" );
        }
        
        // Allocate the output
        this->GetOutput()->SetBufferedRegion( this->GetOutput()->GetRequestedRegion() );
        this->GetOutput()->Allocate();
        this->GetOutput()->FillBuffer( NumericTraits< OutputPixelType >::ZeroValue() );
        
        typename OutputImageType::Pointer output = this->GetOutput();
        
        double currentSigma = 0.0;
        double vesselnessMeasure = 0.0;
        double S = 0.0;
        const double step = ( std::log( m_SigmaMaximum ) - std::log( m_SigmaMinimum ) ) / double( m_NumberOfSigmaSteps - 1 );
        itk::FixedArray< double, 3 > eigenValues;
        eigenValues[0]= 0.0;
        eigenValues[1]= 0.0;
        eigenValues[2]= 0.0;
        
        typedef HessianRecursiveGaussianImageFilter< InputImageType > HessianFilterType;
        typedef typename HessianFilterType::OutputImageType      HessianImageType;
        typedef typename HessianFilterType::OutputImagePixelType HessianPixelType;
        
        typedef ImageRegionIterator< OutputImageType > ImageRegionIteratorType;
        ImageRegionIteratorType itOutput ( output, output->GetRequestedRegion());
        
        typedef ImageRegionIterator< HessianImageType > HessianImageRegionIteratorType;
        
        for ( unsigned int i = 0; i < m_NumberOfSigmaSteps; i++ )
        {
            currentSigma = std::exp( std::log(m_SigmaMinimum) + double( step * i ) );
            
            typename HessianFilterType::Pointer hessianFilter = HessianFilterType::New();
            hessianFilter->SetSigma( currentSigma );
            hessianFilter->SetInput( this->GetInput() );
            hessianFilter->Update();
            
            HessianImageRegionIteratorType itHessian ( hessianFilter->GetOutput(), hessianFilter->GetOutput()->GetRequestedRegion());
            
            for ( itHessian.GoToBegin(), itOutput.GoToBegin(); !itOutput.IsAtEnd(); ++itOutput, ++itHessian )
            {
                HessianPixelType pixelHessian = itHessian.Get();
                
                typedef SymmetricEigenAnalysis< HessianPixelType, itk::FixedArray< double, 3 > > SymmetricEigenAnalysisType;
                SymmetricEigenAnalysisType symmetricEigenSystem( 3 );
                symmetricEigenSystem.SetOrderEigenMagnitudes( true );
                symmetricEigenSystem.ComputeEigenValues( pixelHessian, eigenValues );
                
                S = std::sqrt( itk::Math::sqr( eigenValues[0] ) + itk::Math::sqr( eigenValues[1] ) + itk::Math::sqr( eigenValues[2] ) );
                
                if ( ( eigenValues[1] > 0.0 ) || ( eigenValues[2] > 0.0 ) )
                {
                    vesselnessMeasure = 0.0;
                }
                else
                {
                    vesselnessMeasure = ( ( 1.0 - ( std::abs( std::abs( eigenValues[1]) - std::abs( eigenValues[2] ) ) ) / ( std::abs( eigenValues[1] ) + std::abs( eigenValues[2] ) ) )
                                         * ( ( ( 2.0 / 3.0 ) * eigenValues[0] ) - eigenValues[1] - eigenValues[2] ) )
                                         * ( 1.0 - std::exp( ( -1.0 * itk::Math::sqr( S ) ) / ( 2.0 * itk::Math::sqr( m_Gamma ) ) ) );
                }
                
                if( vesselnessMeasure > 0.0 )
                {
                    vesselnessMeasure = vesselnessMeasure * std::exp( currentSigma );
                }
                
                itOutput.Set( itOutput.Get() + static_cast< OutputPixelType >( vesselnessMeasure ) );
            }
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void MultiScaleModifiedVesselnessMeasureImageFilter < TInputImage, TOutputImage >
    ::PrintSelf(std::ostream & os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
        
        os << indent << "Gamma:  " << m_Gamma << std::endl;
        os << indent << "SigmaMinimum:  " << m_SigmaMinimum << std::endl;
        os << indent << "SigmaMaximum:  " << m_SigmaMaximum  << std::endl;
        os << indent << "NumberOfSigmaSteps:  " << m_NumberOfSigmaSteps  << std::endl;
    }
}  // end namespace itk
#endif
