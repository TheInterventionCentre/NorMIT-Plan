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
#ifndef itkMultiScaleModifiedVesselnessMeasureImageFilter_h
#define itkMultiScaleModifiedVesselnessMeasureImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
    /** \class MultiScaleModifiedVesselnessMeasureImageFilter
     * \brief A filter to enhance vessel objects in 3D images
     *
     * The modified vesselness is a modification of Frangi's vesselness measure,
     * which is based on the analysis of Hessian eigen system. The filter enhances
     * vessel-like structures in 3 dimensional images.
     * The filter takes an image of a Hessian pixels ( SymmetricSecondRankTensor
     * pixels ) and produces an enhanced image. The Hessian input image can be
     * produced using itk::HessianRecursiveGaussianImageFilter.
     *
     *
     * \par References
     * Rahul P Kumar, Fritz Albregtsen, Martin Reimers, Thomas Langø,
     * Bjørn Edwin, Ole Jakob Elle (2013). 3D multiscale vessel enhancement based
     * centerline extraction of blood vessels. Proc. SPIE 8669, Medical Imaging 2013
     *
     *
     * \author Rahul P Kumar PhD, The Intervention Centre,
     *                            Oslo University Hospital, Norway.
     *
     * \sa HessianToObjectnessMeasureImageFilter
     * \sa MultiScaleHessianBasedMeasureImageFilter
     * \sa Hessian3DToVesselnessMeasureImageFilter
     * \sa HessianSmoothedRecursiveGaussianImageFilter
     * \sa SymmetricEigenAnalysisImageFilter
     * \sa SymmetricSecondRankTensor
     *
     * \ingroup SeedVesselSegmentation
     */
    template< typename TInputImage, typename TOutputImage >
    class MultiScaleModifiedVesselnessMeasureImageFilter:public
    ImageToImageFilter<  TInputImage, TOutputImage >
    {
    public:
        /** Standard class typedefs. */
        typedef MultiScaleModifiedVesselnessMeasureImageFilter Self;
        
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
        itkTypeMacro(MultiScaleModifiedVesselnessMeasureImageFilter,
                     ImageToImageFilter);
        
        /** Set/Get Gamma, the weight corresponding to S
         * (the Frobenius norm of the Hessian matrix, or second-order structureness)
         */
        itkSetMacro(Gamma, double);
        itkGetConstMacro(Gamma, double);
        
        /** Set/Get macros for SigmaMin */
        itkSetMacro(SigmaMinimum, double);
        itkGetConstMacro(SigmaMinimum, double);
        
        /** Set/Get macros for SigmaMax */
        itkSetMacro(SigmaMaximum, double);
        itkGetConstMacro(SigmaMaximum, double);
        
        /** Set/Get macros for Number of Scales */
        itkSetMacro(NumberOfSigmaSteps, unsigned int);
        itkGetConstMacro(NumberOfSigmaSteps, unsigned int);
        
#ifdef ITK_USE_CONCEPT_CHECKING
        // Begin concept checking
        itkConceptMacro( DoubleConvertibleToOutputCheck,
                        ( Concept::Convertible< double, OutputPixelType > ) );
        // End concept checking
#endif
        
    protected:
        MultiScaleModifiedVesselnessMeasureImageFilter();
        ~MultiScaleModifiedVesselnessMeasureImageFilter() {};
        void PrintSelf(std::ostream & os, Indent indent) const ITK_OVERRIDE;
        
        /** Generate Data */
        void GenerateData(void) ITK_OVERRIDE;
        
    private:
        MultiScaleModifiedVesselnessMeasureImageFilter(const Self &); //purposely not
        // implemented
        void operator=(const Self &);                                 //purposely not
        // implemented
        
        double m_Gamma;
        double m_SigmaMinimum;
        double m_SigmaMaximum;
        
        unsigned int m_NumberOfSigmaSteps;
    };
}  //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMultiScaleModifiedVesselnessMeasureImageFilter.hxx"
#endif

#endif
