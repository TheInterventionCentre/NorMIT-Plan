/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

// VesselSegmentation Logic includes
#include "vtkSlicerVesselSegmentationLogic.h"
//#include "LiverImagePreProcessing.h"
#include <itkVesselSegmentationPreProcessingFilter.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLScalarVolumeNode.h>
#include <vtkMRMLMarkupsFiducialNode.h>
#include <vtkMRMLMarkupsNode.h>
#include <vtkMRMLAnnotationLinesNode.h>
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
#include <itkGPUImage.h>
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

std::vector<double*> vtkSlicerVesselSegmentationLogic::fiducialVector;
bool vtkSlicerVesselSegmentationLogic::markupJustAdded;

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerVesselSegmentationLogic);

//----------------------------------------------------------------------------
/**
 * Constructor
 */
vtkSlicerVesselSegmentationLogic::vtkSlicerVesselSegmentationLogic()
{
  nodeObserversSet = false;
  hepaticSeg = true;
  hepaticMerge = true;

  hepaticUpdated = false;
  portalUpdated = false;
  mergedUpdated = false;

  lowerThreshold = 100;
  upperThreshold = 250;
  alpha = 20;
  beta = 160;
  conductance = 20;
  interations = 30;
}

//----------------------------------------------------------------------------
vtkSlicerVesselSegmentationLogic::~vtkSlicerVesselSegmentationLogic()
{
  // delete all the vector data (pointers to arrays)
  int s = fiducialVector.size();
  for(int i = 0; i < s; i++) {
    delete[] fiducialVector[i];
  }
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
  this->nodeObserversSet = false;
  this->markupJustAdded = false;
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
::OnMRMLSceneNodeAdded(vtkMRMLNode* addedNode)
{

  vtkMRMLMarkupsNode* tempMarkupNode = vtkMRMLMarkupsNode::SafeDownCast(addedNode);
  vtkMRMLMarkupsFiducialNode* tempFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(addedNode);
  
  if((tempMarkupNode != NULL) && (tempFiducialNode != NULL))
  {
    std::cout << "added node called & both not null." << std::endl;
    
    if(this->nodeObserversSet == false)
    {
      std::cout << "Now observing: " << tempMarkupNode << " " << tempFiducialNode << std::endl;
      this->nodeObserversSet = true;
      
      /* Observe markup node */
      vtkSmartPointer<vtkCallbackCommand> markupAddedCallback = vtkSmartPointer<vtkCallbackCommand>::New();
      markupAddedCallback->SetCallback(this->OnMRMLMarkupAdded);
      markupAddedCallback->SetClientData(this);
      
      tempMarkupNode->AddObserver(vtkMRMLMarkupsNode::MarkupAddedEvent, markupAddedCallback);
    
      tempMarkupNode->InvokeEvent(vtkMRMLMarkupsNode::MarkupAddedEvent);
      /* ----------------------- */
      
      /* Observe fiducial node */
      vtkNew<vtkIntArray> events;
      events->InsertNextValue(vtkCommand::ModifiedEvent);
      vtkUnObserveMRMLNodeMacro(tempFiducialNode);
      vtkObserveMRMLNodeEventsMacro(tempFiducialNode, events.GetPointer());
    
      tempFiducialNode->InvokeEvent(vtkCommand::ModifiedEvent);
      /* ----------------------- */
    }
  }
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
void vtkSlicerVesselSegmentationLogic::OnMRMLNodeModified(vtkMRMLNode* modifiedNode)
{
    if(this->markupJustAdded)
    {
  
    std::cout << "node modified called LOGIC " << std::endl;
    vtkMRMLMarkupsFiducialNode* tempFiducialNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(modifiedNode);
  
    if(tempFiducialNode != NULL)
    {

      int n = tempFiducialNode->GetNumberOfFiducials();
      std::cout << "Fiducial vector: " << n << std::endl;

      if( !this->activeVol )
      {
    	return;
      }
      vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
      this->activeVol->GetIJKToRASMatrix(mat);
      this->IJKtoRASmatrix->DeepCopy(mat);
      this->RAStoIJKmatrix->DeepCopy(mat);
      this->RAStoIJKmatrix->Invert();

      if( n > 0 ) {
        double *coords = new double[4];
        tempFiducialNode->GetNthFiducialWorldCoordinates((n-1),coords);
        std::cout << "world coords: " << coords[0] << " " << coords[1] << " ";
        std::cout << coords[2] << " " << coords[3] << std::endl;

        // add fiducial to our list of fiducials
        this->fiducialVector.push_back(coords);
        this->markupJustAdded = false;
      }
    }
  }
}

/**
 * Markup added to scene
 * change boolean markupJustAdded for other part of code
 * !!! This appears to fire before the coordinates are set. !!!
 */
void vtkSlicerVesselSegmentationLogic::
OnMRMLMarkupAdded(vtkObject *caller,
		long unsigned int vtkNotUsed(eventId),
		void *clientData, void *callData)
{
  //std::cout << "on markup added called" << std::endl;
  
  vtkSlicerVesselSegmentationLogic* logic = reinterpret_cast<vtkSlicerVesselSegmentationLogic*>(clientData);

  logic->markupJustAdded = true;
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

}
/**
 * Set Active volume when use dropdown
 */
void vtkSlicerVesselSegmentationLogic::SetActiveVolume(vtkMRMLScalarVolumeNode *activeVolume)
{
  std::cout << "LOGIC - set active Volume: " << activeVolume << std::endl;
  this->activeVol = activeVolume;
}
vtkSmartPointer<vtkMRMLScalarVolumeNode> vtkSlicerVesselSegmentationLogic::GetActiveVolume()
{
  return this->activeVol;
}
/**
 * Get the last fiducials
 */
double* vtkSlicerVesselSegmentationLogic::GetLastFiducialCoordinate()
{
  int s = fiducialVector.size();

  if(s > 0)
  {
    return fiducialVector[s-1];
  }
  else
  {
    return NULL;
  }
}
/**
 * Returns the fiducial list
 */
std::vector<double*> vtkSlicerVesselSegmentationLogic::GetFiducialList()
{
  return fiducialVector;
}
/**
 * Returns the boolean markupJustAdded
 */
bool vtkSlicerVesselSegmentationLogic::GetMarkupJustAdded()
{
  return markupJustAdded;
}

void vtkSlicerVesselSegmentationLogic::DeleteFiducials()
{
  // delete the fiducials
  vtkCollection *listNodes = this->GetMRMLScene()->GetNodesByClass("vtkMRMLMarkupsNode");

  // loop through collection to get the fiducial node
  int l = listNodes->GetNumberOfItems();
  for(int i = 0; i < l; i++)
  {
    vtkMRMLMarkupsFiducialNode* tempMarkupNode = vtkMRMLMarkupsFiducialNode::SafeDownCast(listNodes->GetItemAsObject(0));
    if( tempMarkupNode != NULL )
    {
        std::cout << "Deleting Markups " << tempMarkupNode << std::endl;

        //int n = tempMarkupNode->GetNumberOfMarkups();
        tempMarkupNode->RemoveAllMarkups();
    }
  }
  listNodes->Delete();

  // delete all the vector data (pointers to arrays)
  int s = fiducialVector.size();
  for(int i = 0; i < s; i++) {
    delete[] fiducialVector[i];
  }
}


/**
 * set all the variables from spin boxes
 * for the preprocessing
 */
void vtkSlicerVesselSegmentationLogic::SetLowerThreshold(int value)
{
  this->lowerThreshold = value;
}
void vtkSlicerVesselSegmentationLogic::SetUpperThreshold(int value)
{
  this->upperThreshold = value;
}
void vtkSlicerVesselSegmentationLogic::SetAlpha(int value)
{
  this->alpha = value;
}
void vtkSlicerVesselSegmentationLogic::SetBeta(int value)
{
  this->beta = value;
}
void vtkSlicerVesselSegmentationLogic::SetConductance(int value)
{
  this->conductance = value;
}
void vtkSlicerVesselSegmentationLogic::SetIterations(int value)
{
  this->interations = value;
}

//---------------------------------------------------------------------------
/**
* Call preprocessing (pipeline of a bunch of ITK filters)
*/
void vtkSlicerVesselSegmentationLogic::CallPreprocessing()
{
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
    std::cerr
      << "Conversion to ITK not successful"
      << std::endl;
  }
  std::cout << "Converted to ITK" << std::endl;

  // Delcare the type of objectness measure image filter
  typedef itk::VesselSegmentationPreProcessingFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType > VesselPreProcessingFilterType;

  // Create a vesselness Filter
  VesselPreProcessingFilterType::Pointer VesselPreProcessingFilter = VesselPreProcessingFilterType::New();

  //Connect to input image
  VesselPreProcessingFilter->SetInput( itkConvertedImage );

  VesselPreProcessingFilter->SetLowerThreshold(this->lowerThreshold);
  VesselPreProcessingFilter->SetUpperThreshold(this->upperThreshold);
  VesselPreProcessingFilter->SetAlpha(this->alpha);
  VesselPreProcessingFilter->SetBeta(this->beta);
  VesselPreProcessingFilter->SetConductance(this->conductance);
  VesselPreProcessingFilter->SetNumberOfIterations(this->interations);

  // pass everything into function
  itk::TimeProbe clock1;
  clock1.Start();
  VesselPreProcessingFilter->Update();
  clock1.Stop();
  std::cout<<"PreProcessing - done" << std::endl << "Time taken for PreProcessing : "<< clock1.GetMean() <<"sec\n"<< std::endl;

  preprocessedImg = VesselPreProcessingFilter->GetOutput();

  preprocessedImg->ReleaseDataFlagOff();
  preprocessedImg->DisconnectPipeline();

  // at this point it should be a copy of the data? since it is the output of a non in place filter...
  vtkSmartPointer<vtkImageData> tempVtkImageData = vtkVesselSegHelper::ConvertItkImageToVtkImageData(preprocessedImg);
  //vtkSmartPointer<vtkMRMLScalarVolumeNode> preprocessedNode = vtkVesselSegHelper::ConvertItkImageToVolumeNode(preprocessedImg, true);

  if (tempVtkImageData == NULL)
  {
    std::cerr
      << "Conversion to VTK not successful"
      << std::endl;
  }
  std::cout << "Converted image to VTK " << std::endl;

  // Need to cast data to get it to be copied so it doesn't go out of scope if we make more than one preprocessed image
  vtkSmartPointer<vtkImageCast> cast2 = vtkSmartPointer<vtkImageCast>::New();
  cast2->SetOutputScalarTypeToFloat();
  cast2->SetInputData( tempVtkImageData );
  cast2->Update();
  std::cout << "Finished casting" << std::endl;
  // casting is not in place (makes a copy), so switch what data the node is pointing to

  vtkSmartPointer<vtkMRMLScalarVolumeNode> preprocessedNode = vtkVesselSegHelper::ConvertVtkImageDataToVolumeNode(cast2->GetOutput(), preprocessedImg, true);

  //preprocessedNode->SetName("preprocessedImage");
  this->GetMRMLScene()->AddNode(preprocessedNode);

  std::cout << "Tried to add preprocessed vtk data as node" << std::endl;
}


//---------------------------------------------------------------------------
/**
* Implementing calls to pass things into Rahul's algorithm (including lots of conversions)
*/
void vtkSlicerVesselSegmentationLogic::CallSegmentationAlgorithm()
{

  /*
   * check if we already have an image converted to ITK
   * if not then create it
   */
  if( preprocessedImg.IsNull() == true ) {

    preprocessedImg = vtkVesselSegHelper::ConvertVolumeNodeToItkImage(this->activeVol);

    if( preprocessedImg.IsNull() == true  )
    {
      std::cerr
        << "Conversion to ITK not successful"
        << std::endl;
    }
    std::cout << "Converted image to ITK " << std::endl;
  }


  // create a list of pairs of fiducials
  typedef std::pair<vtkVesselSegHelper::Index3D, vtkVesselSegHelper::Index3D> fiducialPair;
  std::vector<fiducialPair> fidAndFid;

  // have at least 2 fiducials, so try to calculate direction
  while(this->fiducialVector.size() > 1)
  {
    // get the last 2 fiducials, and take them off the vector
    double *vector1 = this->fiducialVector.back(); // the direction seed
    this->fiducialVector.pop_back();
    double *vector2 = this->fiducialVector.back(); // the actual first seed
    this->fiducialVector.pop_back();

    double *fidIJK1 = new double[4]; // the direction seed
    double *fidIJK2 = new double[4]; // the actual first seed
    // use the ijk matrix to convert the points
    this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(vector1, fidIJK1);
    this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(vector2, fidIJK2);

    vtkVesselSegHelper::Index3D coord1;
    vtkVesselSegHelper::Index3D coord2;

    // the direction seed
    coord1[0] = fidIJK1[0];
    coord1[1] = fidIJK1[1];
    coord1[2] = fidIJK1[2];
    // the actual first seed
    coord2[0] = fidIJK2[0];
    coord2[1] = fidIJK2[1];
    coord2[2] = fidIJK2[2];

    fiducialPair tempPair(coord2, coord1);
    fidAndFid.push_back(tempPair);

    std::cout << "Seed list: " << tempPair.first << tempPair.second << std::endl;
  }

  /*
   * Have what is needed to call rahul's algorithm
   * fiducial list and an ITK image
   */
  if( (fidAndFid.size() > 0) && (preprocessedImg.IsNotNull()) )
  {
    std::cout << "Can call itk::SeedVesselSegmentationImageFilter " << preprocessedImg << std::endl;

    // HEPATIC radio button is selected
    if(this->hepaticSeg == true)
    {
      // instantiate filter
      itk::TimeProbe clock1;
      clock1.Start();
      typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType>  SeedFilterType;

      int n = fidAndFid.size();
      std::cout << "Seed list length: " << n << std::endl;

      // loop through all the fiducials
      for( int i=0; i < n; i++)
      {
        fiducialPair tempPair;
        tempPair = fidAndFid.back(); // get last thing on list
        fidAndFid.pop_back(); // take it out of list
        std::cout << "Seed IJK: " << tempPair.first << std::endl;
        std::cout << "Direction seed: " << tempPair.second << std::endl;

        // Create a seed filter
        SeedFilterType::Pointer filter = SeedFilterType::New();
        filter->SetInput(preprocessedImg);
        filter->SetSeed(tempPair.first);
        filter->SetDirectionSeed(tempPair.second);
        filter->SetOutputLabel(4);
        if( this->hepaticITKdata.IsNotNull() )
        {
          filter->SetPreviousOutput(this->hepaticITKdata);
        }
        filter->Update();

        this->hepaticITKdata  = filter->GetOutput();
      }
      clock1.Stop();
      std::cout<< "Done calling itk::SeedVesselSegmentationImageFilter" << std::endl << "Time taken for itk::SeedVesselSegmentationImageFilter : "<< clock1.GetMean() <<"sec\n"<< std::endl;

    }
    else // PORTAL radio button is selected
    {
      // instantiate filter
      itk::TimeProbe clock1;
      clock1.Start();
      typedef itk::SeedVesselSegmentationImageFilter<vtkVesselSegHelper::SeedImageType, vtkVesselSegHelper::SeedImageType>  SeedFilterType;

      int n = fidAndFid.size();
      std::cout << "Seed list length: " << n << std::endl;

      // loop through all the fiducials
      for( int i=0; i < n; i++)
      {
        fiducialPair tempPair;
        tempPair = fidAndFid.back(); // get last thing on list
        fidAndFid.pop_back(); // take it out of list
        std::cout << "Seed IJK: " << tempPair.first << std::endl;
        std::cout << "Direction seed: " << tempPair.second << std::endl;

        // Create a seed filter
        SeedFilterType::Pointer filter = SeedFilterType::New();
        filter->SetInput(preprocessedImg);
        filter->SetSeed(tempPair.first);
        filter->SetDirectionSeed(tempPair.second);
        filter->SetOutputLabel(5);
        if( this->portalITKdata.IsNotNull() )
        {
          filter->SetPreviousOutput(this->portalITKdata);
        }
        filter->Update();

        this->portalITKdata  = filter->GetOutput();
      }
      clock1.Stop();
      std::cout<< "Done calling itk::SeedVesselSegmentationImageFilter" << std::endl << "Time taken for itk::SeedVesselSegmentationImageFilter : "<< clock1.GetMean() <<"sec\n"<< std::endl;
    }

    if(this->hepaticSeg == true) // HEPATIC radio button is selected
    {
      // convert output of filter back to VTK
      vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegHelper::ConvertItkImageToVtkImageData(this->hepaticITKdata);

      if (outVTKImage.GetPointer()  == NULL )
      {
        std::cerr
          << "Conversion to VTK not successful"
          << std::endl;
      }
      std::cout << "Converted image to VTK " << std::endl;

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
        vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

        selectionNode->SetActiveLabelVolumeID(hepaticLabelMap->GetID());
        selectionNode->SetReferenceActiveLabelVolumeID(hepaticLabelMap->GetID());

        vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
        mrmlAppLogic->PropagateVolumeSelection();

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
      // convert output of filter back to VTK
      vtkSmartPointer<vtkImageData> outVTKImage = vtkVesselSegHelper::ConvertItkImageToVtkImageData(this->portalITKdata);

      if (outVTKImage.GetPointer()  == NULL )
      {
        std::cerr
          << "Conversion to VTK not successful"
          << std::endl;
      }
      std::cout << "Converted image to VTK " << std::endl;

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
        vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

        selectionNode->SetActiveLabelVolumeID(portalLabelMap->GetID());
        selectionNode->SetReferenceActiveLabelVolumeID(portalLabelMap->GetID());

        vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
        mrmlAppLogic->PropagateVolumeSelection();

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
    this->UpdateModels();
  }
  else
  {
    std::cout << "Cannot call Centreline " << this->activeVol << std::endl;
  }
  this->DeleteFiducials();
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
      std::cerr
        << "Conversion to VTK not successful"
        << std::endl;
    }
    std::cout << "Converted image to VTK " << std::endl;

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

    vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));
    //vtkMRMLDisplayNode *displayNode = vtkMRMLDisplayNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLDisplayNodeSingleton"));

    std::cout << "Trying to select merged label map" << std::endl;
    selectionNode->SetActiveLabelVolumeID(mergedLabelMap->GetID());
    selectionNode->SetReferenceActiveLabelVolumeID(mergedLabelMap->GetID());

    vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
    mrmlAppLogic->PropagateVolumeSelection();
  }
  else {
    std::cout << "Do not have 2 label maps" << std::endl;
  }
}

