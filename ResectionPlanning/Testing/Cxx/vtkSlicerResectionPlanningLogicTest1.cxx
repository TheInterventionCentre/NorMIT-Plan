/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkSlicerResectionPlanningLogicTest1.cxx

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

// This module includes
#include "vtkSlicerResectionPlanningLogic.h"
#include "vtkMRMLResectionSurfaceNode.h"

// MRML includes
#include <vtkMRMLCoreTestingMacros.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkNew.h>
#include <vtkCommand.h>

// STD includes

//------------------------------------------------------------------------------
class ModelsCommand : public vtkCommand
{
public:
  vtkTypeMacro(ModelsCommand, vtkCommand);

  // Constructor
  ModelsCommand()
  {
    this->CurrentEventId = 0;
    this->CurrentModelID = "";
    this->CurrentModelName = "";
  }

  // Instantiation
  static ModelsCommand *New()
  {
    return new ModelsCommand;
  }


  // Execute command
  void Execute(vtkObject *vtkNotUsed(caller),
               unsigned long eventId,
               void *callData)
  {
    std::pair<std::string, std::string> *id_name =
      static_cast<std::pair<std::string, std::string>*>(callData);
    this->CurrentModelID = id_name->first;
    this->CurrentModelName = id_name->second;
    this->CurrentEventId = eventId;
  }

  void Reset()
  {
    this->CurrentEventId = 0;
    this->CurrentModelID = "";
    this->CurrentModelName = "";
  }

  // Member variables
  unsigned long CurrentEventId;
  std::string CurrentModelID;
  std::string CurrentModelName;
};

