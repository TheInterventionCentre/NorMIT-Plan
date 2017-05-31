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
#ifndef itkConnectedRegionDistanceMapImageFilter_h
#define itkConnectedRegionDistanceMapImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkIndex.h"
#include "itkSigmoidImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkFastChamferDistanceImageFilter.h"
#include "itkIsoContourDistanceImageFilter.h"

namespace itk
{
    /** \class ConnectedRegionDistanceMapImageFilter
     * \brief A filter to compute distaceMp within the selected region
     *
     * The filter contains a mini-pipeline of four filters. The filter
     * lets the user select a seed on the input image at the desired 
     * object of interest and ouputs an image containing the distanceMap
     * of that object to its borders. The also gives the possibility to 
     * output Thresholded image seperatly in unsigned char format.
     *
     * \author Rahul P Kumar PhD, The Intervention Centre,
     *                            Oslo University Hospital, Norway.
     *
     * \sa SigmoidImageFilter
     * \sa ConnectedThresholdImageFilter
     * \sa IsoContourDistanceImageFilter
     * \sa FastChamferDistanceImageFilter
     * \sa ApproximateSignedDistanceMapImageFilter
     *
     * \ingroup SeedVesselSegmentation
     */
    template< typename TInputImage, typename TOutputImage >
    class ConnectedRegionDistanceMapImageFilter:public
    ImageToImageFilter<  TInputImage, TOutputImage >
    {
    public:
        /** Standard class typedefs. */
        typedef ConnectedRegionDistanceMapImageFilter Self;
        
        typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
        
        typedef SmartPointer< Self >       Pointer;
        typedef SmartPointer< const Self > ConstPointer;
        
        typedef typename Superclass::InputImageType  InputImageType;
        typedef typename Superclass::OutputImageType OutputImageType;
        typedef typename InputImageType::PixelType   InputPixelType;
        typedef typename OutputImageType::PixelType  OutputPixelType;
        typedef typename OutputImageType::RegionType OutputImageRegionType;
        
        /** Type of input image size and size value */
        typedef typename OutputImageType::SizeType     OutputSizeType;
        typedef typename OutputSizeType::SizeValueType OutputSizeValueType;
        
        /** Image dimension. */
        itkStaticConstMacro(ImageDimension, unsigned int,
                            InputImageType ::ImageDimension);
        
        /** Types for Threshold image. */
        //typedef unsigned char  ThresholdPixelType;
        typedef Image< InputPixelType, itkGetStaticConstMacro(ImageDimension) > ThresholdImageType;
        
        /** Get the Thresholded Image of Intput Image. */
        ThresholdImageType * GetThresholdOutput();
        
        typedef typename Superclass::DataObjectPointer DataObjectPointer;
        
        /** Method for creation through the object factory. */
        itkNewMacro(Self);
        
        /** Runtime information support. */
        itkTypeMacro(ConnectedRegionDistanceMapImageFilter,
                     ImageToImageFilter);
        
        /** Set Seed index to select the connected region. */
        itkSetMacro(Seed, Index< ImageDimension >);
        itkGetConstMacro(Seed, Index< ImageDimension >);
        
        /** Set Intenstiy Minimum. */
        itkSetMacro(IntensityMinimum, InputPixelType);
        itkGetConstMacro(IntensityMinimum, InputPixelType);
        
        /** Set Intenstiy Maximum. */
        itkSetMacro(IntensityMaximum, InputPixelType);
        itkGetConstMacro(IntensityMaximum, InputPixelType);
        
        /** Set Threshold Intenstiy. */
        itkSetMacro(Threshold, InputPixelType);
        itkGetConstMacro(Threshold, InputPixelType);
        
        /** Set Alpha for SimoidImageFilter */
        itkSetMacro(Alpha, InputPixelType);
        itkGetConstMacro(Alpha, InputPixelType);
        
        /** Set Beta for SimoidImageFilter */
        itkSetMacro(Beta, InputPixelType);
        itkGetConstMacro(Beta, InputPixelType);
        
        /** Methods to turn on/off flag to bypass
         * Sigmoid filter */
        itkSetMacro(BypassSigmoid, bool);
        itkGetConstMacro(BypassSigmoid, bool);
        itkBooleanMacro(BypassSigmoid);
        
        /** Methods to turn on/off flag to generate a
         * Threshold Image */
        itkSetMacro(GenerateThresholdOutput, bool);
        itkGetConstMacro(GenerateThresholdOutput, bool);
        itkBooleanMacro(GenerateThresholdOutput);
        
        /** This is overloaded to create the Threshold output image */
        typedef ProcessObject::DataObjectPointerArraySizeType DataObjectPointerArraySizeType;
        
#ifdef ITK_USE_CONCEPT_CHECKING
        // Begin concept checking
        itkConceptMacro( DoubleConvertibleToOutputCheck,
                        ( Concept::Convertible< double, OutputPixelType > ) );
        // End concept checking
#endif
        
    protected:
        ConnectedRegionDistanceMapImageFilter();
        ~ConnectedRegionDistanceMapImageFilter() {};
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        
        /** Generate Data */
        void GenerateData(void) ITK_OVERRIDE;
                
        using Superclass::MakeOutput;
        virtual DataObjectPointer MakeOutput(DataObjectPointerArraySizeType idx) ITK_OVERRIDE;
        
    private:
        ConnectedRegionDistanceMapImageFilter(const Self &); //purposely not
        // implemented
        void operator=(const Self &);                                 //purposely not
        // implemented
        
        typedef SigmoidImageFilter< InputImageType, InputImageType >                 SigmoidType;
        typedef ConnectedThresholdImageFilter<InputImageType, ThresholdImageType>    ConnectedType;
        typedef IsoContourDistanceImageFilter< ThresholdImageType, OutputImageType > IsoContourType;
        typedef FastChamferDistanceImageFilter< OutputImageType, OutputImageType >   ChamferType;
        
        typename SigmoidType::Pointer    m_SigmoidFilter;
        typename ConnectedType::Pointer  m_ConnectedFilter;
        typename IsoContourType::Pointer m_IsoContourFilter;
        typename ChamferType::Pointer    m_ChamferFilter;
        
        Index< ImageDimension > m_Seed;
        
        InputPixelType m_IntensityMinimum;
        InputPixelType m_IntensityMaximum;
        InputPixelType m_Threshold;
        InputPixelType m_Alpha;
        InputPixelType m_Beta;
        
        bool m_GenerateThresholdOutput;
        bool m_BypassSigmoid;
    };
}  //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkConnectedRegionDistanceMapImageFilter.hxx"
#endif

#endif