void vtkSlicerVesselSegmentationLogic::CallAssignSeeds()
{
  std::cout << "LOGIC - Assign seeds (seg) " << std::endl;

  // have at least 1 fiducial
  while(this->fiducialVector.size() > 0)
  {
    // get the seed position
    double *vector = this->fiducialVector.back();
    fiducialVector.pop_back();

    double *fidIJK = new double[4];
    // use the ijk matrix to convert the points
    //this->RAStoIJKmatrix.GetPointer()->MultiplyPoint(vector, fidIJK);
    fidIJK = vector;

    itk::Point<double, 3> fidLPS;
    fidLPS[0] = - fidIJK[0];
    fidLPS[1] = - fidIJK[1];
    fidLPS[2] = fidIJK[2];

    std::cout << "Seed in LPS-coordinate: " << fidLPS[0] << " " << fidLPS[1] << " " << fidLPS[2] << std::endl;

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
        distance = fidLPS.EuclideanDistanceTo(objectCentroid);

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
    if(this->hepaticMerge) // hepatic
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
  }

  vtkMRMLSelectionNode *selectionNode = vtkMRMLSelectionNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID("vtkMRMLSelectionNodeSingleton"));

  std::cout << "Trying to select merged label map" << std::endl;
  selectionNode->SetActiveLabelVolumeID(this->mergedLabelMap->GetID());
  selectionNode->SetReferenceActiveLabelVolumeID(this->mergedLabelMap->GetID());

  vtkMRMLApplicationLogic *mrmlAppLogic = this->GetMRMLApplicationLogic();
  mrmlAppLogic->PropagateVolumeSelection();

  this->DeleteFiducials();

  this->hepaticUpdated = true;
  this->portalUpdated = true;
  this->UpdateModels();

  //this->CallMergeLabelMaps();
  this->mergedUpdated = false;
}

