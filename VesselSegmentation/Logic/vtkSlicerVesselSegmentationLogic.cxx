/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkSlicerVesselSegmentationLogic.cxx

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


// VesselSegmentation Logic includes
#include "vtkSlicerVesselSegmentationLogic.h"
#include "vtkMRMLVesselSegmentationSeedNode.h"
#include "itkVesselSegmentationPreProcessingFilter.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>

#include <vtkMRMLSelectionNode.h>
#include <vtkMRMLInteractionNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLLabelMapVolumeNode.h>
#include <vtkMRMLLabelMapVolumeDisplayNode.h>
#include <vtkMRMLVolumeNode.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLModelDisplayNode.h>
#include <vtkMRMLDisplayNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkImageData.h>
#include <vtkImageThreshold.h>

#include <vtkImageToStructuredPoints.h>
#include <vtkMarchingCubes.h>
#include <vtkDiscreteMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkImageBlend.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkImageCast.h>

#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>

#include <vtkMRMLViewNode.h>

#include <qSlicerApplication.h>
#include <qSlicerLayoutManager.h>
#include <qMRMLThreeDWidget.h>
#include <qMRMLThreeDView.h>

// ITK includes
#include <itkImageFileWriter.h>
#include <itkImage.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkOrImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkAddImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkLabelImageToShapeLabelMapFilter.h>
#include <itkPoint.h>
#include <itkTimeProbe.h>

// STD includes
#include <iostream>
#include <cmath>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVesselSegmentationLogic);

//----------------------------------------------------------------------------
/**
 * Constructor
 */
vtkSlicerVesselSegmentationLogic::vtkSlicerVesselSegmentationLogic()
{
  hepaticUpdated = false;
  portalUpdated = false;
  mergedUpdated = false;
}

//----------------------------------------------------------------------------
vtkSlicerVesselSegmentationLogic::~vtkSlicerVesselSegmentationLogic()
{

}

//----------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "vtkSlicerVesselSegmentationLogic: " << this->GetClassName() << "\n";

  os << indent << "vtkScalarType: " << this->vtkScalarType << "\n";

  os << indent << "hepaticUpdated: ";
  if(this->hepaticUpdated)
    os << indent << "true" << "\n";
  else
    os << indent << "false" << "\n";
  os << indent << "hepaticITKdata: " << this->hepaticITKdata << "\n";
  os << indent << "hepaticModelNode: " << this->hepaticModelNode << "\n";
  os << indent << "hepaticModelDisplayNode: " << this->hepaticModelDisplayNode << "\n";
  os << indent << "hepaticLabelMap: " << this->hepaticLabelMap << "\n";

  os << indent << "portalUpdated: ";
  if(this->portalUpdated)
    os << indent << "true" << "\n";
  else
    os << indent << "false" << "\n";
  os << indent << "portalITKdata: " << this->portalITKdata << "\n";
  os << indent << "portalModelNode: " << this->portalModelNode << "\n";
  os << indent << "portalModelDisplayNode: " << this->portalModelDisplayNode << "\n";
  os << indent << "portalLabelMap: " << this->portalLabelMap << "\n";

  os << indent << "mergedUpdated: ";
  if(this->mergedUpdated)
    os << indent << "true" << "\n";
  else
    os << indent << "false" << "\n";
  vtkVesselSegmentationHelper::SeedImageType::Pointer mergedITKdata;
  vtkSmartPointer<vtkMRMLModelDisplayNode> mergedModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> mergedLabelMap;

  os << indent << "mergedITKdata: " << this->mergedITKdata << "\n";
  os << indent << "mergedModelDisplayNode: " << this->mergedModelDisplayNode << "\n";
  os << indent << "mergedLabelMap: " << this->mergedLabelMap << "\n";

  os << indent << "onlyOverlapLabelMap: " << this->onlyOverlapLabelMap << "\n";
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  // for placing of seeds may need to watch the cursor position
  //events->InsertNextValue(vtkMRMLCrosshairNode::CursorPositionModifiedEvent);

  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

void vtkSlicerVesselSegmentationLogic::ObserveMRMLScene()
{
  this->Superclass::ObserveMRMLScene();
}

//-----------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::RegisterNodes()
{

}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::UpdateFromMRMLScene()
{

}

//---------------------------------------------------------------------------
/**
 * Node added to MRML Scene
 * set up observers of particular types of nodes
 */
void vtkSlicerVesselSegmentationLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(addedNode))
{

}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{

}

//---------------------------------------------------------------------------
/*
 * Node in scene modified
 */
void vtkSlicerVesselSegmentationLogic::OnMRMLNodeModified(vtkMRMLNode* vtkNotUsed(modifiedNode))
{

}

