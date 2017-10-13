/*=========================================================================
  Program: NorMIT-Plan
  Module: itkSeedVesselSegmentationImageFilter.h

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

#ifndef itkSeedVesselSegmentationImageFilter_h
#define itkSeedVesselSegmentationImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"
#include "itkIndex.h"
#include "itkOffset.h"
#include "itkVector.h"
#include "itkFixedArray.h"

namespace itk
{
    /** \class SeedVesselSegmentationImageFilter
     * \brief A filter to enhance vessel objects in 3D images
     *
     * The filter is used for vessel segmentation using seed based local structure 
     * analysis. The method starts with user defined seedPoint. Trakes centre 
     * along blood vessel by 2d vessel cross-section analysis. When a change in 
     * radius is detected, bifurcation is assumed. At bifurcation, multiscale 3D
     * analysis is performed to find centres of the bifurcating vessels. The 
     * tracking continues till no more vessel is detected.
     *
     *
     * \param References
     * \parblock
     * Kumar RP, Albregtsen F, Reimers M, Langø T, Edwin B, Elle OJ (2013). 
     * 3D multiscale vessel enhancement based centerline extraction of blood vessels.
     * Proc. SPIE 8669, Medical Imaging 2013.
     *
     * Kumar RP, Albregtsen F, Reimers M, Edwin B, Langø T, Elle OJ (2015).
     * Blood vessel segmentation and centerline tracking using local structure analysis. 
     * In6th European conference of the international federation for medical and 
     * biological engineering 2015 (pp. 122-125). Springer International Publishing.
     * \endparblock
     *
     * \author Rahul P Kumar PhD, The Intervention Centre,
     *                            Oslo University Hospital, Norway.
     *
     *
     * \ingroup SeedVesselSegmentation
     */
    template< typename TInputImage, typename TOutputImage >
    class SeedVesselSegmentationImageFilter: public ImageToImageFilter< TInputImage, TOutputImage >
    {
    public:
        /** Standard class typedefs. */
        typedef SeedVesselSegmentationImageFilter               Self;
        typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
        typedef SmartPointer< Self >                            Pointer;
        typedef SmartPointer< const Self >                      ConstPointer;
        
        /** Method for creation through the object factory. */
        itkNewMacro(Self);
        
        /** Run-time type information (and related methods). */
        itkTypeMacro(SeedVesselSegmentationImageFilter, ImageToImageFilter);
        
        typedef TInputImage                            InputImageType;
        typedef typename InputImageType::Pointer       InputImagePointer;
        typedef typename InputImageType::ConstPointer  InputImageConstPointer;
        typedef typename InputImageType::RegionType    InputImageRegionType;
        typedef typename InputImageType::PixelType     InputImagePixelType;
        
        typedef TOutputImage                           OutputImageType;
        typedef typename OutputImageType::Pointer      OutputImagePointer;
        typedef typename OutputImageType::ConstPointer OutputImageConstPointer;
        typedef typename OutputImageType::RegionType   OutputImageRegionType;
        typedef typename OutputImageType::PixelType    OutputImagePixelType;
        
        typedef typename itk::Image<float, 3>             InternalImageType3D;
        typedef typename InternalImageType3D::Pointer     InternalImage3DPointer;
        typedef typename InternalImageType3D::RegionType  InternalImage3DRegionType;
        
        typedef typename itk::Image<float, 2>             InternalImageType2D;
        typedef typename InternalImageType2D::Pointer     InternalImage2DPointer;
        typedef typename InternalImageType2D::RegionType  InternalImage2DRegionType;
        
        itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
        itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);
        
        typedef typename itk::Offset<3>             Offset3D;
        typedef typename itk::Index<3>              Index3D;
        typedef typename itk::Vector<double,3>      Vector3D;
        
        typedef typename itk::FixedArray<Vector3D, 3> OrthoVecs3D;
        
        typedef typename itk::Image<Index3D, 2>                Pos3DInternalImageType2D;
        typedef typename Pos3DInternalImageType2D::Pointer     Pos3DInternalImage2DPointer;
        typedef typename Pos3DInternalImageType2D::RegionType  Pos3DInternalImage2DRegionType;
        
        /** Types for Threshold image. */
        //typedef unsigned char  ThresholdPixelType;
        typedef Image< InputImagePixelType, itkGetStaticConstMacro(InputImageDimension) > CentrelineImageType;
        
        /** Get the Thresholded Image of Intput Image. */
        CentrelineImageType * GetCentrelineOutput();
        
        typedef typename Superclass::DataObjectPointer DataObjectPointer;

        /** The Input image where the vessel segmentation is performed.*/
        void SetInputImage(const TInputImage* inputImage);
        
        /** The previous output which is added into new output image at the beginning.*/
        void SetPreviousOutput(const TOutputImage* previousOutput);
        
        /** Set/Get macros for Seed 
         * Seed is the start index for the vessel segmentation.
         */
        itkSetMacro(Seed, Index3D);
        itkGetConstMacro(Seed, Index3D);
        
        /** Set/Get macros for DirectionSeed 
         * DirectionSeed is the second index that represents the
         * direction for vessel segmentation tracking.
         */
        itkSetMacro(DirectionSeed, Index3D);
        itkGetConstMacro(DirectionSeed, Index3D);
        
        /** Set/Get macros for OutputLabel
         * OutputLabel sets the label value in the OutputImage.
         */
        itkSetMacro(OutputLabel, unsigned int);
        itkGetConstMacro(OutputLabel, unsigned int);

        /** Set/Get macros for RadiusMinimum */
        itkSetMacro(DiameterMinimum, double);
        itkGetConstMacro(DiameterMinimum, double);
        
        /** Set/Get macros for RadiusMaximum */
        itkSetMacro(DiameterMaximum, double);
        itkGetConstMacro(DiameterMaximum, double);
        
        /** Methods to turn on/off flag to generate a
         * Centreline Image */
        itkSetMacro(GenerateCentrelineOutput, bool);
        itkGetConstMacro(GenerateCentrelineOutput, bool);
        itkBooleanMacro(GenerateCentrelineOutput);
        
        /** Methods to turn on/off flag to generate a
         * Debug messages */
        itkSetMacro(AlgorithmDebug, bool);
        itkGetConstMacro(AlgorithmDebug, bool);
        itkBooleanMacro(AlgorithmDebug);
        
        /** This is overloaded to create the Threshold output image */
        typedef ProcessObject::DataObjectPointerArraySizeType DataObjectPointerArraySizeType;
        