/**
 * Set the boolean isHepatic
 * true = working on hepatic
 * false = working on portal
 */
void vtkSlicerVesselSegmentationLogic::IsHepaticSeg(bool isHepatic)
{
  this->hepaticSeg = isHepatic;
  std::cout << "LOGIC - Hepatic? (seg): " << this->hepaticSeg << std::endl;

}

void vtkSlicerVesselSegmentationLogic::IsHepaticMerge(bool isHepatic)
{
  this->hepaticMerge = isHepatic;
  std::cout << "LOGIC - Hepatic? (merge): " << this->hepaticMerge << std::endl;

}

void vtkSlicerVesselSegmentationLogic::UpdateModels()
{
  std::cout << "LOGIC - update models, hepatic: " << this->hepaticUpdated << " portal: " << this->portalUpdated << std::endl;

  if(this->hepaticLabelMap != NULL && this->hepaticUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsHepatic = vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesHepatic = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsHepatic->SetInputData(hepaticLabelMap->GetImageData());
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
      // first time to create hepatic model node
      hepaticModelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
      hepaticModelNode->SetName("hepaticVesselModel");
      hepaticModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      this->GetMRMLScene()->AddNode(hepaticModelNode);

      // also need a display node
      hepaticModelDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
      hepaticModelDisplayNode->SetColor(0,0,1); // blue
      this->GetMRMLScene()->AddNode(hepaticModelDisplayNode);
      hepaticModelNode->SetAndObserveDisplayNodeID(hepaticModelDisplayNode->GetID());
    }
    else // add to 3D model, as already have nodes
    {
      hepaticModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      std::cout << "Tried to add to hepaticModel" << std::endl;
    }
    this->hepaticUpdated = false;

  }
  if(this->portalLabelMap != NULL && this->portalUpdated)
  {
    vtkSmartPointer<vtkImageToStructuredPoints> structuredPointsPortal = vtkSmartPointer<vtkImageToStructuredPoints>::New();
    vtkSmartPointer<vtkDiscreteMarchingCubes> mCubesPortal = vtkSmartPointer<vtkDiscreteMarchingCubes>::New();

    structuredPointsPortal->SetInputData(portalLabelMap->GetImageData());
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

    if( portalModelNode == NULL )
    {
      // first time to create portal model node
      portalModelNode = vtkSmartPointer<vtkMRMLModelNode>::New();
      portalModelNode->SetName("portalVesselModel");
      portalModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      this->GetMRMLScene()->AddNode(portalModelNode);

      // also need a display node
      portalModelDisplayNode = vtkSmartPointer<vtkMRMLModelDisplayNode>::New();
      portalModelDisplayNode->SetColor(1,0,0); // red
      this->GetMRMLScene()->AddNode(portalModelDisplayNode);
      portalModelNode->SetAndObserveDisplayNodeID(portalModelDisplayNode->GetID());
    }
    else // add to 3D model, as already have nodes
    {
      portalModelNode->SetAndObservePolyData(transformFilter->GetOutput());
      std::cout << "Tried to add to portalModel" << std::endl;
    }
    this->portalUpdated = false;
  }

  this->Reset3DView();
}

