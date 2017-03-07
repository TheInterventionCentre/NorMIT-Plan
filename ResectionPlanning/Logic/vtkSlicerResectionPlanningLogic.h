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

// .NAME vtkSlicerResectionPlanningLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerResectionPlanningLogic_h
#define __vtkSlicerResectionPlanningLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRMLResectionSurfaceNode.h"

// STD includes
#include <cstdlib>

#include "vtkSlicerResectionPlanningModuleLogicExport.h"

class multimap;
class list;
class vtkMRMLModelNode;


/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_RESECTIONPLANNING_MODULE_LOGIC_EXPORT vtkSlicerResectionPlanningLogic :
  public vtkSlicerModuleLogic
{
public:

  enum LogicEventType
  {
    ResectionNodeAdded = 16534,
    ResectionNodeRemoved = 16535,
    TumorNodeAdded = 16536,
    TumorNodeRemoved = 16536,
  };

  static vtkSlicerResectionPlanningLogic *New();
  vtkTypeMacro(vtkSlicerResectionPlanningLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName);
  void RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName);

protected:
  vtkSlicerResectionPlanningLogic();
  virtual ~vtkSlicerResectionPlanningLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);

private:
  vtkSlicerResectionPlanningLogic(const vtkSlicerResectionPlanningLogic&); // Not implemented
  void operator=(const vtkSlicerResectionPlanningLogic&); // Not implemented

  std::list<vtkMRMLResectionSurfaceNode*> resectionList;
  std::list<vtkMRMLModelNode*> tumorList;
  std::multimap<std::string, std::string> resectionToTumorMap;

};

#endif
