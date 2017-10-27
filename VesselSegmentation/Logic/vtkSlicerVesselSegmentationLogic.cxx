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
#include <cassert>
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
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
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
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
/**
 * Node added to MRML Scene
 * set up observers of particular nodes
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

void vtkSlicerVesselSegmentationLogic::SetActiveVolumeNode(vtkMRMLVolumeNode *activeNode)
{
  vtkSetMRMLNodeMacro(this->ActiveVolumeNode, activeNode);

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
  //vtkMRMLDisplayNode *displayNode = vtkMRMLDisplayNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLDisplayNodeSingleton"));

  char *activeVolID = selectionNode->GetActiveVolumeID();
  this->activeVol = vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(activeVolID));
  std::cout << "LOGIC - set active volume node: " << activeNode << std::endl;

  if (this->activeVol != NULL)
  {
    selectionNode->SetActiveVolumeID(activeNode->GetID());
  }

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  this->activeVol->GetIJKToRASMatrix(mat);
  this->IJKtoRASmatrix->DeepCopy(mat);
  this->RAStoIJKmatrix->DeepCopy(mat);
  this->RAStoIJKmatrix->Invert();
}

/**
 * Set Active volume when use dropdown
 */
void vtkSlicerVesselSegmentationLogic::SetActiveVolume(vtkMRMLScalarVolumeNode *activeVolume)
{
  std::cout << "LOGIC - set active Volume: " << activeVolume << std::endl;
  this->activeVol = activeVolume;

  vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
  this->activeVol->GetIJKToRASMatrix(mat);
  this->IJKtoRASmatrix->DeepCopy(mat);
  this->RAStoIJKmatrix->DeepCopy(mat);
  this->RAStoIJKmatrix->Invert();

  // TODO: propagate volume selection so see in 3 views
}

vtkSmartPointer<vtkMRMLScalarVolumeNode> vtkSlicerVesselSegmentationLogic::GetActiveVolume()
{
  return this->activeVol;
}

//-------------------------------------------------------------------------------
void vtkSlicerVesselSegmentationLogic::AddSeedNode()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene *scene = this->GetMRMLScene();

  // Add a seed node
  vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> seedNode =
    vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();
  scene->AddNode(seedNode);
}

//---------------------------------------------------------------------------
/**
* Call preprocessing (pipeline of a bunch of ITK filters)
*/
void vtkSlicerVesselSegmentationLogic::PreprocessImage( int lowerThreshold, int upperThreshold, unsigned int alpha, int beta, unsigned int conductance, unsigned int iterations )
{
  // check have valid image data
  if (!this->activeVol)
  {
    vtkErrorMacro("PreprocessImage: no active volume.")
    return;
  }
  if (!this->activeVol->GetImageData())
  {
    vtkErrorMacro("PreprocessImage: active volume does not have data.")
    return;
  }

  // Need to cast data the first time to get it into float
  vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
  cast->SetOutputScalarTypeToFloat();
  cast->SetInputData( this->activeVol->GetImageData() );
  cast->Update();
  std::cout << "Finished casting" << std::endl;
  // casting is not in place (makes a copy), so switch what data the node is pointing to
  this->activeVol->SetAndObserveImageData(cast->GetOutput());

  vtkVesselSegHelper::SeedImageType::Pointer itkConvertedImage = vtkVesselSegHelper::ConvertVolumeNodeToItkImage(this->activeVol);
  if (itkConvertedImage.IsNull() == true )
  {
    vtkErrorMacro("PreprocessImage: conversion to ITK not successful.")
    return;
  }

  // Declare the type of objectness measure image filter
  typedef itk::VesselSegmentationPreProcessingFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType > VesselPreProcessingFilterType;

  // Create a vesselness Filter
  VesselPreProcessingFilterType::Pointer VesselPreProcessingFilter = VesselPreProcessingFilterType::New();

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
  std::cout << "PreprocessImage: preprocessing done" << std::endl << "Time taken for PreProcessing : " << clock1.GetMean() << "sec\n" << std::endl;

  preprocessedImg = VesselPreProcessingFilter->GetOutput();

  preprocessedImg->ReleaseDataFlagOff();
  preprocessedImg->DisconnectPipeline();

  // at this point it should be a copy of the data? since it is the output of a non in place filter...
  vtkSmartPointer<vtkImageData> tempVtkImageData = vtkVesselSegHelper::ConvertItkImageToVtkImageData(preprocessedImg);
  //vtkSmartPointer<vtkMRMLScalarVolumeNode> preprocessedNode = vtkVesselSegHelper::ConvertItkImageToVolumeNode(preprocessedImg, true);

  if (tempVtkImageData == NULL)
  {
    vtkErrorMacro("PreprocessImage: conversion to ITK not successful.")
    return;
  }

  // Need to cast data to get it to be copied so it doesn't go out of scope if we make more than one preprocessed image
  vtkSmartPointer<vtkImageCast> cast2 = vtkSmartPointer<vtkImageCast>::New();
  cast2->SetOutputScalarTypeToFloat();
  cast2->SetInputData( tempVtkImageData );
  cast2->Update();
  std::cout << "Finished casting" << std::endl;
  // casting is not in place (makes a copy), so switch what data the node is pointing to

  vtkSmartPointer<vtkMRMLScalarVolumeNode> preprocessedNode = vtkVesselSegHelper::ConvertVtkImageDataToVolumeNode(cast2->GetOutput(), preprocessedImg, true);

  preprocessedNode->SetName("preprocessedImage");
  this->GetMRMLScene()->AddNode(preprocessedNode);

  std::cout << "PreprocessImage: tried to add preprocessed vtk data as node" << std::endl;
}