//------------------------------------------------------------------------------
int vtkSlicerResectionPlanningLogicTest1(int, char * [])
{
  vtkSmartPointer<vtkMRMLScene> scene1 =
    vtkSmartPointer<vtkMRMLScene>::New();
  vtkNew<vtkSlicerResectionPlanningLogic> logic1;
  logic1->SetMRMLScene(scene1);

  //----------------------------------------------------------------------------
  //Testing resections handling

  // Observe events related to model nodes and resectio nodes
  vtkSmartPointer<ModelsCommand> modelsCommand1 =
    vtkSmartPointer<ModelsCommand>::New();
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                      modelsCommand1);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ResectionNodeRemoved,
                      modelsCommand1);

  // Adding a resection node.
  vtkSmartPointer<vtkMRMLResectionSurfaceNode> resectionNode1 =
    vtkSmartPointer<vtkMRMLResectionSurfaceNode>::New();
  modelsCommand1->Reset();
  scene1->AddNode(resectionNode1);
  if (modelsCommand1->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::ResectionNodeAdded ||
      modelsCommand1->CurrentModelID.compare(resectionNode1->GetID()) ||
      modelsCommand1->CurrentModelName.compare(resectionNode1->GetName()))
    {
    std::cerr << "Failure to add a new resection node: "
              << "not handled properly by logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a resection node.
  modelsCommand1->Reset();
  scene1->RemoveNode(resectionNode1);
  if (modelsCommand1->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::ResectionNodeRemoved ||
      modelsCommand1->CurrentModelID.compare(resectionNode1->GetID()) ||
      modelsCommand1->CurrentModelName.compare(resectionNode1->GetName()))
    {
    std::cerr << "Failure to remove a resection node: "
              << "not handled properly by logic" << std::endl;
    return EXIT_FAILURE;
    }

  //END: Testing resections handling
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  //Testing models handling

  // Observe events related to model nodes and resectio nodes
  vtkSmartPointer<ModelsCommand> modelsCommand2 =
    vtkSmartPointer<ModelsCommand>::New();

  logic1->AddObserver(vtkSlicerResectionPlanningLogic::TumorModelAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::TumorModelRemoved,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ParenchymaModelAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ParenchymaModelRemoved,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::PortalModelAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::PortalModelRemoved,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::PortalModelAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::PortalModelRemoved,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::HepaticModelAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::HepaticModelRemoved,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ResectionNodeAdded,
                      modelsCommand2);
  logic1->AddObserver(vtkSlicerResectionPlanningLogic::ResectionNodeRemoved,
                      modelsCommand2);

  //Adding a parenchyma model -- correct naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> parenchymaModel1 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  parenchymaModel1->SetName("LRPParenchymaModel");
  scene1->AddNode(parenchymaModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::ParenchymaModelAdded ||
      modelsCommand2->CurrentModelID.compare(parenchymaModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(parenchymaModel1->GetName()))
    {
    std::cerr << "Failure to add a new parenchyma node model (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a parenchyma model -- correct naming
  modelsCommand2->Reset();
  scene1->RemoveNode(parenchymaModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::ParenchymaModelRemoved ||
      modelsCommand2->CurrentModelID.compare(parenchymaModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(parenchymaModel1->GetName()))
    {
    std::cerr << "Failure to removea parenchyma model node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Adding a parenchyma model -- wrong naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> parenchymaModel2 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  parenchymaModel2->SetName("LRPParenchymaModels");
  scene1->AddNode(parenchymaModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new parenchyma node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a parenchyma model node --wrong naming
  modelsCommand2->Reset();
  scene1->RemoveNode(parenchymaModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a parenchyma model node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a portal model -- correct naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> portalModel1 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  portalModel1->SetName("LRPPortalModel");
  scene1->AddNode(portalModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::PortalModelAdded ||
      modelsCommand2->CurrentModelID.compare(portalModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(portalModel1->GetName()))
    {
    std::cerr << "Failure to add a new portal node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a portal model -- correct naming
  modelsCommand2->Reset();
  scene1->RemoveNode(portalModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::PortalModelRemoved ||
      modelsCommand2->CurrentModelID.compare(portalModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(portalModel1->GetName()))
    {
    std::cerr << "Failure to removea portal model node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a portal model -- wrong naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> portalModel2 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  portalModel2->SetName("LRPPortalModels");
  scene1->AddNode(portalModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new portal node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a portal model node --wrong naming
  modelsCommand2->Reset();
  scene1->RemoveNode(portalModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a portal model node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a hepatic model -- correct naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> hepaticModel1 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  hepaticModel1->SetName("LRPHepaticModel");
  scene1->AddNode(hepaticModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::HepaticModelAdded ||
      modelsCommand2->CurrentModelID.compare(hepaticModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(hepaticModel1->GetName()))
    {
    std::cerr << "Failure to add a new hepatic node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a hepatic model -- correct naming
  modelsCommand2->Reset();
  scene1->RemoveNode(hepaticModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::HepaticModelRemoved ||
      modelsCommand2->CurrentModelID.compare(hepaticModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(hepaticModel1->GetName()))
    {
    std::cerr << "Failure to removea hepatic model node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a hepatic model -- wrong naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> hepaticModel2 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  hepaticModel2->SetName("LRPHepaticModels");
  scene1->AddNode(hepaticModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new hepatic node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a hepatic model node --wrong naming
  modelsCommand2->Reset();
  scene1->RemoveNode(hepaticModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a hepatic model node (with wrong naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a tumor model -- correct naming
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> tumorModel1 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  tumorModel1->SetName("LRPTumor1Model");
  scene1->AddNode(tumorModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::TumorModelAdded ||
      modelsCommand2->CurrentModelID.compare(tumorModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(tumorModel1->GetName()))
    {
    std::cerr << "Failure to add a new tumor node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a tumor model -- correct naming
  modelsCommand2->Reset();
  scene1->RemoveNode(tumorModel1);
  if (modelsCommand2->CurrentEventId !=
      vtkSlicerResectionPlanningLogic::TumorModelRemoved ||
      modelsCommand2->CurrentModelID.compare(tumorModel1->GetID()) ||
      modelsCommand2->CurrentModelName.compare(tumorModel1->GetName()))
    {
    std::cerr << "Failure to removea tumor model node (with correct naming): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a tumor model -- wrong naming (1)
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> tumorModel2 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  tumorModel2->SetName("LRPTumorModel");
  scene1->AddNode(tumorModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new tumor node (with wrong naming 1): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a tumor model node --wrong naming
  modelsCommand2->Reset();
  scene1->RemoveNode(tumorModel2);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a tumor model node (with wrong naming 1): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //Adding a tumor model -- wrong naming (2)
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> tumorModel3 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  tumorModel3->SetName("LRPTumor0sa00Model");
  scene1->AddNode(tumorModel3);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new tumor node (with wrong naming 2): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a tumor model node --wrong naming (2)
  modelsCommand2->Reset();
  scene1->RemoveNode(tumorModel3);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a tumor model node (with wrong naming 2): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

    //Adding a tumor model -- wrong naming (3)
  modelsCommand2->Reset();
  vtkSmartPointer<vtkMRMLModelNode> tumorModel4 =
    vtkSmartPointer<vtkMRMLModelNode>::New();
  tumorModel4->SetName("ALRPTumor1Model");
  scene1->AddNode(tumorModel4);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to add a new tumor node (with wrong naming 3): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  // Removing a tumor model node --wrong naming (3)
  modelsCommand2->Reset();
  scene1->RemoveNode(tumorModel4);
  if (modelsCommand2->CurrentEventId != 0 ||
      modelsCommand2->CurrentModelID.compare("") ||
      modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Failure to remove a tumor model node (with wrong naming 3): "
              << "not properly handled by the logic" << std::endl;
    return EXIT_FAILURE;
    }

  //END: Testing models handling
  //----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  // Testing add resection surface from logic

  modelsCommand2->Reset();
  logic1->AddResectionSurface();
  if (modelsCommand2->CurrentEventId == 0 ||
      !modelsCommand2->CurrentModelID.compare("") ||
      !modelsCommand2->CurrentModelName.compare(""))
    {
    std::cerr << "Error adding new resection surface from logic " << std::endl;
    return EXIT_FAILURE;
    }

  // END: Testing add resection surface from logic
  //----------------------------------------------------------------------------


  return EXIT_SUCCESS;
}
