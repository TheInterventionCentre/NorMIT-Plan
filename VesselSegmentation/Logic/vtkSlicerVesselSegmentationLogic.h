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

// .NAME vtkSlicerVesselSegmentationLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


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

#include "vtkVesselSegHelper.h" // includes itk seed filter

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
    
  static vtkSlicerVesselSegmentationLogic *New();
  vtkTypeMacro(vtkSlicerVesselSegmentationLogic, vtkSlicerModuleLogic);

  /**
   * Standard vtk object function to print the properties of the object.
   *
   * @param os output stream where the properties should be printed to.
   * @param indent indentation value.
   */
  void PrintSelf(ostream& os, vtkIndent indent);

  void CallPreprocessing();
  void CallSegmentationAlgorithm();
  void CallMergeLabelMaps();
  void CallAssignSeeds();

  void SetActiveVolumeNode(vtkMRMLVolumeNode *ActiveVolumeNode);
  void SetActiveVolume(vtkMRMLScalarVolumeNode *activeVolume);
  vtkSmartPointer<vtkMRMLScalarVolumeNode> GetActiveVolume();
    
  // fiducial list methods
  static double* GetLastFiducialCoordinate();
  static std::vector<double*> GetFiducialList();
  void DeleteFiducials();

  static bool GetMarkupJustAdded();
  void IsHepaticSeg(bool isHepatic);
  void IsHepaticMerge(bool isHepatic);

  void SetLowerThreshold(int value);
  void SetUpperThreshold(int value);
  void SetAlpha(int value);
  void SetBeta(int value);
  void SetConductance(int value);
  void SetIterations(int value);

  void UpdateModels();
  void Reset3DView();

protected:
  vtkSlicerVesselSegmentationLogic();
  virtual ~vtkSlicerVesselSegmentationLogic();

  vtkSmartPointer<vtkMRMLVolumeNode> ActiveVolumeNode;

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void ObserveMRMLScene();
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  //virtual void OnMRMLSceneChanged(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
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
  vtkVesselSegHelper::SeedImageType::Pointer preprocessedImg; //= vtkVesselSegHelper::SeedImageType::New();

  int vtkScalarType;

  bool hepaticUpdated;
  vtkVesselSegHelper::SeedImageType::Pointer hepaticITKdata;
  vtkSmartPointer<vtkMRMLModelNode> hepaticModelNode;
  vtkSmartPointer<vtkMRMLModelDisplayNode> hepaticModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> hepaticLabelMap;

  bool portalUpdated;
  vtkVesselSegHelper::SeedImageType::Pointer portalITKdata;
  vtkSmartPointer<vtkMRMLModelNode> portalModelNode;
  vtkSmartPointer<vtkMRMLModelDisplayNode> portalModelDisplayNode;
  vtkSmartPointer<vtkMRMLLabelMapVolumeNode> portalLabelMap;

  bool mergedUpdated;
  vtkVesselSegHelper::SeedImageType::Pointer mergedITKdata;
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