//---------------------------------------------------------------------------
/**
* Implementing calls to pass things into Rahul's algorithm (including lots of conversions)
*/
void vtkSlicerVesselSegmentationLogic::SegmentVesselsFromWidget(bool isHepatic)
{
  vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node1 = vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

  // get the node from the scene?
  //vtkMRMLVesselSegmentationSeedNode *testNode = vtkMRMLVesselSegmentationSeedNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("VesselSegmentationSeed"));

  this->SplitVessels(node1.GetPointer(), isHepatic);
}

void vtkSlicerVesselSegmentationLogic::SegmentVessels(vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic)
{
  // check have valid image data
  if (!this->activeVol)
  {
    vtkErrorMacro("SegmentVessels: no active volume.")
    return;
  }
  if (!this->activeVol->GetImageData())
  {
    vtkErrorMacro("SegmentVessels: active volume does not have data.")
    return;
  }

  /*
   * check if we already have an image converted to ITK
   * if not then create it
   */
  if( this->preprocessedImg.IsNull() == true ) {

    this->preprocessedImg = vtkVesselSegHelper::ConvertVolumeNodeToItkImage(this->activeVol);

    if( this->preprocessedImg.IsNull() == true  )
    {
      vtkErrorMacro("SegmentVessels: conversion to ITK not successful.")
      return;
    }
  }

  // get the pair of seeds in the right format for the itk filter
  double *seed1 = seedNode->GetSeed1();
  double *seed2 = seedNode->GetSeed2();

  if( seed1[0] == 0.0 && seed1[1] == 0.0 && seed1[2] == 0.0  )
  {
    vtkErrorMacro("CallAssignSeeds: Do not have first seed set.")
    return;
  }
  if( seed2[0] == 0.0 && seed2[1] == 0.0 && seed2[2] == 0.0  )
  {
    vtkErrorMacro("CallAssignSeeds: Do not have second seed set.")
    return;
  }

  const double seed1_0[4] = {seed1[0], seed1[1], seed1[2], 0}; // IJK
  const double seed2_0[4] = {seed2[0], seed2[1], seed2[2], 0}; // IJK

  double *seedIJK1 = new double[4]; // the actual first seed
  double *seedIJK2 = new double[4]; // the direction seed

  // use the ijk matrix to convert the points
  this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed1_0, seedIJK1);
  this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed2_0, seedIJK2);

  vtkVesselSegHelper::Index3D coord1;
  vtkVesselSegHelper::Index3D coord2;

  // the actual first seed
  coord1[0] = seedIJK1[0];
  coord1[1] = seedIJK1[1];
  coord1[2] = seedIJK1[2];
  // the direction seed
  coord2[0] = seedIJK2[0];
  coord2[1] = seedIJK2[1];
  coord2[2] = seedIJK2[2];

  // create filter
  typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType>  SeedFilterType;
  SeedFilterType::Pointer filter = SeedFilterType::New();
  /*
   * Have what is needed to call rahul's algorithm
   * as pair of seeds and an ITK image
   */
  if( preprocessedImg.IsNotNull() )
  {
    std::cout << "Can call itk::SeedVesselSegmentationImageFilter " << preprocessedImg << std::endl;

    if(isHepatic == true) // HEPATIC radio button is selected
    {
      // instantiate filter
      itk::TimeProbe clock1;
      clock1.Start();

      std::cout << "Seed IJK: " << coord1 << std::endl;
      std::cout << "Direction seed: " << coord2 << std::endl;

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
      std::cout << "Done calling itk::SeedVesselSegmentationImageFilter" << std::endl;
      std::cout << "Time taken for itk::SeedVesselSegmentationImageFilter : "<< clock1.GetMean() <<"sec\n"<< std::endl;

      //----------------------------------------------------------------------------
      // convert output of filter back to VTK
      vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegHelper::ConvertItkImageToVtkImageData(this->hepaticITKdata);

      if (outVTKImage.GetPointer()  == NULL )
      {
        vtkErrorMacro("SegmentVessels: conversion to VTK not successful.")
        return;
      }

      if(this->hepaticLabelMap == NULL)
      {
        // first time to create a hepatic label map
        hepaticLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
        hepaticLabelMap->CopyOrientation(this->activeVol);
        hepaticLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
        hepaticLabelMap->SetName("hepaticLabel");
        this->GetMRMLScene()->AddNode(hepaticLabelMap);
        std::cout << "Created hepaticLabelMap " << std::endl;

        // make this label map the selected one
        //vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

        // causes a crash?
        //selectionNode->SetActiveLabelVolumeID(hepaticLabelMap->GetID());
        //selectionNode->SetReferenceActiveLabelVolumeID(hepaticLabelMap->GetID());

        //vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
        //mrmlAppLogic->PropagateVolumeSelection();

        // have updated hepatic, but now merged is not up to date
        this->hepaticUpdated = true;
        this->mergedUpdated = false;
      }
      else
      {
        hepaticLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
        std::cout << "Replaced data for hepaticLabelMap " << std::endl;

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
      typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType>  SeedFilterType;

      std::cout << "Seed IJK: " << coord1 << std::endl;
      std::cout << "Direction seed: " << coord2 << std::endl;

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
      std::cout << "Done calling itk::SeedVesselSegmentationImageFilter" << std::endl;
      std::cout << "Time taken for itk::SeedVesselSegmentationImageFilter : "<< clock1.GetMean() <<"sec\n"<< std::endl;

      //----------------------------------------------------------------------------
      // convert output of filter back to VTK
      vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegHelper::ConvertItkImageToVtkImageData(this->portalITKdata);

      if (outVTKImage.GetPointer()  == NULL )
      {
        vtkErrorMacro("SegmentVessels: conversion to VTK not successful.")
        return;
      }

      if(this->portalLabelMap == NULL)
      {
        // first time to create a portal label map
        portalLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
        portalLabelMap->CopyOrientation(this->activeVol);
        portalLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
        portalLabelMap->SetName("portalLabel");
        this->GetMRMLScene()->AddNode(portalLabelMap);
        std::cout << "Created portalLabelMap " << std::endl;

        // make this label map the selected one
        //vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

        //selectionNode->SetActiveLabelVolumeID(portalLabelMap->GetID());
        //selectionNode->SetReferenceActiveLabelVolumeID(portalLabelMap->GetID());

        //vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
        //mrmlAppLogic->PropagateVolumeSelection();

        // have updated portal, but now merged is not up to date
        this->portalUpdated = true;
        this->mergedUpdated = false;
      }
      else
      {
        portalLabelMap->SetAndObserveImageData(outVTKImage.GetPointer());
        std::cout << "Replaced data for portalLabelMap " << std::endl;

        // have updated portal, but now merged is not up to date
        this->portalUpdated = true;
        this->mergedUpdated = false;
      }
    }
    // will updated models if data has been modified: portalUpdated or hepaticUpdated
    //this->UpdateModels();
  }
  else
  {
    vtkErrorMacro("SegmentVessels: Cannot segment, since do not have preprocessed image.")
    return;
  }
}