//---------------------------------------------------------------------------
/**
* Preprocess image for better vesselness (pipeline of ITK filters)
*/
void vtkSlicerVesselSegmentationLogic::PreprocessImage( int lowerThreshold,
                                                        int upperThreshold,
                                                        unsigned int alpha,
                                                        int beta,
                                                        unsigned int conductance,
                                                        unsigned int iterations )
{
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = this->GetActiveVolume();

  // check have valid activevol
  if (!activeVol)
    {
    vtkErrorMacro("SegmentVessels: could not get active volume.")
    return;
    }

  // check again for the mrml scene, since used explicitly in this function
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }

  // Need to cast data the first time to get it into float
  vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
  cast->SetOutputScalarTypeToFloat();
  cast->SetInputData( activeVol->GetImageData() );
  cast->Update();
  // casting is not in place (makes a copy), so switch what data the node is pointing to
  activeVol->SetAndObserveImageData(cast->GetOutput());

  vtkVesselSegmentationHelper::SeedImageType::Pointer itkConvertedImage =
      vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(activeVol);
  if (itkConvertedImage.IsNull() == true )
    {
    vtkErrorMacro("PreprocessImage: conversion to ITK not successful.")
    return;
    }

  // Declare the type of objectness measure image filter
  typedef itk::VesselSegmentationPreProcessingFilter<vtkVesselSegmentationHelper::SeedImageType,
      vtkVesselSegmentationHelper::SeedImageType > VesselPreProcessingFilterType;

  // Create a vesselness Filter
  VesselPreProcessingFilterType::Pointer VesselPreProcessingFilter =
      VesselPreProcessingFilterType::New();

  //Connect to input image
  VesselPreProcessingFilter->SetInput( itkConvertedImage );

  VesselPreProcessingFilter->SetLowerThreshold(lowerThreshold);
  VesselPreProcessingFilter->SetUpperThreshold(upperThreshold);
  VesselPreProcessingFilter->SetAlpha(alpha);
  VesselPreProcessingFilter->SetBeta(beta);
  VesselPreProcessingFilter->SetConductance(conductance);
  VesselPreProcessingFilter->SetNumberOfIterations(iterations);

  // pass everything into function
  itk::TimeProbe clock1;
  clock1.Start();
  VesselPreProcessingFilter->Update();
  clock1.Stop();
  vtkDebugMacro("Time taken for PreProcessing : " << clock1.GetMean() << "sec\n" );

  preprocessedImg = VesselPreProcessingFilter->GetOutput();

  preprocessedImg->ReleaseDataFlagOff();
  preprocessedImg->DisconnectPipeline();

  // at this point it should be a copy of the data? since it is the output of a non in place filter...
  vtkSmartPointer<vtkImageData> tempVtkImageData =
      vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(preprocessedImg);
  if (tempVtkImageData == NULL)
    {
    vtkErrorMacro("PreprocessImage: conversion to ITK not successful.")
    return;
    }

  // Need to cast data to get it to be copied so it doesn't go out of scope
  // if we make more than one preprocessed image
  vtkSmartPointer<vtkImageCast> cast2 = vtkSmartPointer<vtkImageCast>::New();
  cast2->SetOutputScalarTypeToFloat();
  cast2->SetInputData( tempVtkImageData );
  cast2->Update();
  // casting is not in place (makes a copy), so switch what data the node is pointing to

  vtkSmartPointer<vtkMRMLScalarVolumeNode> preprocessedNode =
      vtkVesselSegmentationHelper::ConvertVtkImageDataToVolumeNode(cast2->GetOutput(), preprocessedImg, true);

  preprocessedNode->SetName("preprocessedImage");
  this->GetMRMLScene()->AddNode(preprocessedNode);
}

