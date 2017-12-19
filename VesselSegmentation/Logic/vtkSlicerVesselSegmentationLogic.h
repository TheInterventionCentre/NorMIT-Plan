/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkSlicerVesselSegmentationLogic.h

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


#ifndef __vtkSlicerVesselSegmentationLogic_h
#define __vtkSlicerVesselSegmentationLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"
#include <vtkMRMLVolumeNode.h>

// STD includes
#include <vector>
#include <vtkNew.h>

#include <itkImage.h>
#include <itkShapeLabelObject.h>
#include <itkLabelMap.h>

// MRML includes
#include "vtkSlicerVesselSegmentationModuleLogicExport.h"

#include "vtkVesselSegmentationHelper.h" // includes itk seed filter

class vtkMRMLNode;
class vtkMRMLScene;
class vtkMRMLVesselSegmentationSeedNode;
class vtkMRMLScalarVolumeNode;
class vtkMRMLLabelMapVolumeNode;
class vtkMRMLModelNode;
class vtkMRMLModelDisplayNode;

/**
 * \ingroup VesselSegmentation
 *
 * \brief This class contains methods to support the high-level actions that the
 * user performs and which are related to the module. The class also serves as
 * coordinator between the MRML reality and the GUI.
 *
 * This class contains the functions that handle adding listeners when nodes are added to the MRML scene.
 * It also handles the seed nodes, and calling the segmentation algorithm.
 * The hepatic and portal label maps and models are also created and handled inside this class.
 */
class VTK_SLICER_VESSELSEGMENTATION_MODULE_LOGIC_EXPORT vtkSlicerVesselSegmentationLogic :
  public vtkSlicerModuleLogic
{
public:

  /**
   * Standard vtk object instantiation method.
   *
   * @return a pointer to the newly created object.
   */
  static vtkSlicerVesselSegmentationLogic *New();
  vtkTypeMacro(vtkSlicerVesselSegmentationLogic, vtkSlicerModuleLogic);

  /**
   * Standard vtk object function to print the properties of the object.
   *
   * @param os output stream where the properties should be printed to.
   * @param indent indentation value.
   */
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Calls preprocessing to enhance the vesselness of the image
   * (prerequisite: an input image).
   *
   * @param lower threshold.
   * @pararm upper threshold.
   * @param alpha.
   * @param beta.
   * @param conductance.
   * @param number of interations.
   */
  void PreprocessImage( int lowerThreshold, int upperThreshold, unsigned int alpha, int beta, unsigned int conductance, unsigned int iterations );

  /**
   * Calls the segmentation algorithm when the button is pressed.
   *
   * @param if the hepatic radio button is selected.
   */
  void SegmentVesselsFromWidget(bool isHepatic);

  /**
   * Runs the segmentation algorithm.
   * (prerequisite: an input image (preprocessed) and a seed node (with 2 points set)
   *
   * @param the seed node.
   * @param if the hepatic radio button is selected.
   */
  void SegmentVessels(vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic);

  /**
   * Combines the hepatic and portal label maps.
   */
  void MergeLabelMaps();

  /**
   * Calls to assign the seed to either portal or hepatic.
   *
   * @param if the hepatic radio button is selected.
   */
  void SplitVesselsFromWidget(bool isHepatic);

  /**
   * Runs the algorithm to assign the area around the seed to either portal or hepatic
   * (if in an overlapping area).
   *
   * @param the seed node.
   * @param if the hepatic radio button is selected.
   */
  void SplitVessels(vtkMRMLVesselSegmentationSeedNode *seedNode, bool isHepatic);

  /**
   * Helper function to update the 3D models
   */
  void UpdateModels();

  /**
   * Helper function to reset the 3D view
   */
  void Reset3DView();

  /**
   * Method to get active volume.
   *
   * @return scalar volume node pointer, throws errors
   * and returns null if do not have active volume.
   */
  vtkMRMLScalarVolumeNode* GetActiveVolume();

  /**
   * Method to set and propagate selection of active volume.
   *
   * @param scalar volume node pointer.
   */
  void SetAndPropagateActiveVolume(vtkMRMLScalarVolumeNode*);

  /**
   * Method to set and propagate selection of active label.
   *
   * @param label volume node pointer.
   */
  void SetAndPropagateActiveLabel(vtkMRMLLabelMapVolumeNode*);

  /**
   * Method to get the merged ITK data.
   *
   * @return pointer to merged ITK data.
   */
  vtkVesselSegmentationHelper::SeedImageType::Pointer GetPreprocessedITKData();

  /**
   * Method to get the hepatic ITK data.
   *
   * @return pointer to hepatic ITK data.
   */
  vtkVesselSegmentationHelper::SeedImageType::Pointer GetHepaticITKData();

  /**
   * Method to get the portal ITK data.
   *
   * @return pointer to portal ITK data.
   */
  vtkVesselSegmentationHelper::SeedImageType::Pointer GetPortalITKData();

  /**
   * Method to get the merged ITK data.
   *
   * @return pointer to merged ITK data.
   */
  vtkVesselSegmentationHelper::SeedImageType::Pointer GetMergedITKData();

  /**
   * Method to set the hepatic label map.
   *
   * @param label volume node pointer.
   */
  void SetHepaticLabelMap(vtkMRMLLabelMapVolumeNode*);

  /**
   * Method to set the portal label map.
   *
   * @param label volume node pointer.
   */
  void SetPortalLabelMap(vtkMRMLLabelMapVolumeNode*);

protected:
  vtkSlicerVesselSegmentationLogic();
  virtual ~vtkSlicerVesselSegmentationLogic();

  /**
   * Setting the MRML scene internally.
   *
   * @param newScene pointer to the new MRML scene.
   */
  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);

  /**
   * Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
   */
  virtual void ObserveMRMLScene();

  /**
   * Register MRML Node classes to Scene. Gets called automatically when the
   * MRMLScene is attached to this logic class.
   */
  virtual void RegisterNodes();

  /**
   * Update the logic based on changes happening in the MRML scene.
   */
  virtual void UpdateFromMRMLScene();

  /**
   * Method to control actions when a new MRML node is added to the MRML scene.
   *
   * @param node pointer to the vtkMRMLNode added to the scene.
   */
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);

  /**
   * Method to control actions when a MRML node in the scene gets removed.
   *
   * @param node pointer to the vtkMRMLNode to be removed.
   */
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

  /**
   * Method to control actions when a MRML node in the scene modified.
   *
   * @param node pointer to the vtkMRMLNode to be modified.
   */
  virtual void OnMRMLNodeModified(vtkMRMLNode* node);
    