//---------------------------------------------------------------------------
/**
* Section to do with hepatic and portal splitting
*/
void vtkSlicerVesselSegmentationLogic::CallMergeLabelMaps()
{
  /*
   * Merge the 2 label maps (hepatic and portal)
   * check the data for both exists
   * loop through them, and in places they both segmented - make it purple
   *
   */
  if((this->hepaticITKdata.IsNotNull()) && (this->portalITKdata.IsNotNull()))
  {
    std::cout << "Can Merge the two label maps" << std::endl;

    typedef itk::AddImageFilter<vtkVesselSegHelper::SeedImageType,vtkVesselSegHelper::SeedImageType> AddFilterType;
    AddFilterType::Pointer addFilter = AddFilterType::New();
    addFilter->SetInput1(this->hepaticITKdata);
    addFilter->SetInput2(this->portalITKdata);
    addFilter->Update();
    this->mergedITKdata = addFilter->GetOutput();

    // convert output of add filter back to VTK
    vtkSmartPointer<vtkImageData> mergedVTKImage = vtkVesselSegHelper::ConvertItkImageToVtkImageData(this->mergedITKdata);

    if (mergedVTKImage.GetPointer()  == NULL )
    {
      vtkErrorMacro("CallMergeLabelMaps: Conversion to VTK not successful.")
      return;
    }

    if(this->mergedLabelMap == NULL)
    {
      // first time to create a hepatic label map
      mergedLabelMap = vtkSmartPointer<vtkMRMLLabelMapVolumeNode>::New();
      mergedLabelMap->CopyOrientation(this->activeVol);
      mergedLabelMap->SetAndObserveImageData(mergedVTKImage.GetPointer());
      mergedLabelMap->SetName("mergedLabel");
      this->GetMRMLScene()->AddNode(mergedLabelMap);
      std::cout << "Created mergedLabelMap " << std::endl;

      // merged label map has been updated
      this->mergedUpdated = true;
    }
    else
    {
      mergedLabelMap->SetAndObserveImageData(mergedVTKImage.GetPointer());
      std::cout << "Replaced data for mergedLabelMap " << std::endl;

      // merged label map has been updated
      this->mergedUpdated = true;
    }

    //vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    //vtkMRMLDisplayNode *displayNode = vtkMRMLDisplayNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLDisplayNodeSingleton"));

    //std::cout << "Trying to select merged label map" << std::endl;
    //selectionNode->SetActiveLabelVolumeID(mergedLabelMap->GetID());
    //selectionNode->SetReferenceActiveLabelVolumeID(mergedLabelMap->GetID());

    //vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
    //mrmlAppLogic->PropagateVolumeSelection();
  }
  else {
    vtkErrorMacro("CallMergeLabelMaps: Do not have 2 label maps.")
    return;
  }
}

