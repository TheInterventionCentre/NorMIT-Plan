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

/**
 * \ingroup VesselSegmentation
 *
 * \brief This class contains methods to support the high-level actions that the
 * user performs and which are related to the module. The class also serves as
 * coordinator between the MRML reality and the GUI.
 */
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
#include "vtkVesselSegmentationHelper.h" // includes itk seed filter

// MRML includes
#include "vtkSlicerVesselSegmentationModuleLogicExport.h"


class vtkMRMLNode;
class vtkMatrix4x4;
class vtkMRMLScalarVolumeNode;
class vtkMRMLLabelMapVolumeNode;
class vtkMRMLModelNode;
class vtkMRMLModelDisplayNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
/**
 * This class contains the functions that handle adding listeners when nodes are added to the MRML scene.
 * It also handles the fiducials, and calling rahul's algorithm.
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
   * Calls the image preprocessing (needs an input image)
   */
  void CallPreprocessing();

  /**
   * Calls the segmentation algorithm (needs seeds)
   */
  void CallSegmentationAlgorithm();

  /**
   * Calls the a merge of the hepatic and portal label maps
   */
  void CallMergeLabelMaps();

  /**
   * Calls to assign the seed to either portal or hepatic (in an overlapping area)
   */
  void CallAssignSeeds();

  /**
   * Set the active volume node
   *
   * @param the vtkMRMLVolumeNode
   */
  void SetActiveVolumeNode(vtkMRMLVolumeNode *ActiveVolumeNode);

  /**
   * Set the active volume
   *
   * @param the vtkMRMLScalarVolumeNode
   */
  void SetActiveVolume(vtkMRMLScalarVolumeNode *activeVolume);

  /**
   * Get the active volume
   *
   * @return the vtkSmartPointer to a vtkMRMLScalarVolumeNode
   */
  vtkSmartPointer<vtkMRMLScalarVolumeNode> GetActiveVolume();
    
  // fiducial list methods
  /**
   * Get the coordinates of the last fiducial added
   *
   * @return double* to fiducial
   */
  static double* GetLastFiducialCoordinate();

  /**
   * Get the list of fiducials
   *
   * @return std::vector<double*> list of fiducials
   */
  static std::vector<double*> GetFiducialList();

  /**
   * Delete the list of fiducials
   */
  void DeleteFiducials();

  /**
   * If a markup was added
   *
   * @return boolean if a markup was added
   */
  static bool GetMarkupJustAdded();

  /**
   * Set whether working on hepatic or portal for segmentation
   *
   * @param bool where true if hepatic
   */
  void IsHepaticSeg(bool isHepatic);

  /**
   * Set whether working on hepatic or portal for merge
   *
   * @param bool where true if hepatic
   */
  void IsHepaticMerge(bool isHepatic);

  /**
   * Set the lower threshold for preprocessing
   *
   * @param value for the threshold
   */
  void SetLowerThreshold(int value);

  /**
   * Set the upper threshold for preprocessing
   *
   * @param value for the threshold
   */
  void SetUpperThreshold(int value);

  /**
   * Set the alpha value for preprocessing
   *
   * @param value for alpha
   */
  void SetAlpha(int value);

  /**
   * Set the beta value for preprocessing
   *
   * @param value for beta
   */
  void SetBeta(int value);

  /**
   * Set the conductance value for preprocessing
   *
   * @param value for conductance
   */
  void SetConductance(int value);

  /**
   * Set the number of iterations for preprocessing
   *
   * @param value for iterations
   */
  void SetIterations(int value);

  /**
   * Helper function to update the 3D models
   */
  void UpdateModels();

  /**
   * Helper function to reset the 3D view
   */
  void Reset3DView();

protected:
  vtkSlicerVesselSegmentationLogic();
  virtual ~vtkSlicerVesselSegmentationLogic();

  vtkSmartPointer<vtkMRMLVolumeNode> ActiveVolumeNode;

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

  //virtual void OnMRMLSceneChanged(vtkMRMLNode* node);

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
    
  static void OnMRMLMarkupAdded(vtkObject *caller, unsigned long int id, void *clientData, void *callerData);
    
private:    
  bool nodeObserversSet;
  static bool markupJustAdded;
  bool hepaticSeg;
  bool hepaticMerge;

  int lowerThreshold;
  int upperThreshold;
  int alpha;
  int beta;
  int conductance;
  int interations;

  vtkNew<vtkMatrix4x4> IJKtoRASmatrix;
  vtkNew<vtkMatrix4x4> RAStoIJKmatrix;

  static std::vector<double*> fiducialVector;
  vtkSmartPointer<vtkMRMLScalarVolumeNode> activeVol;
  vtkVesselSegmentationHelper::SeedImageType::Pointer preprocessedImg; //= vtkVesselSegHelper::SeedImageType::New();

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