//---------------------------------------------------------------------------
/**
* Triggered when button to segment vessels is clicked
*/
void vtkSlicerVesselSegmentationLogic::SegmentVesselsFromWidget(bool isHepatic)
{
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast
      (scene->GetNodeByID("vtkMRMLVesselSegmentationSeedNodeSingleton"));

  // check if this seed node satisfies the requirements for segmentation
  if(seedNode != NULL && seedNode->GetIsSeed1Set() && seedNode->GetIsSeed2Set())
    {
    this->SegmentVessels(seedNode, isHepatic);
    // then remove from scene
    scene->RemoveNode(seedNode);
    }
  else
    {
    vtkErrorMacro("Do not have seed pre-requisites for segmentation.");
    return;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SegmentVessels(
    vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic)
{
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = this->GetActiveVolume();

  // check have valid activevol
  if (!activeVol)
    {
    vtkErrorMacro("SegmentVessels: could not get active volume.")
    return;
    }
  /*
   * check if we already have an image converted to ITK
   * if not then create it
   */
  if( this->preprocessedImg.IsNull() )
    {
    this->preprocessedImg = vtkVesselSegmentationHelper::
        ConvertVolumeNodeToItkImage(activeVol);

    if( this->preprocessedImg.IsNull() )
    {
      vtkErrorMacro("SegmentVessels: conversion to ITK not successful, "
          "do not have preprocessed image.")
      return;
    }
  }

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  activeVol->GetIJKToRASMatrix(mat);
  vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
  vtkNew<vtkMatrix4x4> RAStoIJKmatrix;
  IJKtoRASmatrix->DeepCopy(mat);
  RAStoIJKmatrix->DeepCopy(mat);
  RAStoIJKmatrix->Invert();

  // get the pair of seeds in the right format for the itk filter
  double *seed1 = seedNode->GetSeed1();
  double *seed2 = seedNode->GetSeed2();

  const double seed1_0[4] = {seed1[0], seed1[1], seed1[2], 1}; // RAS
  const double seed2_0[4] = {seed1[0], seed2[1], seed2[2], 1}; // RAS

  double *seedIJK1 = new double[4]; // the actual first seed
  double *seedIJK2 = new double[4]; // the direction seed

  // use the ijk matrix to convert the points
  RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed1_0, seedIJK1);
  RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed2_0, seedIJK2);

  vtkVesselSegmentationHelper::Index3D coord1;
  vtkVesselSegmentationHelper::Index3D coord2;

  // the actual first seed
  coord1[0] = seedIJK1[0];
  coord1[1] = seedIJK1[1];
  coord1[2] = seedIJK1[2];
  // the direction seed
  coord2[0] = seedIJK2[0];
  coord2[1] = seedIJK2[1];
  coord2[2] = seedIJK2[2];

  //std::cout << "Seed IJK1 (155, 118, 41): " << coord1[0] << " " << coord1[1] << " " << coord1[2] << std::endl;
  //std::cout << "Seed IJK2 (145, 116, 55): " << coord2[0] << " " << coord2[1] << " " << coord2[2] << std::endl;

  // create filter
  typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegmentationHelper::SeedImageType,
      vtkVesselSegmentationHelper::SeedImageType>  SeedFilterType;
  SeedFilterType::Pointer filter = SeedFilterType::New();

  /*
   * Have what is needed to call segmentation algorithm
   * a pair of seeds and an ITK image
   */
  if(isHepatic == true) // HEPATIC radio button is selected
    {
    // instantiate filter
    itk::TimeProbe clock1;
    clock1.Start();

    vtkDebugMacro( "Seed IJK: " << coord1 << " Direction seed: " << coord2 );

    filter->SetInput(preprocessedImg);
    filter->SetSeed(coord1);
    filter->SetDirectionSeed(coord2);
    filter->SetOutputLabel(4);
    if( this->hepaticITKdata.IsNotNull() )
      {
      filter->SetPreviousOutput(this->hepaticITKdata);
      }
    filter->Update();

    this->hepaticITKdata  = filter->GetOutput();
    clock1.Stop();
    vtkDebugMacro( "Time taken for itk::SeedVesselSegmentationImageFilter : "
        << clock1.GetMean() <<"sec\n" );

    // convert output of filter back to VTK
    vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(this->hepaticITKdata);
    if (outVTKImage.GetPointer()  == NULL )
      {
      vtkErrorMacro("SegmentVessels: conversion to VTK not successful.")
      return;
      }

    if(this->hepaticLabelMap == NULL)
      {
      // first time to create a hepatic label map
      this->hepaticLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
      this->hepaticLabelMap->CopyOrientation(activeVol);
      this->hepaticLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
      this->hepaticLabelMap->SetName("hepaticLabel");
      this->GetMRMLScene()->AddNode(this->hepaticLabelMap);

      // make this label map the selected one
      this->SetAndPropagateActiveLabel(this->hepaticLabelMap);

      // have updated hepatic, but now merged is not up to date
      this->hepaticUpdated = true;
      this->mergedUpdated = false;
      }
    else // already had an hepaticLabelMap
      {
      this->hepaticLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());

      // have updated hepatic, but now merged is not up to date
      this->hepaticUpdated = true;
      this->mergedUpdated = false;
      }
    }
    else // PORTAL radio button is selected
      {
      // instantiate filter
      itk::TimeProbe clock1;
      clock1.Start();
      typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegmentationHelper::SeedImageType,
          vtkVesselSegmentationHelper::SeedImageType>  SeedFilterType;

      vtkDebugMacro( "Seed IJK: " << coord1 << " Direction seed: " << coord2 );

      // Create a seed filter
      SeedFilterType::Pointer filter = SeedFilterType::New();
      filter->SetInput(preprocessedImg);
      filter->SetSeed(coord1);
      filter->SetDirectionSeed(coord2);
      filter->SetOutputLabel(5);
      if( this->portalITKdata.IsNotNull() )
        {
        filter->SetPreviousOutput(this->portalITKdata);
        }
      filter->Update();

      this->portalITKdata  = filter->GetOutput();
      clock1.Stop();
      vtkDebugMacro( "Time taken for itk::SeedVesselSegmentationImageFilter : "
          << clock1.GetMean() <<"sec\n" );

      //----------------------------------------------------------------------------
      // convert output of filter back to VTK
      vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegmentationHelper::
          ConvertItkImageToVtkImageData(this->portalITKdata);

      if (outVTKImage.GetPointer()  == NULL )
        {
        vtkErrorMacro("SegmentVessels: conversion to VTK not successful.")
        return;
        }

      if(this->portalLabelMap == NULL)
        {
        // first time to create a portal label map
        this->portalLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
        this->portalLabelMap->CopyOrientation(activeVol);
        this->portalLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
        this->portalLabelMap->SetName("portalLabel");
        this->GetMRMLScene()->AddNode(this->portalLabelMap);

        // make this label map the selected one
        this->SetAndPropagateActiveLabel(this->portalLabelMap);

        // have updated portal, but now merged is not up to date
        this->portalUpdated = true;
        this->mergedUpdated = false;
        }
      else // already had an portalLabelMap
        {
        this->portalLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());

        // have updated portal, but now merged is not up to date
        this->portalUpdated = true;
        this->mergedUpdated = false;
      }
    }
    // will updated models if data has been modified: portalUpdated or hepaticUpdated
    this->UpdateModels();
}