void vtkSlicerVesselSegmentationLogic::MergeLabelMaps()
{

}

void vtkSlicerVesselSegmentationLogic::SplitVesselsFromWidget(bool isHepatic)
{
  // create empty node to pass into function for now
  vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode> node1 = vtkSmartPointer<vtkMRMLVesselSegmentationSeedNode>::New();

  this->SplitVessels(node1.GetPointer(), isHepatic);
}

void vtkSlicerVesselSegmentationLogic::SplitVessels(vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic)
{
  double *seed1 = seedNode->GetSeed1();

  std::cout << "Seed node: " << seed1[0] << " " << seed1[1] << " " << seed1[2] << std::endl;

  if( seed1[0] == 0.0 && seed1[1] == 0.0 && seed1[2] == 0.0 )
  {
    vtkErrorMacro("CallAssignSeeds: Do not have a seed set.")
    return;
  }

  const double seed1_0[4] = {seed1[0], seed1[1], seed1[2], 0}; // IJK
  // have a seed
  double *seedIJK = new double[4];
  // use the ijk matrix to convert the points
  this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(seed1_0, seedIJK); // may not be needed, was commented out in old code???

  itk::Point<double, 3> seedLPS;
  seedLPS[0] = - seedIJK[0];
  seedLPS[1] = - seedIJK[1];
  seedLPS[2] = seedIJK[2];

  std::cout << "Seed in LPS-coordinate: " << seedLPS[0] << " " << seedLPS[1] << " " << seedLPS[2] << std::endl;

  //Cast image for LabelMap
  typedef itk::Image<LabelType, 3>  LabelImageType;
  typedef itk::CastImageFilter< vtkVesselSegHelper::SeedImageType, LabelImageType > CastFilterType;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput(mergedITKdata);
  castFilter->Update();

  // create image with just overlapping areas (overlapping areas added lower down)
  LabelImageType::Pointer OnlyOverlap = LabelImageType::New();
  OnlyOverlap->CopyInformation(castFilter->GetOutput());
  OnlyOverlap->SetRegions(castFilter->GetOutput()->GetRequestedRegion());
  OnlyOverlap->Allocate();
  OnlyOverlap->FillBuffer(0);

  itk::ImageRegionIteratorWithIndex<LabelImageType> itOrg(castFilter->GetOutput(), castFilter->GetOutput()->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<LabelImageType> itOnlyOverlap(OnlyOverlap, OnlyOverlap->GetRequestedRegion());

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
    typedef itk::BinaryDilateImageFilter <LabelImageType, LabelImageType, StructuringElementType>  BinaryDilateImageFilterType;
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
    std::cout<<"Dilation and Connected Components - done" << std::endl << "Time taken : "<< clock1.GetMean() <<"sec\n"<< std::endl;

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

    std::cout << "Found a label of: " << labelObject->GetLabel() << " underlying label: " << labelValue <<
            " number of pixels: " << labelObject->GetNumberOfPixels();

    objectCentroid = labelObject->GetCentroid();
    std::cout << "  coordinates of Centroid :" << objectCentroid  << std::endl;
    distance = seedLPS.EuclideanDistanceTo(objectCentroid);

    if((minDistance > distance) && (labelObject->GetNumberOfPixels() > 100))
    {
      minDistance = distance;
      labelObjectNumber = n;
      std::cout << "Distance and label object number set: " << distance << " " << n << std::endl;
    }
  }

  // assign object
  itk::ImageRegionIteratorWithIndex<vtkVesselSegHelper::SeedImageType> itMerged(mergedITKdata, mergedITKdata->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<vtkVesselSegHelper::SeedImageType> itHepatic(hepaticITKdata, hepaticITKdata->GetRequestedRegion());
  itk::ImageRegionIteratorWithIndex<vtkVesselSegHelper::SeedImageType> itPortal(portalITKdata, portalITKdata->GetRequestedRegion());

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

  //vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

  //std::cout << "Trying to select merged label map" << std::endl;
  //selectionNode->SetActiveLabelVolumeID(this->mergedLabelMap->GetID());
  //selectionNode->SetReferenceActiveLabelVolumeID(this->mergedLabelMap->GetID());

  //vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
  //mrmlAppLogic->PropagateVolumeSelection();

  this->hepaticUpdated = true;
  this->portalUpdated = true;
  this->UpdateModels();

  //this->CallMergeLabelMaps();
  this->mergedUpdated = false;
}

void vtkSlicerVesselSegmentationLogic::UpdateModels()
{
  std::cout << "LOGIC - update models, hepatic: " << this->hepaticUpdated << " portal: " << this->portalUpdated << std::endl;

  if(this->hepaticLabelMap != NULL && this->hepaticUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsHepatic = vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesHepatic = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsHepatic->SetInputData(this->hepaticLabelMap->GetImageData());
    structuredPointsHepatic->Update();
    mCubesHepatic->SetInputConnection(structuredPointsHepatic->GetOutputPort());
    mCubesHepatic->SetValue(0,4);
    mCubesHepatic->Update();

    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

    smoother->SetInputConnection(mCubesHepatic->GetOutputPort());
    smoother->SetNumberOfIterations(10);
    smoother->BoundarySmoothingOff();
    smoother->FeatureEdgeSmoothingOff();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

    translation->SetMatrix(this->IJKtoRASmatrix.GetPointer());

    transformFilter->SetInputConnection(smoother->GetOutputPort());
    transformFilter->SetTransform(translation);
    transformFilter->Update();

    if( this->hepaticModelNode == NULL )
    {
      std::cout << "Creating hepatic model" << std::endl;
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
      std::cout << "Tried to add to hepaticModel" << std::endl;
    }
    this->hepaticUpdated = false;

  }
  if(this->portalLabelMap != NULL && this->portalUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsPortal = vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesPortal = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsPortal->SetInputData(this->portalLabelMap->GetImageData());
    structuredPointsPortal->Update();
    mCubesPortal->SetInputConnection(structuredPointsPortal->GetOutputPort());
    mCubesPortal->SetValue(0,5);
    mCubesPortal->Update();

    vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
    vtkSmartPointer<vtkTransform> translation = vtkSmartPointer<vtkTransform>::New();
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();

    smoother->SetInputConnection(mCubesPortal->GetOutputPort());
    smoother->SetNumberOfIterations(10);
    smoother->BoundarySmoothingOff();
    smoother->FeatureEdgeSmoothingOff();
    smoother->SetFeatureAngle(120.0);
    smoother->SetPassBand(.001);
    smoother->NonManifoldSmoothingOn();
    smoother->NormalizeCoordinatesOn();
    smoother->Update();

    translation->SetMatrix(this->IJKtoRASmatrix.GetPointer());

    transformFilter->SetInputConnection(smoother->GetOutputPort());
    transformFilter->SetTransform(translation);
    transformFilter->Update();

    if( this->portalModelNode == NULL )
    {
      std::cout << "Creating portal model" << std::endl;
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
      std::cout << "Tried to add to portalModel" << std::endl;
    }
    this->portalUpdated = false;
  }

  this->Reset3DView();
}

void vtkSlicerVesselSegmentationLogic::Reset3DView()
{
  std::cout << "LOGIC - reset 3D view" << std::endl;

  //Hide Labels and cube
  //vtkMRMLScene *mrmlScene = qSlicerApplication::application()->mrmlScene();
  vtkMRMLScene *mrmlScene = this->GetMRMLScene();
  vtkSmartPointer<vtkCollection> views;
  views.TakeReference(mrmlScene->GetNodesByClassByName("vtkMRMLViewNode",
                                                       "View1"));
  vtkMRMLViewNode *view = vtkMRMLViewNode::SafeDownCast(views->GetItemAsObject(0));
  if(view)
    {
    view->SetAxisLabelsVisible(false);
    view->SetBoxVisible(false);
    }

  //Reset the camera (this causes a crash from testing)
  qSlicerLayoutManager *layoutManager = qSlicerApplication::application()->layoutManager();

  qMRMLThreeDWidget *threeDWidget = layoutManager->threeDWidget(0);
  qMRMLThreeDView *threeDView = threeDWidget->threeDView();
  vtkRenderWindow *renderWindow = threeDView->renderWindow();

  vtkRendererCollection *renderers = renderWindow->GetRenderers();

  for(int i=0; i<renderers->GetNumberOfItems(); i++)
    {
    vtkRenderer *renderer =
      vtkRenderer::SafeDownCast(renderers->GetItemAsObject(i));
    renderer->ResetCamera();
    }
}

vtkVesselSegHelper::SeedImageType::Pointer vtkSlicerVesselSegmentationLogic::GetPortalITKData()
{
  return this->portalITKdata;
}

vtkVesselSegHelper::SeedImageType::Pointer vtkSlicerVesselSegmentationLogic::GetHepaticITKData()
{
  return this->hepaticITKdata;
}

