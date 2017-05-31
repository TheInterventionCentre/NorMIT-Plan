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

#ifndef itkSeedVesselSegmentationImageFilter_hxx
#define itkSeedVesselSegmentationImageFilter_hxx

#include "itkSeedVesselSegmentationImageFilter.h"
#include "itkMultiScaleModifiedVesselnessMeasureImageFilter.h"
#include "itkConnectedRegionDistanceMapImageFilter.h"
#include "itkBinaryThinningImageFilter3D.h"

#include "itkObjectFactory.h"
#include "itkMath.h"
#include "itkPoint.h"
#include "itkMatrix.h"
#include "itkNumericTraits.h"
#include "itkLineIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkSymmetricEigenAnalysis.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkLabelMap.h"
#include "itkCastImageFilter.h"
#include "itkBinaryImageToLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"
#include "itkShapeLabelObject.h"
#include "itkExtractImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkShapedNeighborhoodIterator.h"
#include "itkBinaryBallStructuringElement.h"

namespace itk
{
    /**
     * Constructor
     */
    template< typename TInputImage, typename TOutputImage >
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::SeedVesselSegmentationImageFilter()
    {
        //Set Defaults
        
        this->SetNumberOfIndexedInputs(2);

        m_OutputLabel = 1;
        
        m_DiameterMinimum = 1.0;
        m_DiameterMaximum = 24.0;
        
        m_GenerateCentrelineOutput = false;
        m_AlgorithmDebug           = false;
        
        m_TempOutputImage = InputImageType::New();
        
        m_CentrelineImage = CentrelineImageType::New();
        this->ProcessObject::SetNumberOfIndexedOutputs(2);
        this->ProcessObject::SetNthOutput( 1, m_CentrelineImage.GetPointer() );
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::SetInputImage(const TInputImage* inputImage)
    {
        this->SetNthInput(0, const_cast<TInputImage*>(inputImage));
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::SetPreviousOutput(const TOutputImage* previousOutput)
    {
        this->SetNthInput(1, const_cast<TOutputImage*>(previousOutput));
    }
    
    template< typename TInputImage, typename TOutputImage >
    typename SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::InputImageConstPointer
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::GetInputImage()
    {
        return static_cast< const TInputImage* >
        ( this->ProcessObject::GetInput(0) );
    }
    
    template< typename TInputImage, typename TOutputImage >
    typename SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::OutputImageConstPointer
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::GetPreviousOutput()
    {
        return static_cast< const TOutputImage* >
        ( this->ProcessObject::GetInput(1) );
    }
    
    template< typename TInputImage, typename TOutputImage >
    typename SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::DataObjectPointer
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::MakeOutput(DataObjectPointerArraySizeType idx)
    {
        if ( idx == 1 )
        {
            return CentrelineImageType::New().GetPointer();
        }
        return Superclass::MakeOutput(idx);
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::UnitVector(Index3D & index1, Index3D & index2, Vector3D & direction)
    {
        direction[0] = index2[0] - index1[0];
        direction[1] = index2[1] - index1[1];
        direction[2] = index2[2] - index1[2];
        
        double temp = 0.0;
        if ( ( std::fabs(direction[0]) >= std::fabs(direction[1]) ) && ( std::fabs(direction[0]) >= std::fabs(direction[2]) ) )
        {
            temp = std::fabs(direction[0]);
            direction[0] = direction[0] / temp;
            direction[1] = direction[1] / temp;
            direction[2] = direction[2] / temp;
        }
        else if ( ( std::fabs(direction[1]) >= std::fabs(direction[0]) ) && ( std::fabs(direction[1]) >= std::fabs(direction[2]) ) )
        {
            temp = std::fabs(direction[1]);
            direction[0] = direction[0] / temp;
            direction[1] = direction[1] / temp;
            direction[2] = direction[2] / temp;
        }
        else
        {
            temp = std::fabs(direction[2]);
            direction[0] = direction[0] / temp;
            direction[1] = direction[1] / temp;
            direction[2] = direction[2] / temp;
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::DirectionToOffset(Vector3D & vector, Offset3D & offset)
    {
        offset[0] = ( ( vector[0] > 0.0 ) ? ( ( vector[0] >= 0.5 ) ? ceil(vector[0]) : floor(vector[0]) )
                     : ( ( vector[0] <= -0.5 ) ? floor(vector[0]) : ceil(vector[0]) ) );
        offset[1] = ( ( vector[1] > 0.0 ) ? ( ( vector[1] >= 0.5 ) ? ceil(vector[1]) : floor(vector[1]) )
                     : ( ( vector[1] <= -0.5 ) ? floor(vector[1]) : ceil(vector[1]) ) );
        offset[2] = ( ( vector[2] > 0.0 ) ? ( ( vector[2] >= 0.5 ) ? ceil(vector[2]) : floor(vector[2]) )
                     : ( ( vector[2] <= -0.5 ) ? floor(vector[2]) : ceil(vector[2]) ) );
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::DistanceBetweenIndices(Index3D & index1, Index3D & index2, double & distance)
    {
        itk::Point<double, 3> p1;
        itk::Point<double, 3> p2;
        
        p1[0] = index1[0];
        p1[1] = index1[1];
        p1[2] = index1[2];
        
        p2[0] = index2[0];
        p2[1] = index2[1];
        p2[2] = index2[2];
        
        distance = p2.EuclideanDistanceTo(p1);
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::AngleBetweenVectors( Vector3D & vector1, Vector3D & vector2, double & angle )
    {
        if ((vector1[0] == -vector2[0]) && (vector1[1] == -vector2[1]) && (vector1[2] == -vector2[2]))
        {
            angle = 180.0;
            return;
        }
        
        double cosAngle = 0.0;
        
        double dotProduct = vector1 * vector2;
        
        cosAngle = dotProduct / ( std::sqrt( itk::Math::sqr(vector1[0]) + itk::Math::sqr(vector1[1]) + itk::Math::sqr(vector1[2]) )
                                 * std::sqrt( itk::Math::sqr(vector2[0]) + itk::Math::sqr(vector2[1]) + itk::Math::sqr(vector2[2]) ) );
        
        angle = std::acos( cosAngle ) * ( 180 / itk::Math::pi );
        
        if ( itk::Math::isnan( angle ) )
        {
            angle = 0.0;
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::CreateSubVolume(Index3D & subVolumeCentre, double & radius, InternalImage3DPointer subVolume)
    {
        InputImageConstPointer inputImage = this->GetInputImage();
        
        int localRadius = int(radius) + 1;
        
        InternalImage3DRegionType            region;
        InternalImage3DRegionType::SizeType  size;
        InternalImage3DRegionType::IndexType index;
        
        size[0] = ( localRadius * 2 ) + 1;
        size[1] = ( localRadius * 2 ) + 1;
        size[2] = ( localRadius * 2 ) + 1;
        region.SetSize(size);
        
        index[0] = subVolumeCentre[0] - localRadius;
        index[1] = subVolumeCentre[1] - localRadius;
        index[2] = subVolumeCentre[2] - localRadius;
        region.SetIndex(index);
        
        subVolume->SetRegions(region);
        subVolume->Allocate();
        subVolume->FillBuffer( 0.0f );
        
        Index3D changingPosition;
        
        typedef typename itk::ImageRegionIteratorWithIndex<InternalImageType3D>  InternalImage3DIteratorWithIndexType;
        typedef typename itk::ImageRegionConstIteratorWithIndex<InputImageType>  InputImageConstIteratorWithIndexType;
        
        InputImageConstIteratorWithIndexType itIn(inputImage, inputImage->GetRequestedRegion());
        InternalImage3DIteratorWithIndexType itsv(subVolume, subVolume->GetRequestedRegion());
        
        for (itsv.GoToBegin(); !itsv.IsAtEnd(); ++itsv)
        {
            changingPosition = itsv.GetIndex();
            if (!inputImage->GetRequestedRegion().IsInside(changingPosition))
            {
                itsv.Set(0.0f);
                continue;
            }
            itIn.SetIndex(changingPosition);
            itsv.Set(itIn.Get());
        }
        
        typedef itk::RescaleIntensityImageFilter< InternalImageType3D, InternalImageType3D > RescaleFilter;
        RescaleFilter::Pointer rescale = RescaleFilter::New();
        rescale->SetInput(subVolume);
        rescale->SetOutputMinimum(0.0f);
        rescale->SetOutputMaximum(255.0f);
        rescale->Update();
        
        subVolume = rescale->GetOutput();
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::SeedEigenVectors(Index3D & seed, double & radius, OrthoVecs3D & eigenVectors3D)
    {
        if ( radius < 1.0 )
        {
            radius = 1.0;
        }
        
        InternalImage3DPointer subVolume = InternalImageType3D::New();
        CreateSubVolume( seed, radius, subVolume );
        
        typedef itk::HessianRecursiveGaussianImageFilter<InternalImageType3D> HessianFilter3D;
        typename HessianFilter3D::Pointer hessianFilter3D = HessianFilter3D::New();
        hessianFilter3D->SetSigma( radius );
        hessianFilter3D->SetInput( subVolume );
        hessianFilter3D->Update();
        
        typedef typename HessianFilter3D::OutputImageType      HessianImageType3D;
        typedef typename HessianFilter3D::OutputImagePixelType HessianPixelType3D;
        
        typename HessianImageType3D::Pointer hessianImage = hessianFilter3D->GetOutput();
        
        typedef itk::ImageRegionIteratorWithIndex<HessianImageType3D> HessianImageIteratorType3D;
        HessianImageIteratorType3D itH( hessianImage , hessianImage->GetRequestedRegion() );
        
        itH.SetIndex(seed);
        HessianPixelType3D pixelHessian3D = itH.Get();
        
        itk::FixedArray<double, 3>  eigenValues;
        itk::Matrix<double, 3, 3>   eigenVectors;
        
        typedef itk::SymmetricEigenAnalysis< HessianPixelType3D, itk::FixedArray<double, 3> , itk::Matrix<double, 3, 3> > SymmetricEigenAnalysisType3D;
        SymmetricEigenAnalysisType3D symmetricEigenSystem(3);
        symmetricEigenSystem.SetOrderEigenMagnitudes(true);
        symmetricEigenSystem.ComputeEigenValuesAndVectors( pixelHessian3D, eigenValues, eigenVectors );
        
        eigenVectors3D[0] = eigenVectors[0];
        eigenVectors3D[1] = eigenVectors[1];
        eigenVectors3D[2] = eigenVectors[2];
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::CreateCrossSectionImage(Index3D & ImageCentre, double & radius, OrthoVecs3D & eigenVectors3D,
                              InternalImage2DPointer vesselCrossSectionImage, Pos3DInternalImage2DPointer pos3DCrossSectionImage)
    {
        if (radius < 2.5)
        {
            ++radius;
        }
        double crossSectionSize = radius * 4.0;    //making cross-section size to be bit more than the vessel cross-section size
        
        InternalImage2DRegionType                     region2D;
        typename InternalImage2DRegionType::SizeType  size2D;
        typename InternalImage2DRegionType::IndexType index2D;
        
        size2D[0] = crossSectionSize + 1;          //+1 for centrepixel
        size2D[1] = crossSectionSize + 1;
        region2D.SetSize(size2D);
        index2D[0] = 0;
        index2D[1] = 0;
        region2D.SetIndex(index2D);
        
        vesselCrossSectionImage->SetRegions(region2D);
        vesselCrossSectionImage->Allocate();
        vesselCrossSectionImage->FillBuffer(0.0f);
        
        pos3DCrossSectionImage->SetRegions(region2D);
        pos3DCrossSectionImage->Allocate();
        
        typedef itk::ImageRegionIterator<InternalImageType2D>      ImageIteratorType2D;
        typedef itk::ImageRegionIterator<Pos3DInternalImageType2D> PosImageIteratorType2D;
        ImageIteratorType2D    itCrossSection( vesselCrossSectionImage, vesselCrossSectionImage->GetRequestedRegion() );
        PosImageIteratorType2D itPosImage( pos3DCrossSectionImage, pos3DCrossSectionImage->GetRequestedRegion() );
        
        typedef itk::LinearInterpolateImageFunction<InternalImageType3D, float> LinearInterpolator;
        LinearInterpolator::Pointer interpolator = LinearInterpolator::New();
        interpolator->SetInputImage( this->GetInputImage() );
        
        Index3D currentIndex3D;
        itk::ContinuousIndex<float, 3> tempIndex1;
        itk::ContinuousIndex<float, 3> tempIndex2;
        
        int leftSize, rightSize;                    //Sizes left and right to the centre pixel.
        leftSize = int(crossSectionSize/2);
        if( int(crossSectionSize)%2 == 0 )
            rightSize = leftSize;
        else
            rightSize = leftSize + 1;
        
        itCrossSection.GoToBegin();
        itPosImage.GoToBegin();
        for (int i = -leftSize; i <= rightSize; i++)
        {
            tempIndex1[0] = ImageCentre[0] + ( i * eigenVectors3D[1][0] );
            tempIndex1[1] = ImageCentre[1] + ( i * eigenVectors3D[1][1] );
            tempIndex1[2] = ImageCentre[2] + ( i * eigenVectors3D[1][2] );
            
            for(int j = -leftSize; j <= rightSize; j++)
            {
                tempIndex2[0] = tempIndex1[0] + ( j * eigenVectors3D[2][0] );
                tempIndex2[1] = tempIndex1[1] + ( j * eigenVectors3D[2][1] );
                tempIndex2[2] = tempIndex1[2] + ( j * eigenVectors3D[2][2] );
                
                if (!itCrossSection.IsAtEnd())
                {
                    if (interpolator->IsInsideBuffer(tempIndex2))
                    {
                        itCrossSection.Set(interpolator->EvaluateAtContinuousIndex(tempIndex2));
                        currentIndex3D[0] = int( round(tempIndex2[0]) );
                        currentIndex3D[1] = int( round(tempIndex2[1]) );
                        currentIndex3D[2] = int( round(tempIndex2[2]) );
                        itPosImage.Set(currentIndex3D);
                    }
                    else
                    {
                        itCrossSection.Set(0.0f);
                        currentIndex3D[0] = 0;
                        currentIndex3D[1] = 0;
                        currentIndex3D[2] = 0;
                        itPosImage.Set(currentIndex3D);
                    }
                    
                    ++itCrossSection;
                    ++itPosImage;
                }
            }
        }
        
        //Linear remapping of image intensities
        typedef itk::RescaleIntensityImageFilter<InternalImageType2D, InternalImageType2D> RescaleFilterType;
        RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
        rescaleFilter->SetInput(vesselCrossSectionImage);
        rescaleFilter->SetOutputMinimum(0.0f);
        rescaleFilter->SetOutputMaximum(255.0f);
        rescaleFilter->Update();
        
        vesselCrossSectionImage = rescaleFilter->GetOutput();
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::ComputeRoundness(double & roundness, const InternalImage2DPointer thresholdImage2D)
    {
        typedef typename itk::Image<unsigned char, 2>  UnsignedCharImageType;
        
        typedef itk::CastImageFilter< InternalImageType2D, UnsignedCharImageType > CastFilterType;
        CastFilterType::Pointer castFilter = CastFilterType::New();
        castFilter->SetInput(thresholdImage2D);
        
        typedef unsigned long LabelType;
        
        typedef itk::ShapeLabelObject< LabelType, 2 > LabelObjectType;
        typedef itk::LabelMap< LabelObjectType >      LabelMapType;
        
        typedef itk::BinaryImageToLabelMapFilter< UnsignedCharImageType, LabelMapType > ConverterType;
        ConverterType::Pointer converter = ConverterType::New();
        converter->SetInput( castFilter->GetOutput() );
        converter->SetInputForegroundValue( 100 );
        
        typedef itk::ShapeLabelMapFilter< LabelMapType > ShapeFilterType;
        ShapeFilterType::Pointer shape = ShapeFilterType::New();
        shape->SetInput( converter->GetOutput() );
        shape->Update();
        
        LabelMapType::Pointer labelMap      = converter->GetOutput();
        const LabelObjectType * labelObject = labelMap->GetLabelObject( 1 );
        
        roundness = labelObject->GetRoundness();
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::FindNewCentresAfterBifurcation(InternalImage3DPointer distanceMap, InternalImage3DPointer vesselThreshold3D,
                                     double & radius, std::list<Index3D> & newCentres3D, std::list<double> & newRadius)
    {
        typedef typename itk::Index<2> Index2D;
        
        InternalImageType3D::SizeType  imageSize   = distanceMap->GetLargestPossibleRegion().GetSize();
        InternalImageType3D::IndexType imageOrigin = distanceMap->GetLargestPossibleRegion().GetIndex();
        
        InternalImageType3D::IndexType desiredOrigin;
        InternalImageType3D::SizeType  desiredSize;
        InternalImage3DRegionType      desiredRegion;
        
        //Creating  and Analyzing 6 2D-image sides of 3D Volume
        for (int j=0; j<6; j++)
        {
            int i = int(j/2);
            
            //Find regions containing the 2D bounding images
            desiredOrigin  = imageOrigin;
            desiredSize    = imageSize;
            desiredSize[i] = 0;
            
            if (j % 2)
            {
                desiredOrigin[i] = imageOrigin[i] + (imageSize[i] - 1);
            }
            
            desiredRegion.SetIndex(desiredOrigin);
            desiredRegion.SetSize(desiredSize);
            
            //Extract 2D image from DistanceMap
            typedef itk::ExtractImageFilter< InternalImageType3D, InternalImageType2D > ExtractFilterType;
            ExtractFilterType::Pointer extractFilter = ExtractFilterType::New();
            extractFilter->SetExtractionRegion(desiredRegion);
            extractFilter->SetInput(distanceMap);
            extractFilter->SetDirectionCollapseToIdentity();
            extractFilter->Update();
            InternalImage2DPointer extractDistMapImage = extractFilter->GetOutput();
            
            //Extract 2D image from VesselThreshold
            ExtractFilterType::Pointer extractThresholdFilter = ExtractFilterType::New();
            extractThresholdFilter->SetExtractionRegion(desiredRegion);
            extractThresholdFilter->SetInput(vesselThreshold3D);
            extractThresholdFilter->SetDirectionCollapseToIdentity();
            extractThresholdFilter->Update();
            InternalImage2DPointer extractThresholdImage = extractThresholdFilter->GetOutput();
            
            //Rescale extractThresholdImage to binary 0-1
            typedef itk::RescaleIntensityImageFilter<InternalImageType2D> RescaleFilterType;
            RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
            rescaleFilter->SetInput(extractThresholdImage);
            rescaleFilter->SetOutputMinimum(0.0f);
            rescaleFilter->SetOutputMaximum(1.0f);
            
            //Casting Threshold image to UnsignedChar
            typedef itk::Image<unsigned char, 2>  UnsignedCharImageType2D;
            typedef itk::CastImageFilter< InternalImageType2D, UnsignedCharImageType2D > CastFilterType;
            CastFilterType::Pointer castFilter = CastFilterType::New();
            castFilter->SetInput(rescaleFilter->GetOutput());
            
            //Label connected objects separately
            typedef itk::ConnectedComponentImageFilter <UnsignedCharImageType2D, UnsignedCharImageType2D >   ConnectedComponentImageFilterType;
            ConnectedComponentImageFilterType::Pointer connectedComponent =   ConnectedComponentImageFilterType::New ();
            connectedComponent->SetInput(castFilter->GetOutput());
            
            // The output of this filter is an itk::LabelMap, which contains itk::LabelObject's
            typedef itk::LabelImageToLabelMapFilter <UnsignedCharImageType2D>  LabelImageToLabelMapFilterType;
            LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New ();
            labelImageToLabelMapFilter->SetInput(connectedComponent->GetOutput());
            labelImageToLabelMapFilter->Update();
            
            typedef itk::ImageRegionIteratorWithIndex<InternalImageType2D>  ImageRegionIterator2D;
            ImageRegionIterator2D itDistMap(extractDistMapImage, extractDistMapImage->GetLargestPossibleRegion());
            
            std::list<Index2D> maxPositions2D;
            
            // Loop over each connected object
            for(unsigned int p = 0; p < labelImageToLabelMapFilter->GetOutput()->GetNumberOfLabelObjects(); p++)
            {
                // Get the ith region
                LabelImageToLabelMapFilterType::OutputImageType::LabelObjectType* labelObject = labelImageToLabelMapFilter->GetOutput()->GetNthLabelObject(p);
                
                float max = 0.0f;
                Index2D maxIndex2D;
                // Output the pixels composing the region
                for(unsigned int pixelId = 0; pixelId < labelObject->Size(); pixelId++)
                {
                    itDistMap.SetIndex(labelObject->GetIndex(pixelId));
                    
                    //Find for maximum value and its Index for each region
                    if (max < itDistMap.Get())
                    {
                        max = itDistMap.Get();
                        maxIndex2D = itDistMap.GetIndex();
                    }
                }
                
                //Maximun value's Index saved to list
                maxPositions2D.push_back(maxIndex2D);
                newRadius.push_back((double(max) + 1.0));
            }
            
            //looping through the 2D maximums
            for (std::list<Index2D>::iterator it_maxPos2D = maxPositions2D.begin(); it_maxPos2D != maxPositions2D.end(); it_maxPos2D++)
            {
                Index2D centre2D = *it_maxPos2D;
                Index3D newCentre;
                
                //Translate 2D maximum to 3D volume position
                newCentre[i] = imageOrigin[i];
                if (j%2)
                {
                    newCentre[i] = imageOrigin[i] + (imageSize[i] - 1);
                }
                if (i==0)
                {
                    newCentre[1] = centre2D[0];
                    newCentre[2] = centre2D[1];
                }
                else if (i==1)
                {
                    newCentre[0] = centre2D[0];
                    newCentre[2] = centre2D[1];
                }
                else if (i==2)
                {
                    newCentre[0] = centre2D[0];
                    newCentre[1] = centre2D[1];
                }
                
                newCentres3D.push_back(newCentre);
            }
            
        }//end of for loop on 2D sides of 3D volume
        
        typedef itk::ImageRegionConstIteratorWithIndex<InternalImageType3D> Image3DConstIteratorWithIndexType;
        Image3DConstIteratorWithIndexType it_dist3D(distanceMap, distanceMap->GetLargestPossibleRegion());
        
        typedef itk::ImageRegionConstIteratorWithIndex<OutputImageType> OutputConstIteratorWithIndexType;
        OutputConstIteratorWithIndexType it_output(this->GetOutput(), this->GetOutput()->GetLargestPossibleRegion());
        
        //remove centres that are outside the image region, already segmented locations, and those with radius greater than prevRadius
        std::list<double>::iterator it_radius = newRadius.begin();
        for (std::list<Index3D>::iterator it_centres = newCentres3D.begin(); it_centres != newCentres3D.end(); it_centres++, it_radius++)
        {
            if (!this->GetInputImage()->GetRequestedRegion().IsInside(*it_centres))
            {
                it_centres = newCentres3D.erase(it_centres);
                it_radius = newRadius.erase(it_radius);
                continue;
            }
            it_output.SetIndex(*it_centres);
            if (it_output.Get())
            {
                it_centres = newCentres3D.erase(it_centres);
                it_radius = newRadius.erase(it_radius);
            }
            else if (ceilf(*it_radius) > ceilf(radius))
            {
                it_centres = newCentres3D.erase(it_centres);
                it_radius = newRadius.erase(it_radius);
            }
        }
        
        //remove Centres that too close to each other from the list and remove corresponding radius
        it_radius = newRadius.begin();
        for (std::list<Index3D>::iterator it_centres = newCentres3D.begin(); it_centres != newCentres3D.end(); it_centres++)
        {
            std::list<double>::iterator it_r = it_radius;
            for (std::list<Index3D>::iterator it_c = it_centres; it_c != newCentres3D.end(); it_c++)
            {
                if (it_c != it_centres)
                {
                    double x_dist = itk::Math::sqr( (*it_c)[0] - (*it_centres)[0] );
                    double y_dist = itk::Math::sqr( (*it_c)[1] - (*it_centres)[1] );
                    double z_dist = itk::Math::sqr( (*it_c)[2] - (*it_centres)[2] );
                    double distance = std::sqrt( x_dist + y_dist + z_dist );
                    if (distance <= 0.01)
                    {
                        //Removing duplicates
                        it_c = newCentres3D.erase(it_c);
                        it_r = newRadius.erase(it_r);
                    }
                    else if (ceil(distance) < radius)
                    {
                        //Removing centres closeby
                        it_dist3D.SetIndex(*it_c);
                        float point1 = it_dist3D.Get();
                        
                        it_dist3D.SetIndex(*it_centres);
                        float point2 = it_dist3D.Get();
                        
                        if (point1 > point2)
                        {
                            it_centres = newCentres3D.erase(it_centres);
                            it_radius  = newRadius.erase(it_radius);
                            --it_centres;
                            --it_radius;
                            break;
                        }
                        else
                        {
                            it_c = newCentres3D.erase(it_c);
                            it_r = newRadius.erase(it_r);
                            --it_c;
                            --it_r;
                        }
                    }
                }
                it_r++;
            }
            it_radius++;
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::BifurcationRegionAnalysis(Index3D & bifurcationCentre, double & radius, double & prevRadius, Index3D & prevCentre, OrthoVecs3D & prevEigenVecs3D)
    {
        //Create vesselSubvolume
        InternalImage3DPointer vesselSubVolume = InternalImageType3D::New();
        double volumeRadius = 2.5 * radius;
        CreateSubVolume(bifurcationCentre, volumeRadius, vesselSubVolume);
        
        // TODO: Find smarter way process border regions.
        if ( !this->GetInputImage()->GetLargestPossibleRegion().IsInside(vesselSubVolume->GetLargestPossibleRegion()) )
        {
            return;
        }
        
        
        //Create modofied multilscale vesselness image
        double sigmaMinValue = radius / 4.0;
        double sigmaMaxValue = (2.0 * radius) / 3.0;
        int numberOfSigmaStepsValue = 3;
        typedef itk::MultiScaleModifiedVesselnessMeasureImageFilter<InternalImageType3D, InternalImageType3D > VesselnessFilter;
        VesselnessFilter::Pointer vesselness = VesselnessFilter::New();
        vesselness->SetInput( vesselSubVolume );
        vesselness->SetGamma( 4.0 );
        vesselness->SetSigmaMinimum( sigmaMinValue );
        vesselness->SetSigmaMaximum( sigmaMaxValue );
        vesselness->SetNumberOfSigmaSteps( numberOfSigmaStepsValue );
        
        //Rescale results to 0 - 255 range for next part of pipeline
        typedef itk::RescaleIntensityImageFilter< InternalImageType3D, InternalImageType3D > RescaleFilter;
        RescaleFilter::Pointer rescale = RescaleFilter::New();
        rescale->SetInput(vesselness->GetOutput());
        rescale->SetOutputMinimum(0.0f);
        rescale->SetOutputMaximum(255.0f);
        
        //Create Connected Region DistanceMap from the vesselnessOutput
        typedef itk::ConnectedRegionDistanceMapImageFilter<InternalImageType3D, InternalImageType3D > ConnectedFilter;
        ConnectedFilter::Pointer connectedDistanceMap = ConnectedFilter::New();
        connectedDistanceMap->SetInput( rescale->GetOutput() );
        connectedDistanceMap->SetBypassSigmoid(true);
        connectedDistanceMap->SetSeed(bifurcationCentre);
        connectedDistanceMap->SetIntensityMaximum(255.0f);
        connectedDistanceMap->SetThreshold(25.0f);
        connectedDistanceMap->SetGenerateThresholdOutput(true);
        try
        {
            connectedDistanceMap->Update();
        }
        catch (itk::ExceptionObject &e)
        {
            return;
        }
        
        InternalImage3DPointer distanceMapWithinVessel = connectedDistanceMap->GetOutput();
        InternalImage3DPointer vesselThresholdImage    = connectedDistanceMap->GetThresholdOutput();
        
        typedef itk::ImageRegionConstIterator<InternalImageType3D> Image3DConstIteratorType;
        typedef itk::ImageRegionIterator<InternalImageType3D>      Image3DIteratorType;
        
        if (m_GenerateCentrelineOutput)
        {
            //Obtain the centres and radius along the vessel
            Image3DConstIteratorType itDistanceMap(distanceMapWithinVessel, distanceMapWithinVessel->GetLargestPossibleRegion());
            
            m_CentrelineImage->SetRequestedRegion(distanceMapWithinVessel->GetLargestPossibleRegion());
            Image3DIteratorType itCentreline(m_CentrelineImage, m_CentrelineImage->GetRequestedRegion());
            
            //Binary thining of vessel threhold image to obtain the centreline at the bifurcation region
            typedef itk::BinaryThinningImageFilter3D< InternalImageType3D, InternalImageType3D > ThinningFilterType;
            ThinningFilterType::Pointer thinningFilter = ThinningFilterType::New();
            thinningFilter->SetInput( vesselThresholdImage );
            thinningFilter->Update();
            
            Image3DIteratorType itBinaryThin(thinningFilter->GetOutput(), thinningFilter->GetOutput()->GetRequestedRegion());
            
            //Set centreline and radius from the binary thin and distance map into the Centreline Image
            for (itCentreline.GoToBegin(), itBinaryThin.GoToBegin(); !itBinaryThin.IsAtEnd(); ++itCentreline, ++itBinaryThin)
            {
                itCentreline.Set(itBinaryThin.Get());
            }
            
            for (itDistanceMap.GoToBegin(), itCentreline.GoToBegin(); !itDistanceMap.IsAtEnd(); ++itDistanceMap, ++itCentreline)
            {
                if (itCentreline.Get())
                {
                    itCentreline.Set(itDistanceMap.Get());
                }
            }
        }

        
        //Calculate new centres and radius for next cross section image analysis
        std::list<Index3D> newCentres3D;
        std::list<double>  newRadius;
        FindNewCentresAfterBifurcation(distanceMapWithinVessel, vesselThresholdImage, radius, newCentres3D, newRadius);
        
        
        //Save 3D threshold subVolume to Output Image3D
        Image3DIteratorType itThreshold(vesselThresholdImage, vesselThresholdImage->GetLargestPossibleRegion());
        
        m_TempOutputImage->SetRequestedRegion(distanceMapWithinVessel->GetLargestPossibleRegion());
        Image3DIteratorType itTempOutput(m_TempOutputImage, m_TempOutputImage->GetRequestedRegion());
        
        typename TOutputImage::Pointer outputImage = this->GetOutput();
        outputImage->SetRequestedRegion(distanceMapWithinVessel->GetLargestPossibleRegion());
        Image3DIteratorType itOutput(outputImage, outputImage->GetRequestedRegion());
        
        for (itThreshold.GoToBegin(), itOutput.GoToBegin(), itTempOutput.GoToBegin(); !itThreshold.IsAtEnd(); ++itThreshold, ++itOutput, ++itTempOutput)
        {
            if (itThreshold.Get())
            {
                itOutput.Set(static_cast< OutputImagePixelType >(m_OutputLabel));
                itTempOutput.Set(static_cast< InputImagePixelType >(m_OutputLabel));
            }
        }
        
        if (m_AlgorithmDebug)
        {
            std::cout<<std::endl<<std::endl<<"Bifurcation::"<<std::endl;
        }
        
        std::list<double>::iterator it_radius = newRadius.begin();
        for (std::list<Index3D>::iterator it_centres = newCentres3D.begin(); it_centres != newCentres3D.end(); it_centres++, it_radius++)
        {
            if ((*it_radius) > (m_DiameterMaximum / 2.0))  //Too Large Radius
            {
                continue;
            }
            
            if ((*it_radius) < (m_DiameterMinimum / 2.0))   //Too Small Radius
            {
                continue;
            }
            
            //get unit vector direction of bifurcating vessel
            Vector3D direction;
            UnitVector(bifurcationCentre, (*it_centres), direction);
            
            //get unit vector direction of prevDirection
            Vector3D prevDirection;
            prevDirection = prevEigenVecs3D[0];
            
            //get unit vector direction of prevDirection
            Vector3D prev2newDirection;
            UnitVector(prevCentre, (*it_centres), prev2newDirection);
            
            double angle;
            AngleBetweenVectors(prevDirection, direction, angle);
            
            if (m_AlgorithmDebug)
            {
                std::cout<<std::endl<<"PrevCentre : "<<prevCentre<<" ; "<<"BifCentre : "<<bifurcationCentre<<std::endl;
                std::cout<<"Angle between prevDir and direction : "<<angle<<std::endl;
            }
            
            double anglePrev2New = angle;
            
            AngleBetweenVectors(prev2newDirection, direction, angle);
            
            if (m_AlgorithmDebug)
            {
                std::cout<<"Angle between prev2newDirection and direction : "<<angle<<std::endl;
            }
            
            OrthoVecs3D newEigenVecs3D;
            SeedEigenVectors(*it_centres, *it_radius, newEigenVecs3D);
            
            //align the vessel track direction in the required direction
            angle = 0.0;
            AngleBetweenVectors(newEigenVecs3D[0], direction, angle);
            if (angle > 90.0)
            {
                newEigenVecs3D[0][0] = -newEigenVecs3D[0][0];
                newEigenVecs3D[0][1] = -newEigenVecs3D[0][1];
                newEigenVecs3D[0][2] = -newEigenVecs3D[0][2];
            }
            
            AngleBetweenVectors(newEigenVecs3D[0], prevDirection, angle);
            
            if (m_AlgorithmDebug)
            {
                std::cout<<"Angle between prevDir and NewSeedDir : "<<angle<<std::endl;
            }
            
            if (anglePrev2New > 135.0)
            {
                if (m_AlgorithmDebug)
                {
                    std::cout<<"Exit"<<std::endl;
                }
                continue;
            }
            if (angle > 135.0)
            {
                if (m_AlgorithmDebug)
                {
                    std::cout<<"Exit"<<std::endl;
                }
                continue;
            }

            
            // initialize lists
            // radius
            std::list<double> calculatedRadius;
            // eigenVecs3D
            std::list<OrthoVecs3D> calculatedEigenVecs;
            // slice centre position
            std::list<Index3D> calculatedCentre;
            
            //Add elements to front of the lists
            calculatedRadius.push_front(*it_radius);
            calculatedEigenVecs.push_front(newEigenVecs3D);
            calculatedCentre.push_front(*it_centres);

            double checkRadius = (prevRadius - *it_radius) / prevRadius;  //radius
            
            if (m_AlgorithmDebug)
            {
                std::cout<<"EigenVec : "<<newEigenVecs3D[0]<<" ; Centre : "<< *it_centres<<std::endl;
                std::cout<<"Radius : "<<*it_radius<<" ; Bifurcation Radius : "<<radius<<std::endl;
                std::cout<<"checkRadius = "<<checkRadius<<std::endl;
            }

            //Condition to not consider bifurcating vessels with radius greater than previous vessel trunk
            if (anglePrev2New > 45.0)
            {
                if (checkRadius < 0.0)
                {
                    if (m_AlgorithmDebug)
                    {
                        std::cout<<"Exit"<<std::endl;
                    }
                    continue;
                }
            }
            
            //Call function to track the next cross-sections of vessel
            RunNextCrossSection(calculatedRadius, calculatedCentre, calculatedEigenVecs);
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::RunNextCrossSection(std::list<double> & calculatedRadius, std::list<Index3D> & calculatedCentre,
                          std::list<OrthoVecs3D> & calculatedEigenVecs)
    {
        typedef typename itk::Index<2> Index2D;
        
        //get previous values from lists
        double prevRadius = calculatedRadius.front();
        Index3D prevCentre = calculatedCentre.front();
        OrthoVecs3D prevEigenVecs = calculatedEigenVecs.front();
        
        if (m_AlgorithmDebug)
        {
            std::cout<<std::endl<<calculatedRadius.size()<<std::endl;
        }
        
        //calculate next possible centre position from previous cross-section
        Index3D nextPossibleCentre;
        itk::Offset<3> offset;
        DirectionToOffset(prevEigenVecs[0], offset);
        
        nextPossibleCentre = prevCentre + offset;
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"CrossSection Offset : "<<offset<<std::endl;
        }
        
        if (! this->GetInputImage()->GetRequestedRegion().IsInside(nextPossibleCentre))
        {
            if (!m_AllCentresRadius.empty())
            {
                m_AllCentresRadius.pop_front();
            }
            
            return;
        }
        
        
        //Calculate eigenVectors at the next possible centrePosition
        // TODO: Try limiting the call of seedEigenVectors when within previously calculated region.
        OrthoVecs3D eigenVectors3D;
        SeedEigenVectors(nextPossibleCentre, prevRadius, eigenVectors3D);
        
        //align the vessel track direction in the required direction
        double angle;
        AngleBetweenVectors(eigenVectors3D[0], prevEigenVecs[0], angle);
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"PreviousVec : "<<prevEigenVecs[0]<<" ; EigenVec : "<<eigenVectors3D[0]<<std::endl;
            std::cout<<"Initial Angle : "<<angle<<" ; ";
        }
        
        if (angle > 90.0)
        {
            eigenVectors3D[0][0] = -eigenVectors3D[0][0];
            eigenVectors3D[0][1] = -eigenVectors3D[0][1];
            eigenVectors3D[0][2] = -eigenVectors3D[0][2];
        }
        
        AngleBetweenVectors(eigenVectors3D[0], prevEigenVecs[0], angle);
        if ( (angle > 30.0) && (calculatedRadius.size() > 1) )
        {
            eigenVectors3D = prevEigenVecs;
        }
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"Angle : "<<angle<<std::endl;
        }
        
        
        //Create CrossSectionImage of Vessel with given next possible centre
        InternalImage2DPointer      vesselCrossSectionImage = InternalImageType2D::New();
        Pos3DInternalImage2DPointer pos3DCrossSectionImage  = Pos3DInternalImageType2D::New();
        CreateCrossSectionImage(nextPossibleCentre, prevRadius,  eigenVectors3D, vesselCrossSectionImage, pos3DCrossSectionImage);
        
        Index2D crossSectionCentre;
        crossSectionCentre[0] = int(vesselCrossSectionImage->GetLargestPossibleRegion().GetSize()[0] / 2);
        crossSectionCentre[1] = int(vesselCrossSectionImage->GetLargestPossibleRegion().GetSize()[1] / 2);
        

        double lowerThreshold = 100.0;
        
        
        //Create connected region distanceMap
        typedef itk::ConnectedRegionDistanceMapImageFilter<InternalImageType2D, InternalImageType2D > ConnectedFilter;
        ConnectedFilter::Pointer connectedDistanceMap = ConnectedFilter::New();
        connectedDistanceMap->SetInput( vesselCrossSectionImage );
        connectedDistanceMap->SetAlpha( 35 );
        connectedDistanceMap->SetSeed( crossSectionCentre );
        connectedDistanceMap->SetIntensityMaximum( 255.0 );
        connectedDistanceMap->SetIntensityMinimum( 0.0 );
        connectedDistanceMap->SetThreshold( lowerThreshold );
        connectedDistanceMap->SetGenerateThresholdOutput(true);
        try
        {
            connectedDistanceMap->Update();
        }
        catch (itk::ExceptionObject &e)
        {
            return;
        }
        
        InternalImage2DPointer distanceWithinObject = connectedDistanceMap->GetOutput();
        InternalImage2DPointer thresholdImage2D     = connectedDistanceMap->GetThresholdOutput();
        
        double radius, roundness;
        Index2D centre;
        
        ComputeRoundness(roundness, thresholdImage2D);
        
        typedef itk::MinimumMaximumImageCalculator<InternalImageType2D>   MinMaxCalculatorType;
        MinMaxCalculatorType::Pointer minMaxFilter = MinMaxCalculatorType::New();
        minMaxFilter->SetImage(distanceWithinObject);
        minMaxFilter->Compute();
        
        centre = minMaxFilter->GetIndexOfMaximum();
        radius = minMaxFilter->GetMaximum();
        
        if (radius > (m_DiameterMaximum / 2.0))   //Too Large Radius
        {
            return;
        }
        
        if (radius < (m_DiameterMinimum / 2.0))   //Too Small Radius
        {
            return;
        }
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"PrevRadius : "<<prevRadius<<" ; Current Radius : "<<radius<<std::endl;
        }
        
        
        //Exit when current radius is too large compared to previous radius
        if ( (radius >= (2.0 * prevRadius)) && (calculatedRadius.size() > 3) )  //1
        {
            if (!m_AllCentresRadius.empty())
            {
                m_AllCentresRadius.pop_front();
            }
            if (!m_AllCentresRadius.empty())
            {
                m_AllCentresRadius.pop_front();
            }
            
            return;
        }
        if (radius >= (3.0 * prevRadius))
        {
            if (!m_AllCentresRadius.empty())
            {
                m_AllCentresRadius.pop_front();
            }
            return;
        }
        
        
        //Saving 2D Image to 3D Output Image
        Index3D centre3D;
        
        typedef itk::ImageRegionIteratorWithIndex<InternalImageType2D>      Image2DIteratorType;
        typedef itk::ImageRegionIteratorWithIndex<Pos3DInternalImageType2D> Position3DImage2DIteratorType;
        
        typedef itk::ImageRegionIterator<InternalImageType3D> Image3DIteratorType;
        
        Image2DIteratorType           it_threshold2D (thresholdImage2D, thresholdImage2D->GetLargestPossibleRegion());
        Position3DImage2DIteratorType it_pos3Dimage2D(pos3DCrossSectionImage, pos3DCrossSectionImage->GetLargestPossibleRegion());
        
        Image3DIteratorType it_tempOutput3D(m_TempOutputImage, m_TempOutputImage->GetLargestPossibleRegion());
        
        int crossSectionArea = 0;
        int prevOutputArea = 0;
        
        for (it_threshold2D.GoToBegin(); !it_threshold2D.IsAtEnd(); ++it_threshold2D)
        {
            if (it_threshold2D.Get())
            {
                it_pos3Dimage2D.SetIndex( it_threshold2D.GetIndex() );
                it_tempOutput3D.SetIndex( it_pos3Dimage2D.Get() );
                if (!it_tempOutput3D.Get())
                {
                    it_tempOutput3D.Set(static_cast< InputImagePixelType >(m_OutputLabel));
                }
                else
                {
                    ++prevOutputArea;
                }
                if (centre == it_threshold2D.GetIndex())
                {
                    centre3D = it_pos3Dimage2D.Get();
                }
                ++crossSectionArea;
            }
        }
        
        //Exit when overlapp between the current cross-section and output already written, is too large
        double overlappingArea = 0.0;
        overlappingArea = (double(prevOutputArea)/double(crossSectionArea))*100.0;
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"Overlap Area : "<<overlappingArea<<std::endl;
        }
        
        if ( (overlappingArea > 80.0) && (calculatedRadius.size() > 1) )   //79.0
        {
            if (!m_AllCentresRadius.empty())
            {
                m_AllCentresRadius.pop_front();
            }
            
            return;
        }
        
        
        //Add elements to front of the lists
        calculatedRadius.push_front(radius);
        calculatedEigenVecs.push_front(eigenVectors3D);
        calculatedCentre.push_front(centre3D);
        
        //Add elements to front of the list containing all centres and radius
        C_R currentValues;
        currentValues.c = centre3D;
        currentValues.r = radius;
        m_AllCentresRadius.push_front(currentValues);
        
        if (m_AlgorithmDebug)
        {
            std::cout<<"NewCentre : "<<centre3D<<std::endl;
        }
        
        
        Image3DIteratorType it_output(this->GetOutput(), this->GetOutput()->GetLargestPossibleRegion());
        Image3DIteratorType it_c(m_CentrelineImage, m_CentrelineImage->GetRequestedRegion());
        
        if (m_GenerateCentrelineOutput)
        {
            it_output.SetIndex(centre3D);
        }
        
        
        //Exit when the vessel trunk analysis becomes too long
        if (calculatedRadius.size() >= 200)
        {
            return;
        }
        
        if (calculatedRadius.size() <= 3)
        {
            if (m_GenerateCentrelineOutput)
            {
                if (!it_output.Get())
                {
                    it_c.SetIndex(centre3D);
                    it_c.Set(radius);
                }
            }
            
            //Call function to track the next cross-sections of vessel
            RunNextCrossSection(calculatedRadius, calculatedCentre, calculatedEigenVecs);
        }
        else
        {
            //Setting number of previous cross-section radius to analysed for calculating variance
            int checkLength = 0;
            double sum = 0.0, mean = 0.0, variance = 0.0;
            
            if(calculatedRadius.size() <= 6)     //Avoiding last element in list
                checkLength = calculatedRadius.size() - 1;
            else
                checkLength = 6;
            
            int count = 1;
            
            //Calculate variance of current Radius compared to previous set of radius
            double earlierRadius = 0.0;
            for (std::list<double>::iterator it_radius = calculatedRadius.begin(); it_radius != calculatedRadius.end(); ++it_radius, ++count)
            {
                if (count == 1)
                {
                    continue;
                }
                if (count < 5)
                {
                    earlierRadius = *it_radius;
                }
                if (count > checkLength)
                {
                    break;
                }
                
                sum += *it_radius;
            }
            
            mean  = sum / double(checkLength-1);
            count = 1;
            
            std::list<double>::iterator it_radius = calculatedRadius.begin();
            variance = (mean - *it_radius) * (mean - *it_radius);
            
            if (m_AlgorithmDebug)
            {
                std::cout<<"Variance : "<<variance<<" ; Roundness : "<<roundness<<std::endl;
            }
            
            //Detect sudden change in radius with variance of radius and roundness of the cross-section
            if ( (variance < 0.50) && (roundness >= 0.85) ) //No significant change // (variance < 0.60/0.50) && (roundness >= 0.75)
            {
                if (m_GenerateCentrelineOutput)
                {
                    if (!it_output.Get())
                    {
                        it_c.SetIndex(centre3D);
                        it_c.Set(radius);
                    }
                }
                
                //Call function to track the next cross-sections of vessel
                RunNextCrossSection(calculatedRadius, calculatedCentre, calculatedEigenVecs);
            }
            else                                            //Significant change
            {
                //Exit if the variance is too large
                if (variance > 8)
                {
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    
                    return;
                }
                
                count = 1;
                Index3D prevPrevCentre;
                for (std::list<Index3D>::iterator it_centre = calculatedCentre.begin(); it_centre != calculatedCentre.end(); ++it_centre)
                {
                    if(count == 3)
                    {
                        prevPrevCentre = *it_centre;
                        break;
                    }
                    ++count;
                }
                
                if(prevRadius > radius)
                {
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    
                    //Call Bifurcation Analysis with previousCentre
                    BifurcationRegionAnalysis(prevCentre, prevRadius, earlierRadius, prevPrevCentre, eigenVectors3D);
                }
                else
                {
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    if (!m_AllCentresRadius.empty())
                    {
                        m_AllCentresRadius.pop_front();
                    }
                    
                    //Call Bifurcation Analysis with currentCentre
                    BifurcationRegionAnalysis(centre3D, radius, earlierRadius, prevCentre, eigenVectors3D);
                }
            }
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    bool
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::CalculateFirstSeedRadius( double & radius )
    {
        //Create vesselSubvolume with predefined radius
        InternalImage3DPointer vesselSubVolume = InternalImageType3D::New();
        double maxRadius = (m_DiameterMaximum / 2.0) + 3.0;
        CreateSubVolume(m_Seed, maxRadius, vesselSubVolume);
        
        typedef itk::ConnectedRegionDistanceMapImageFilter<InternalImageType3D, InternalImageType3D > FilterType;
        FilterType::Pointer Filter = FilterType::New();
        Filter->SetInput( vesselSubVolume );
        Filter->SetAlpha( 40 );
        Filter->SetSeed( m_Seed );
        Filter->SetIntensityMaximum( 255.0 );
        Filter->SetIntensityMinimum( 0.0 );
        Filter->SetThreshold( 100.0 );
        
        try
        {
            Filter->Update();
        }
        catch (itk::ExceptionObject &e)
        {
            return false;
        }
        
        typedef itk::NeighborhoodIterator< InternalImageType3D > NeighborhoodIteratorType;
        typename NeighborhoodIteratorType::RadiusType neighborhoodRadius;
        neighborhoodRadius.Fill(1);
        NeighborhoodIteratorType it(neighborhoodRadius, Filter->GetOutput(), Filter->GetOutput()->GetRequestedRegion());
        
        if (!this->GetInputImage()->GetRequestedRegion().IsInside(it.GetBoundingBoxAsImageRegion()))
        {
            std::cout<<"Outside Image Bound on First Seed Radius Calculation"<<std::endl;
            return false;
        }

        //Propagate to the maximum point
        radius = 0.0;
        neighborhoodRadius.Fill(1);
        it.SetLocation( m_Seed );
        bool flag = true;
        bool firstIteration = true;
        while ( flag )
        {
            typename NeighborhoodIteratorType::OffsetType nextMove;
            nextMove.Fill(0);
            
            flag = false;
            
            double max = it.GetCenterPixel();
            int count = 0;
            for (unsigned i = 0; i < it.Size(); i++)
            {
                if ( it.GetPixel(i) > max )
                {
                    max = it.GetPixel(i);
                    nextMove = it.GetOffset(i);
                    flag = true;
                    count++;
                }
            }
            
            if (m_AlgorithmDebug)
            {
                std::cout<<count<<" ";
            }
            
            if ((count <= 2) && !firstIteration)
            {
                break;
            }
            
            firstIteration = false;
            it.SetCenterPixel( 0.0f );
            radius = max;
            it += nextMove;
        }
        
        if (radius > (m_DiameterMaximum / 2.0))
        {
            return false;
        }
        
        if (radius < (m_DiameterMinimum / 2.0))
        {
            return false;
        }
        
        //radius = std::ceil(radius);
        
        return true;
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::SmoothOutput()
    {
        C_R centresAndRadius;
        typedef itk::ShapedNeighborhoodIterator< OutputImageType > ShapedNeighborhoodIteratorType;
        typedef itk::BinaryBallStructuringElement< OutputImagePixelType, 3 >  StructuringElementType;
        typename StructuringElementType::RadiusType elementRadius;
        
        while (!m_AllCentresRadius.empty())
        {
            centresAndRadius = m_AllCentresRadius.front();
            
            Index3D centre = centresAndRadius.c;
            double radius  = double(int(centresAndRadius.r));
            
            if (radius >= 12)
            {
                m_AllCentresRadius.pop_front();
                continue;
            }
            
            elementRadius.Fill(radius);
            StructuringElementType structuringElement;
            structuringElement.SetRadius(elementRadius);
            structuringElement.CreateStructuringElement();
            
            ShapedNeighborhoodIteratorType it(structuringElement.GetRadius(), this->GetOutput(),
                                              this->GetOutput()->GetLargestPossibleRegion());
            ShapedNeighborhoodIteratorType itin(structuringElement.GetRadius(), this->GetInputImage(),
                                              this->GetInputImage()->GetLargestPossibleRegion());
            
            it.CreateActiveListFromNeighborhood(structuringElement);
            it.NeedToUseBoundaryConditionOn();
            
            itin.CreateActiveListFromNeighborhood(structuringElement);
            itin.NeedToUseBoundaryConditionOn();
            
            it.SetLocation(centre);
            itin.SetLocation(centre);
            
            typename ShapedNeighborhoodIteratorType::Iterator i;
            typename ShapedNeighborhoodIteratorType::Iterator in;
            
            Index3D tempIndex;
            
            for (i = it.Begin(), in = itin.Begin(); !i.IsAtEnd(); i++, in++)
            {
                tempIndex = centre + i.GetNeighborhoodOffset();
                if (!this->GetInputImage()->GetRequestedRegion().IsInside(tempIndex))
                {
                    continue;
                }
                if ( (!i.Get()) && (in.Get() >= 100) )
                {
                    i.Set(static_cast< OutputImagePixelType >(m_OutputLabel));
                }
            }
            
            m_AllCentresRadius.pop_front();
        }
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::GenerateData()
    {
        if ( InputImageDimension != 3 )
        {
            itkExceptionMacro( "The filter only takes in images with dimension of 3" );
        }
        
        InputImageConstPointer  inputImage     = this->GetInputImage();
        
        // Allocate the output
        OutputImagePointer output = this->GetOutput();
        output->SetBufferedRegion( this->GetOutput()->GetRequestedRegion() );
        output->Allocate();
        output->FillBuffer( NumericTraits< OutputImagePixelType >::ZeroValue() );
    
        bool additionalSeed = false;
        if ( this->GetPreviousOutput().IsNotNull() )
        {
            additionalSeed = true;
            
            //Initialize pointer for Previous Output
            OutputImageConstPointer previousOutput = this->GetPreviousOutput();
            
            itk::ImageRegionIterator<InputImageType> outputIterator(output, output->GetLargestPossibleRegion());
            itk::ImageRegionConstIterator<OutputImageType> prevOutputIterator(previousOutput, previousOutput->GetLargestPossibleRegion());
            
            //Copy Previous Outuput to Current Output
            for (outputIterator.GoToBegin(), prevOutputIterator.GoToBegin(); !outputIterator.IsAtEnd(); ++outputIterator, ++prevOutputIterator)
            {
                if (prevOutputIterator.Get())
                {
                    outputIterator.Set(prevOutputIterator.Get());
                }
            }
        }
        

        m_CentrelineImage = dynamic_cast< CentrelineImageType * >( this->ProcessObject::GetOutput(1) );
        m_CentrelineImage->SetBufferedRegion( inputImage->GetRequestedRegion()  );
        m_CentrelineImage->CopyInformation(inputImage);
        
        if ( m_GenerateCentrelineOutput )
        {
            m_CentrelineImage->Allocate(true); // initialize buffer to zero
            m_CentrelineImage->FillBuffer( NumericTraits<typename CentrelineImageType::PixelType>::ZeroValue() );
        }
        
        //TempOutputImage Allocation
        m_TempOutputImage->SetRegions(inputImage->GetLargestPossibleRegion());
        m_TempOutputImage->CopyInformation(inputImage);
        m_TempOutputImage->Allocate();
        m_TempOutputImage->FillBuffer(NumericTraits<InputImagePixelType>::ZeroValue());
        
        Vector3D initialTrackDirection;
        UnitVector( m_Seed, m_DirectionSeed, initialTrackDirection);
        
        if (additionalSeed)
        {
            //Go through the indeces on the line connecting the seed and direction seed to find new seed
            itk::LineIterator<OutputImageType> itl(output, m_Seed, m_DirectionSeed);
            for (itl.GoToBegin(); !itl.IsAtEnd(); ++itl)
            {
                if (!itl.Get())
                {
                    m_Seed = itl.GetIndex();
                    break;
                }
            }
        }
        
        double radius;
        bool flag = CalculateFirstSeedRadius( radius );
        if (!flag)
        {
            std::cout<< "No Output Created from First Slice Radius Calculation";
            return;
        }
        
        OrthoVecs3D firstEigenVecs;
        SeedEigenVectors( m_Seed, radius, firstEigenVecs );
        
        if (m_AlgorithmDebug)
        {
            std::cout<<std::endl<<"Seed : "<<m_Seed<<" ; Direction Seed : "<<m_DirectionSeed;
            std::cout<<std::endl<<"Init Direction : "<<initialTrackDirection;
        }
        
        //align the vessel track direction in the direction of user's request
        double angle;
        AngleBetweenVectors( firstEigenVecs[0], initialTrackDirection, angle );
        
        if (angle > 90.0)
        {
            firstEigenVecs[0][0] = -firstEigenVecs[0][0];
            firstEigenVecs[0][1] = -firstEigenVecs[0][1];
            firstEigenVecs[0][2] = -firstEigenVecs[0][2];
        }
        
        if (m_AlgorithmDebug)
        {
            std::cout<<" ; New Direction : "<<firstEigenVecs[0]<<std::endl<<"Angle between direction : "<<angle;
        }
        
        AngleBetweenVectors( firstEigenVecs[0], initialTrackDirection, angle );
        
        if (m_AlgorithmDebug)
        {
            std::cout<<" ; Angle after change : "<<angle<<std::endl<<"Radius : "<<radius;
        }
        
        //compute new start seed to adjust for the change in RunNextCrossSection function
        Index3D startSeed;
        Offset3D offset;
        DirectionToOffset( firstEigenVecs[0], offset );
        startSeed = m_Seed - offset;
        
        if (m_AlgorithmDebug)
        {
            std::cout<<" Offset : "<<offset<<std::endl<<std::endl;
        }
        
        // initialize lists
        // radius
        std::list<double> calculatedRadius;
        // eigenVecs3D
        std::list<OrthoVecs3D> calculatedEigenVecs;
        // slice centre position
        std::list<Index3D> calculatedCentre;
        
        //Add elements to front of the lists
        calculatedRadius.push_front( radius );
        calculatedEigenVecs.push_front( firstEigenVecs );
        calculatedCentre.push_front( startSeed );
        
        //Call function to track the next cross-sections of vessel
        RunNextCrossSection(calculatedRadius, calculatedCentre, calculatedEigenVecs);
        
        //Call function to make spherical label at all centres with known radius at the centre
        SmoothOutput();
    }
    
    /** Get the Centreline Image Output */
    template< typename TInputImage, typename TOutputImage >
    typename SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::CentrelineImageType *
    SeedVesselSegmentationImageFilter< TInputImage, TOutputImage >
    ::GetCentrelineOutput()
    {
        return dynamic_cast< CentrelineImageType * >( this->ProcessObject::GetOutput(1) );
    }
    
    template< typename TInputImage, typename TOutputImage >
    void
    SeedVesselSegmentationImageFilter < TInputImage, TOutputImage >
    ::PrintSelf(std::ostream & os, Indent indent) const
    {
        Superclass::PrintSelf(os, indent);
        
        os << indent << "Seed:  " << m_Seed << std::endl;
        os << indent << "DirectionSeed:  " << m_DirectionSeed << std::endl;
    }
}// end namespace

#endif