#ifdef ITK_USE_CONCEPT_CHECKING
        // Begin concept checking
        itkConceptMacro( OutputEqualityComparableCheck, ( Concept::EqualityComparable< OutputImagePixelType > ) );
        itkConceptMacro( InputEqualityComparableCheck, ( Concept::EqualityComparable< InputImagePixelType > ) );
        itkConceptMacro( InputConvertibleToOutputCheck, ( Concept::Convertible< InputImagePixelType, OutputImagePixelType > ) );
        itkConceptMacro( SameDimensionCheck, ( Concept::SameDimension< InputImageDimension, OutputImageDimension > ) );
        itkConceptMacro( IntConvertibleToInputCheck, ( Concept::Convertible< int, InputImagePixelType > ) );
        itkConceptMacro( OutputOStreamWritableCheck, ( Concept::OStreamWritable< OutputImagePixelType > ) );
        // End concept checking
#endif
        
    protected:
        SeedVesselSegmentationImageFilter();
        ~SeedVesselSegmentationImageFilter(){}
        
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        
        InputImageConstPointer   GetInputImage();
        OutputImageConstPointer  GetPreviousOutput();
        
        /** Does the real work. */
        void GenerateData() ITK_OVERRIDE;
        
        using Superclass::MakeOutput;
        virtual DataObjectPointer MakeOutput(DataObjectPointerArraySizeType idx) ITK_OVERRIDE;
        
    private:
        SeedVesselSegmentationImageFilter(const Self &); //purposely not implemented
        void operator=(const Self &);                    //purposely not implemented
        
        Index3D m_Seed;
        Index3D m_DirectionSeed;
        
        unsigned int m_OutputLabel;

        double m_DiameterMinimum;
        double m_DiameterMaximum;
        
        bool m_GenerateCentrelineOutput;
        bool m_AlgorithmDebug;
        
        typename CentrelineImageType::Pointer m_CentrelineImage;
        
        InputImagePointer m_TempOutputImage;
        
        //Data For final label adjustment
        typedef struct Centre_Radius
        {
            Index3D c;
            float r;
        } C_R;
        std::list< C_R > m_AllCentresRadius;
        
        /** Function to find unit vector direction from index1 to index2:- 
         * direction = index2 -index1. 
         */
        void UnitVector(Index3D & index1, Index3D & index2, Vector3D & direction);
        
        /** Function to reduce direction to index offset. */
        void DirectionToOffset(Vector3D & vector, Offset3D & offset);
        
        /** Function to find distance between indices. */
        void DistanceBetweenIndices(Index3D & index1, Index3D & index2, double & distance);
        
        /** Function to calculate angle between vectors. */
        void AngleBetweenVectors(Vector3D & vector1, Vector3D & vector2, double & angle);
        
        /** Function to create RegionOfInterest Volume. */
        void CreateSubVolume(Index3D & subVolumeCentre, double & radius, InternalImage3DPointer subVolume);
        
        /** Function to calculate eigenVectors at a vessel location. */
        void SeedEigenVectors(Index3D & seed, double & radius, OrthoVecs3D & eigenVectors3D);
        
        /*
         * Function to create CrossSection Image at a given location and radius.
         * Inputs:- ImageCentre, radius, eigenVectors3D
         * Outputs:- vesselCrossSectionImage, pos3DCrossSectionImage
         */
        void CreateCrossSectionImage(Index3D & ImageCentre, double & radius, OrthoVecs3D & eigenVectors3D,
                                     InternalImage2DPointer vesselCrossSectionImage, Pos3DInternalImage2DPointer pos3DCrossSectionImage);
        
        /** Function to calculate the Roundness of the cross-section. */
        void ComputeRoundness(double & roundness, const InternalImage2DPointer thresholdImage2D);
        
        /** Function to find new centres after bifurcation to proceed vessel segmentation. */
        void FindNewCentresAfterBifurcation(InternalImage3DPointer distanceMap, InternalImage3DPointer vesselThreshold3D,
                                            double & radius, std::list<Index3D> & newCentres3D, std::list<double> & newRadius);
        
        /** Function to analysis the area of bifurcation. */
        void BifurcationRegionAnalysis(Index3D & bifurcationCentre, double & radius, double & prevRadius, Index3D & prevCentre, OrthoVecs3D & preEignVecs3D);
        
        /** Function to Run processes for analysing next cross-section of vessel
         * Here the function calls itself till end of segmentation. 
         */
        void RunNextCrossSection(std::list<double> & calculatedRadius, std::list<Index3D> & calculatedCentre,
                                 std::list<OrthoVecs3D> & calculatedEigenVecs);
        
        /** Function to calculate radius at the first seed location. */
        bool CalculateFirstSeedRadius( double & radius );
        
        /** Function to make sperical label maps around all centres. */
        void SmoothOutput();
    };
} //end namespace ITK


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSeedVesselSegmentationImageFilter.hxx"
#endif

#endif 