//---------------------------------------------------------------------------
/**
* Section to do with hepatic and portal splitting
*/
void vtkSlicerVesselSegmentationLogic::MergeLabelMaps()
{
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = this->GetActiveVolume();

  // check have valid activevol
  if (!activeVol)
    {
    vtkErrorMacro("MergeLabelMaps: could not get active volume.")
    return;
    }
  // check if we have the ITK versions of data, and try to convert it if not
  if( this->hepaticITKdata.IsNull() )
    {
    this->hepaticITKdata =
        vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(this->hepaticLabelMap);
    }
  if( this->portalITKdata.IsNull() )
    {
    this->portalITKdata =
        vtkVesselSegmentationHelper::ConvertVolumeNodeToItkImage(this->portalLabelMap);
    }

  // check we have the data needed (if the conversions worked)
  if( this->hepaticITKdata.IsNull() && this->portalITKdata.IsNull() )
    {
    vtkErrorMacro("CallMergeLabelMaps: Do not have 2 label maps.")
    return;
    }
  else if( this->hepaticITKdata.IsNull() )
    {
    vtkErrorMacro("CallMergeLabelMaps: Do not have hepatic label map.")
    return;
    }
  else if( this->portalITKdata.IsNull() )
    {
    vtkErrorMacro("CallMergeLabelMaps: Do not have portal label map.")
    return;
    }

  typedef itk::AddImageFilter<vtkVesselSegmentationHelper::SeedImageType,
      vtkVesselSegmentationHelper::SeedImageType> AddFilterType;
  AddFilterType::Pointer addFilter = AddFilterType::New();
  addFilter->SetInput1(this->hepaticITKdata);
  addFilter->SetInput2(this->portalITKdata);
  addFilter->Update();
  this->mergedITKdata = addFilter->GetOutput();

  // convert output of add filter back to VTK
  vtkSmartPointer<vtkImageData> mergedVTKImage =
      vtkVesselSegmentationHelper::ConvertItkImageToVtkImageData(this->mergedITKdata);

  if (mergedVTKImage.GetPointer()  == NULL )
    {
    vtkErrorMacro("CallMergeLabelMaps: Conversion to VTK not successful.")
    return;
    }

  if(this->mergedLabelMap == NULL)
    {
    // first time to create a hepatic label map
    mergedLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
    mergedLabelMap->CopyOrientation(activeVol);
    mergedLabelMap->SetAndObserveImageData(mergedVTKImage.GetPointer());
    mergedLabelMap->SetName("mergedLabel");
    this->GetMRMLScene()->AddNode(mergedLabelMap);

    // merged label map has been updated
    this->mergedUpdated = true;
    }
  else
    {
    mergedLabelMap->SetAndObserveImageData(mergedVTKImage.GetPointer());

    // merged label map has been updated
    this->mergedUpdated = true;
    }

  this->SetAndPropagateActiveLabel(this->mergedLabelMap);
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SplitVesselsFromWidget(bool isHepatic)
{
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkMRMLVesselSegmentationSeedNode *seedNode =
      vtkMRMLVesselSegmentationSeedNode::SafeDownCast
      (scene->GetNodeByID("vtkMRMLVesselSegmentationSeedNodeSingleton"));

  //check if this seed node satisfies the requirements for splitting
  if(seedNode != NULL && seedNode->GetIsSeed1Set())
    {
    this->SplitVessels(seedNode, isHepatic);
    // then remove from scene
    scene->RemoveNode(seedNode);
    }
  else
    {
    vtkErrorMacro("Do not have seed pre-requisites for splitting.");
    return;
    }
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SplitVessels(
    vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic)
{
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = this->GetActiveVolume();

  // check have valid activevol
  if (!activeVol)
    {
    vtkErrorMacro("SplitVessels: could not get active volume.")
    return;
    }
  // check have a merged label map
  if( this->mergedITKdata.IsNull() )
    {
    vtkErrorMacro("SplitVessels: Do not have merged label map.")
    return;
    }

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  activeVol->GetIJKToRASMatrix(mat);
  vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
  vtkNew<vtkMatrix4x4> RAStoIJKmatrix;
  IJKtoRASmatrix->DeepCopy(mat);
  RAStoIJKmatrix->DeepCopy(mat);
  RAStoIJKmatrix->Invert();

  double *seed1 = seedNode->GetSeed1();

  const double seed1_0[4] = {seed1[0], seed1[1], seed1[2], 1}; // IJK
  // have a seed
  double *seedIJK = new double[4];
  // use the ijk matrix to convert the points
  RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed1_0, seedIJK); //was commented out in old code?

  itk::Point<double, 3> seedLPS;
  seedLPS[0] = - seedIJK[0];
  seedLPS[1] = - seedIJK[1];
  seedLPS[2] = seedIJK[2];

  //Cast image for LabelMap
  typedef itk::Image<LabelType, 3>  LabelImageType;
  typedef itk::CastImageFilter< vtkVesselSegmentationHelper::SeedImageType, LabelImageType > CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput(mergedITKdata);
  castFilter->Update();

  // create image with just overlapping areas (overlapping areas added lower down)
  LabelImageType::Pointer OnlyOverlap = LabelImageType::New();
  OnlyOverlap->CopyInformation(castFilter->GetOutput());
  OnlyOverlap->SetRegions(castFilter->GetOutput()->GetRequestedRegion());
  OnlyOverlap->Allocate();
  OnlyOverlap->FillBuffer(0);

  itk::ImageRegionIteratorWithIndex<LabelImageType> itOrg(castFilter->GetOutput(),
      castFilter->GetOutput()->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<LabelImageType> itOnlyOverlap(OnlyOverlap,
      OnlyOverlap->GetRequestedRegion());

  // Copy only overlapping regions to the image created above
  for(itOrg.GoToBegin(), itOnlyOverlap.GoToBegin(); !itOrg.IsAtEnd(); ++itOrg, ++itOnlyOverlap)
    {
    if(itOrg.Get() == 9)
      {
      itOnlyOverlap.Set( 1 );
      }
    }

  // if first time, or if segmentations have been updated
  if( this->onlyOverlapLabelMap.IsNull() || this->mergedUpdated == true )
    {
    itk::TimeProbe clock1;
    clock1.Start();

    //Structuring Element for Dilation filter
    typedef itk::BinaryBallStructuringElement< LabelType, 3 > StructuringElementType;
    StructuringElementType structuringElement;
    structuringElement.SetRadius(4);
    structuringElement.CreateStructuringElement();

    //Dilation filter for increasing the border size of the object
    typedef itk::BinaryDilateImageFilter <LabelImageType, LabelImageType,
        StructuringElementType>  BinaryDilateImageFilterType;
    BinaryDilateImageFilterType::Pointer dilateFilter = BinaryDilateImageFilterType::New();
    dilateFilter->SetInput(OnlyOverlap);
    dilateFilter->SetKernel(structuringElement);
    dilateFilter->SetBackgroundValue(0);
    dilateFilter->SetForegroundValue(1);

    //Label connected objects separately
    typedef itk::ConnectedComponentImageFilter <LabelImageType, LabelImageType >   ConnectedComponentImageFilterType;
    ConnectedComponentImageFilterType::Pointer connectedComponent =   ConnectedComponentImageFilterType::New ();
    connectedComponent->SetInput(dilateFilter->GetOutput());
    connectedComponent->SetBackgroundValue(0);

    //Convert LabelImage to LabelMap containing connected labelObjects
    typedef itk::LabelImageToShapeLabelMapFilter< LabelImageType, LabelMapType> LabelImageToLabelMapFilterType;
    LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
    labelImageToLabelMapFilter->SetInput(connectedComponent->GetOutput());
    labelImageToLabelMapFilter->SetBackgroundValue( itk::NumericTraits< LabelType >::Zero );
    labelImageToLabelMapFilter->Update();

    clock1.Stop();
    vtkDebugMacro( "Dilation and Connected Components - time taken : " << clock1.GetMean() <<"sec\n" );

    this->onlyOverlapLabelMap = labelImageToLabelMapFilter->GetOutput();
    }

  //Find LabelObject containing Seed
  unsigned int labelObjectNumber = 0;
  double distance = 0.0;
  double minDistance = 100000.0;
  itk::Point<double, 3> objectCentroid;

  // Loop over each connected object to find minimum distance
  for(unsigned int n = 0; n < this->onlyOverlapLabelMap->GetNumberOfLabelObjects(); n++)
    {
    ShapeLabelObjectType * labelObject = this->onlyOverlapLabelMap->GetNthLabelObject(n);

    // check if this connected object is of value 9 in label map
    itOrg.SetIndex(labelObject->GetIndex(0));
    int labelValue = itOrg.Get();

    vtkDebugMacro( "Found a label of: " << labelObject->GetLabel() << " underlying label: "
            << labelValue << " number of pixels: " << labelObject->GetNumberOfPixels() );

    objectCentroid = labelObject->GetCentroid();
    vtkDebugMacro("  coordinates of Centroid :" << objectCentroid );
    distance = seedLPS.EuclideanDistanceTo(objectCentroid);

    if((minDistance > distance) && (labelObject->GetNumberOfPixels() > 100))
      {
      minDistance = distance;
      labelObjectNumber = n;
      vtkDebugMacro( "Distance and label object number set: " << distance << " " << n );
      }
    }

  // assign object
  itk::ImageRegionIteratorWithIndex<vtkVesselSegmentationHelper::SeedImageType>
      itMerged(mergedITKdata, mergedITKdata->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<vtkVesselSegmentationHelper::SeedImageType>
      itHepatic(hepaticITKdata, hepaticITKdata->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<vtkVesselSegmentationHelper::SeedImageType>
      itPortal(portalITKdata, portalITKdata->GetRequestedRegion());

  // Change Selected LabelObject as Hepatic or Portal
  ShapeLabelObjectType * selectedLabelObject = this->onlyOverlapLabelMap->GetNthLabelObject(labelObjectNumber);
  if(isHepatic) // hepatic
    {
    for(unsigned int pixelId = 0; pixelId < selectedLabelObject->Size(); pixelId++)
      {
      itMerged.SetIndex(selectedLabelObject->GetIndex(pixelId));
      if(itMerged.Get()) // merged region exists
        {
          // change the merged label map to hepatic
          itMerged.Set(4);
          // erase the portal in this area
          itPortal.SetIndex(selectedLabelObject->GetIndex(pixelId));
          itPortal.Set(0);
          // ensure the hepatic label map is also correct
          itHepatic.SetIndex(selectedLabelObject->GetIndex(pixelId));
          if(!itHepatic.Get())
          {
            itHepatic.Set(4);
          }
        }
      }
    }
  else // portal
    {
    for(unsigned int pixelId = 0; pixelId < selectedLabelObject->Size(); pixelId++)
      {
      itMerged.SetIndex(selectedLabelObject->GetIndex(pixelId));
      if(itMerged.Get()) // merged region exists
        {
        // change the merged label map to portal
        itMerged.Set(5);
        // erase the hepatic in this area
        itHepatic.SetIndex(selectedLabelObject->GetIndex(pixelId));
        itHepatic.Set(0);
        // ensure the portal label map is also correct
        itPortal.SetIndex(selectedLabelObject->GetIndex(pixelId));
        if(!itPortal.Get())
          {
          itPortal.Set(5);
          }
        }
      }
    }

  this->hepaticUpdated = true;
  this->portalUpdated = true;
  this->UpdateModels();

  this->mergedUpdated = false;
  this->SetAndPropagateActiveLabel(this->mergedLabelMap);
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::UpdateModels()
{
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol = this->GetActiveVolume();

  // check have valid activevol
  if (!activeVol)
    {
    vtkErrorMacro("UpdateModels: could not get active volume.")
    return;
    }

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  activeVol->GetIJKToRASMatrix(mat);
  vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
  vtkNew<vtkMatrix4x4> RAStoIJKmatrix;
  IJKtoRASmatrix->DeepCopy(mat);
  RAStoIJKmatrix->DeepCopy(mat);
  RAStoIJKmatrix->Invert();

  if(this->hepaticLabelMap != NULL && this->hepaticUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsHepatic =
        vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesHepatic =
        vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsHepatic->SetInputData(this->hepaticLabelMap->GetImageData());
    structuredPointsHepatic->Update();
    mCubesHepatic->SetInputConnection(structuredPointsHepatic->GetOutputPort());
    mCubesHepatic->SetValue(0,4);
    mCubesHepatic->Update();

    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
        vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    vtkSmartPointer<vtkTransform> translation =
        vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();

    smoother->SetInputConnection(mCubesHepatic->GetOutputPort());
    smoother->SetNumberOfIterations(10);
    smoother->BoundarySmoothingOff();
    smoother->FeatureEdgeSmoothingOff();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

    translation->SetMatrix(IJKtoRASmatrix.GetPointer());

    transformFilter->SetInputConnection(smoother->GetOutputPort());
    transformFilter->SetTransform(translation);
    transformFilter->Update();

    if( this->hepaticModelNode == NULL )
    {
      // first time to create hepatic model node
      this->hepaticModelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
      this->hepaticModelNode->SetName("hepaticVesselModel");
      this->hepaticModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      this->GetMRMLScene()->AddNode(this->hepaticModelNode);

      // also need a display node
      this->hepaticModelDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
      this->hepaticModelDisplayNode->SetColor(0,0,1); // blue
      this->GetMRMLScene()->AddNode(this->hepaticModelDisplayNode);
      this->hepaticModelNode->SetAndObserveDisplayNodeID(this->hepaticModelDisplayNode->GetID());
    }
    else // add to 3D model, as already have nodes
    {
      this->hepaticModelNode->SetAndObservePolyData(transformFilter->GetOutput());
    }
    this->hepaticUpdated = false;

  }
  if(this->portalLabelMap != NULL && this->portalUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsPortal =
        vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesPortal =
        vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsPortal->SetInputData(this->portalLabelMap->GetImageData());
    structuredPointsPortal->Update();
    mCubesPortal->SetInputConnection(structuredPointsPortal->GetOutputPort());
    mCubesPortal->SetValue(0,5);
    mCubesPortal->Update();

    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother =
        vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    vtkSmartPointer<vtkTransform> translation =
        vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
        vtkSmartPointer<vtkTransformPolyDataFilter>::New();

    smoother->SetInputConnection(mCubesPortal->GetOutputPort());
    smoother->SetNumberOfIterations(10);
    smoother->BoundarySmoothingOff();
    smoother->FeatureEdgeSmoothingOff();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

    translation->SetMatrix(IJKtoRASmatrix.GetPointer());

    transformFilter->SetInputConnection(smoother->GetOutputPort());
    transformFilter->SetTransform(translation);
    transformFilter->Update();

    if( this->portalModelNode == NULL )
    {
      // first time to create portal model node
      this->portalModelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
      this->portalModelNode->SetName("portalVesselModel");
      this->portalModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      this->GetMRMLScene()->AddNode(this->portalModelNode);

      // also need a display node
      this->portalModelDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
      this->portalModelDisplayNode->SetColor(1,0,0); // red
      this->GetMRMLScene()->AddNode(this->portalModelDisplayNode);
      this->portalModelNode->SetAndObserveDisplayNodeID(this->portalModelDisplayNode->GetID());
    }
    else // add to 3D model, as already have nodes
    {
      this->portalModelNode->SetAndObservePolyData(transformFilter->GetOutput());
    }
    this->portalUpdated = false;
  }

  this->Reset3DView();
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::Reset3DView()
{
  if (!this->GetMRMLScene())
    {
    vtkErrorMacro("No MRML scene.");
    return;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  //Hide Labels and cube
  vtkSmartPointer<vtkCollection> views;
  views.TakeReference(scene->GetNodesByClassByName("vtkMRMLViewNode",
                                                       "View1"));
  vtkMRMLViewNode *view = vtkMRMLViewNode::SafeDownCast(views->GetItemAsObject(0));
  if(!view)
    {
    vtkErrorMacro("No view node.");
    return;
    }
  else
    {
    view->SetAxisLabelsVisible(false);
    view->SetBoxVisible(false);
    }

  //Reset the camera (this causes a crash from testing)
  qSlicerApplication *app = qSlicerApplication::application();
  if(!app)
    {
    vtkErrorMacro("No qSlicerApplication.");
    return;
    }

  qSlicerLayoutManager *layoutManager = app->layoutManager();
  if(!layoutManager)
    {
    vtkErrorMacro("No layout manager.");
    return;
    }

  qMRMLThreeDWidget *threeDWidget = layoutManager->threeDWidget(0);
  if(!threeDWidget)
    {
    vtkErrorMacro("No 3D widget.");
    return;
    }

  qMRMLThreeDView *threeDView = threeDWidget->threeDView();
  if(!threeDView )
    {
    vtkErrorMacro("No 3D view.");
    return;
    }

  vtkRenderWindow *renderWindow = threeDView->renderWindow();
  if(!renderWindow )
    {
    vtkErrorMacro("No render window.");
    return;
    }

  vtkRendererCollection *renderers = renderWindow->GetRenderers();
  if(!renderers )
    {
    vtkErrorMacro("No renderers.");
    return;
    }

  for(int i=0; i<renderers->GetNumberOfItems(); i++)
    {
    vtkRenderer *renderer =
      vtkRenderer::SafeDownCast(renderers->GetItemAsObject(i));
    renderer->ResetCamera();
    }
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkSlicerVesselSegmentationLogic::GetActiveVolume()
{
  if(!this->GetMRMLScene())
    {
    vtkErrorMacro("GetActiveVolume: No MRML scene.");
    return NULL;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::
      SafeDownCast(scene->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

  if(selectionNode == NULL)
    {
    vtkErrorMacro("GetActiveVolume: no valid selection node.")
    return NULL;
    }

  char *activeVolID = selectionNode->GetActiveVolumeID();

  if(activeVolID == NULL)
    {
    vtkErrorMacro("GetActiveVolume: no active volume ID.")
    return NULL;
    }

  vtkMRMLScalarVolumeNode* activeVol =
      vtkMRMLScalarVolumeNode::SafeDownCast(scene->GetNodeByID(activeVolID));

  // check have valid image data
  if (!activeVol)
    {
    vtkErrorMacro("GetActiveVolume: no active volume.")
    return NULL;
    }
  if (!activeVol->GetImageData())
    {
    vtkErrorMacro("GetActiveVolume: active volume does not have data.")
    return NULL;
    }

  return activeVol;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetAndPropagateActiveVolume(
    vtkMRMLScalarVolumeNode* activeVol)
{
  if(!this->GetMRMLScene())
    {
    vtkErrorMacro("SetAndPropagateActiveVolume: No MRML scene.");
    return;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::
      SafeDownCast(scene->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

  if(selectionNode == NULL)
    {
    vtkErrorMacro("SetAndPropagateActiveVolume: no valid selection node.")
    return;
    }

  selectionNode->SetActiveVolumeID(activeVol->GetID());

  vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
  if(mrmlAppLogic == NULL)
    {
    vtkErrorMacro("SetAndPropagateActiveVolume: cannot get mrml app logic.")
    return;
    }

  mrmlAppLogic->PropagateVolumeSelection();
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetAndPropagateActiveLabel(
    vtkMRMLLabelMapVolumeNode* activelLabel)
{
  if(!this->GetMRMLScene())
    {
    vtkErrorMacro("SetAndPropagateActiveLabel: No MRML scene.");
    return;
    }
  vtkMRMLScene *scene = this->GetMRMLScene();

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::
      SafeDownCast(scene->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

  if(selectionNode == NULL)
    {
    vtkErrorMacro("SetAndPropagateActiveLabel: no valid selection node.")
    return;
    }

  selectionNode->SetActiveLabelVolumeID(activelLabel->GetID());
  selectionNode->SetReferenceActiveLabelVolumeID(activelLabel->GetID());

  vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
  if(mrmlAppLogic == NULL)
    {
    vtkErrorMacro("SetAndPropagateActiveLabel: cannot get mrml app logic.")
    return;
    }

  mrmlAppLogic->PropagateVolumeSelection();
}

// some functions used in testing
//---------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkSlicerVesselSegmentationLogic::GetPreprocessedITKData()
{
  return this->preprocessedImg;
}

//---------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkSlicerVesselSegmentationLogic::GetHepaticITKData()
{
  return this->hepaticITKdata;
}

//---------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkSlicerVesselSegmentationLogic::GetPortalITKData()
{
  return this->portalITKdata;
}

//---------------------------------------------------------------------------
vtkVesselSegmentationHelper::SeedImageType::Pointer
vtkSlicerVesselSegmentationLogic::GetMergedITKData()
{
  return this->mergedITKdata;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetHepaticLabelMap(
    vtkMRMLLabelMapVolumeNode* labelMap)
{
  this->hepaticLabelMap = labelMap;
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetPortalLabelMap(
    vtkMRMLLabelMapVolumeNode* labelMap)
{
  this->portalLabelMap = labelMap;
}

