/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkSlicerResectionPlanningLogic.h

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

#ifndef __vtkSlicerResectionPlanningLogic_h
#define __vtkSlicerResectionPlanningLogic_h

// This module includes
#include "vtkSlicerResectionPlanningModuleLogicExport.h"

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes
#include "vtkMRMLResectionSurfaceNode.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkWeakPointer.h>

// STD includes
#include <cstdlib>

//------------------------------------------------------------------------------
class vtkAppendPolyData;
class vtkMRMLModelNode;
class vtkMRMLLRPModelNode;
class vtkMRMLResectionSurfaceNode;
class vtkMRMLResectionInitializationNode;
class vtkMRMLLRPModelNode;

//------------------------------------------------------------------------------

/**
 * \ingroup ResectionPlanning
 *
 * \brief This class contains methods to support the high-level actions that the
 * user performs and which are related to the module. The class also serves as
 * coordinator between the MRML reality and the GUI.
 */
class VTK_SLICER_RESECTIONPLANNING_MODULE_LOGIC_EXPORT
vtkSlicerResectionPlanningLogic :
public vtkSlicerModuleLogic
{
 public:

  enum LogicEventType
  {
    ResectionNodeAdded = 16534, //!< Resection node added to MRML scene and processed by logic.
    ResectionNodeRemoved = 16535, //!< Resection node removed form MRML Scene and
                                //!   not needed by logic anymore.
    TumorModelAdded = 16536,    //!< Tumor node added and processed by the logic.
    TumorModelRemoved = 16537,  //!< Tumor node removed from MRML scene and not
                                //!   needed by the logic anymore.
    ParenchymaModelAdded,       //!< Parenchma node added and processed by the
                                //!   logic.
    ParenchymaModelRemoved,     //!< Parenchyma node removed from MRML scene and
                                //!  not needed by the logic anymore.
    HepaticModelAdded,          //!< Parenchma node added and processed by the
                                //!   logic.
    HepaticModelRemoved,        //!< Hepatic node removed from MRML scene and
                                //!  not needed by the logic anymore.
    PortalModelAdded,           //!< Parenchma node added and processed by the
                                //!   logic.
    PortalModelRemoved,         //!< Portal node removed from MRML scene and
                                //!  not needed by the logic anymore.

  };

  /**
   * Standard vtk object instantiation method.
   *
   * @return a pointer to the newly created object.
   */
  static vtkSlicerResectionPlanningLogic *New();

  vtkTypeMacro(vtkSlicerResectionPlanningLogic, vtkSlicerModuleLogic);

  /**
   * Standard vtk object function to print the properties of the object.
   *
   * @param os output stream where the properties should be printed to.
   * @param indent indentation value.
   */
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
   * Adds a new resection surface node and the associated display node to the
   * scene. Default values are used (0 tumors associated, 10.00mm resection
   * margin).
   */
  void AddResectionSurface();

  /**
   * Adds a new resection surface node (called from the reader)
   *
   * @param the file name of the resection being loaded
   * @return the resection node that has been added to the scene
   */
  vtkMRMLResectionSurfaceNode* AddResectionSurface(const char* filename);

  /**
   * Adds a new organ model node (called from the reader)
   *
   * @param filename the file name of the model being added.
   *
   * @return the model node that has been added to the scene.
   */
  vtkMRMLLRPModelNode* AddLRPModel(const char* filename);

  /**
   * Remove the resection specified by  nodeId.
   *
   * @param node pointer to node to remove.
   */
  void RemoveResection(vtkMRMLResectionSurfaceNode *node);

  /**
   * Auxiliary function to convert models to LRP models
   *
   * @param node vtkMRMLModelNode containing the model
   * @param structure type of structure to convert to: "parenchyma", "tumor", "hepatic", "portal".
   *
   * @return pointer to a newly created LRP node.
   */
  vtkMRMLLRPModelNode* ConvertModelNodeToLRPModelNode(vtkMRMLModelNode *node, const char* structure);

 protected:
  vtkSlicerResectionPlanningLogic();
  virtual ~vtkSlicerResectionPlanningLogic();

  /**
   * Setting the MRML scene internally.
   *
   * @param newScene pointer to the new MRML scene.
   */
  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);

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
   * Process nodes events (particularly for resection and resection initialization)
   *
   * @param object object triggering the event.
   * @param eventId event id
   * @param data additional data (not used).
   */
  virtual void ProcessMRMLNodesEvents(vtkObject *object,
                                      unsigned long int eventId,
                                      void *data);

  /**
   * Update the Bezier widget according to the initialization interaction.
   *
   * @param initNode pointer to initialization node.
   */
  void UpdateBezierWidgetOnInitialization(vtkMRMLResectionInitializationNode *initNode);

  /**
   * Turn off the visibilty of the resection widget for initiailzation purposes.
   *
   * @param initNode pointer to initialization node.
   */
  void HideResectionSurfaceOnInitialization(vtkMRMLResectionInitializationNode
                                            *initNode);

  /**
   * Turn off the visibility of the initialization widget.
   *
   * @param initNode pointer to initialization node.
   */
  void HideInitializationOnResectionModification(vtkMRMLResectionSurfaceNode
                                                 *initNode);

  vtkSmartPointer<vtkAppendPolyData> AppendTumors;

  // Pointer to the parenchyma node. The reader should note that there is only
  // one valid parenchyma.
  vtkWeakPointer<vtkMRMLModelNode> ParenchymaModelNode;

  // Resection initialization and resection node map
  std::map<vtkMRMLResectionInitializationNode*, vtkMRMLResectionSurfaceNode*>
    ResectionInitializationMap;
  typedef std::map<vtkMRMLResectionInitializationNode*,
    vtkMRMLResectionSurfaceNode *>
    ::iterator ResectionInitializationIt;

 private:
  vtkSlicerResectionPlanningLogic(const vtkSlicerResectionPlanningLogic&); // Not implemented
  void operator=(const vtkSlicerResectionPlanningLogic&); // Not implemented

};

#endif