private:
  vtkVesselSegmentationHelper::SeedImageType::Pointer preprocessedImg;

  int vtkScalarType;

  bool hepaticUpdated;
  vtkVesselSegmentationHelper::SeedImageType::Pointer hepaticITKdata;
  vtkSmartPointer<vtkMRMLModelNode> hepaticModelNode;
  vtkSmartPointer<vtkMRMLModelDisplayNode> hepaticModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> hepaticLabelMap;

  bool portalUpdated;
  vtkVesselSegmentationHelper::SeedImageType::Pointer portalITKdata;
  vtkSmartPointer<vtkMRMLModelNode> portalModelNode;
  vtkSmartPointer<vtkMRMLModelDisplayNode> portalModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> portalLabelMap;

  bool mergedUpdated;
  vtkVesselSegmentationHelper::SeedImageType::Pointer mergedITKdata;
  vtkSmartPointer<vtkMRMLModelDisplayNode> mergedModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> mergedLabelMap;

  typedef unsigned short LabelType;
  typedef itk::ShapeLabelObject< LabelType, 3 >  ShapeLabelObjectType;
  typedef itk::LabelMap< ShapeLabelObjectType >  LabelMapType;

  LabelMapType::Pointer onlyOverlapLabelMap;

  vtkSlicerVesselSegmentationLogic(const vtkSlicerVesselSegmentationLogic&); // Not implemented
  void operator=(const vtkSlicerVesselSegmentationLogic&); // Not implemented
};

#endif
