/*=========================================================================
Program: NorMIT-Nav
Module: vtkSlicerResectionPlanningLogic.h
Copyright (c) 2017 The Intervention Centre, Oslo University Hospital
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

/**
 * \ingroup ResectionPlanning
 *
 * \brief The logic watches the MRML scene and knows when changes occur
 *
 * This class sends information to qSlicerResectionPlanningModuleWidget, based on changes to the MRML scene.
 * It also receives information back from the Widget, so that it can keep track of the state based on user inputs.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_LOGIC_EXPORT vtkSlicerResectionPlanningLogic :
  public vtkSlicerModuleLogic
{
public:
  /**
   * Events to be triggered by the logic
   */
  enum LogicEventType
  {
    ResectionNodeAdded = 16534,
    ResectionNodeRemoved = 16535,
    TumorNodeAdded = 16536,
    TumorNodeRemoved = 16536,
  };

  /**
   * Instantiation of a new object
   */
  static vtkSlicerResectionPlanningLogic *New();

  vtkTypeMacro(vtkSlicerResectionPlanningLogic, vtkSlicerModuleLogic);

  /**
   * Print properties of the object
   *
   * @param os output stream
   * @param indent indentation
   */
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Set an association between a resection and a tumor
   *
   * @param resection node name
   * @param tumor node name
   */
  void SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName);

  /**
   * Remove an association between a resection and a tumor
   *
   * @param resection node name
   * @param tumor node name
   */
  void RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName);

protected:
  vtkSlicerResectionPlanningLogic();
  virtual ~vtkSlicerResectionPlanningLogic();

  /**
   * Attach events to the logic, so can listen to those events: NodeAddedEvent and NodeRemovedEvent
   */
  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);

  /**
   * Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
   */
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