void vtkSlicerVesselSegmentationLogic::Reset3DView()
{

  //Hide Labels and cube
  vtkMRMLScene *mrmlScene = qSlicerApplication::application()->mrmlScene();
  vtkSmartPointer<vtkCollection> views;
  views.TakeReference(mrmlScene->GetNodesByClassByName("vtkMRMLViewNode",
                                                       "View1"));

  vtkMRMLViewNode *view = vtkMRMLViewNode::SafeDownCast(views->GetItemAsObject(0));
  if(view)
    {
    view->SetAxisLabelsVisible(false);
    view->SetBoxVisible(false);
    }

  //Reset the camera
  qSlicerLayoutManager *layoutManager =
    qSlicerApplication::application()->layoutManager();

  qMRMLThreeDWidget *threeDWidget =
    layoutManager->threeDWidget(0);
  qMRMLThreeDView *threeDView =
    threeDWidget->threeDView();
  vtkRenderWindow *renderWindow = threeDView->renderWindow();

  vtkRendererCollection *renderers = renderWindow->GetRenderers();

  for(int i=0; i<renderers->GetNumberOfItems(); i++)
    {
    vtkRenderer *renderer =
      vtkRenderer::SafeDownCast(renderers->GetItemAsObject(i));
    renderer->ResetCamera();
    }
}


