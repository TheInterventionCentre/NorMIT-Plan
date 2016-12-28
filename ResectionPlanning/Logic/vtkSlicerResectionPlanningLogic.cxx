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

// ResectionPlanning Logic includes
#include "vtkSlicerResectionPlanningLogic.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLModelNode.h>
#include <vtkMRMLDisplayNode.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>
#include <iostream>
#include <string.h>
#include <regex>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerResectionPlanningLogic);

//----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic::vtkSlicerResectionPlanningLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerResectionPlanningLogic::~vtkSlicerResectionPlanningLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::SetMRMLSceneInternal(vtkMRMLScene * newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------

/**
 * Node added to MRML Scene
 * set up observers of particular nodes
 */
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* addedNode)
{
  vtkMRMLModelNode* tempModelNode = vtkMRMLModelNode::SafeDownCast(addedNode);
  if(tempModelNode != nullptr)
  {
    std::cout << "Logic: Model node added!" << std::endl;

    std::regex parenchyma ("(.*)(parenchyma)(.*)");
    std::regex Parenchyma ("(.*)(Parenchyma)(.*)");
    std::regex tumor ("(.*)(tumor)(.*)");
    std::regex Tumor ("(.*)(Tumor)(.*)");
    std::regex hepatic ("(.*)(hepatic)(.*)");
    std::regex Hepatic ("(.*)(Hepatic)(.*)");
    std::regex portal ("(.*)(portal)(.*)");
    std::regex Portal ("(.*)(portal)(.*)");

    const char* name = tempModelNode->GetName();
    std::cout << "Logic: model node name = " << name << std::endl;

    if (std::regex_match (name, parenchyma) || std::regex_match (name, Parenchyma))
    {
      std::cout << "String matched to parenchyma" << std::endl;
      vtkMRMLDisplayNode* tempDisplayNode = tempModelNode->GetDisplayNode();
      tempDisplayNode->SetScalarVisibility(true);
    }

    if (std::regex_match (name, tumor) || std::regex_match (name, Tumor))
    {
      std::cout << "String matched to tumor" << std::endl;
      vtkMRMLDisplayNode* tempDisplayNode = tempModelNode->GetDisplayNode();
      tempDisplayNode->SetScalarVisibility(true);
    }

    if (std::regex_match (name, hepatic) || std::regex_match (name, Hepatic))
    {
      std::cout << "String matched to hepatic" << std::endl;
      vtkMRMLDisplayNode* tempDisplayNode = tempModelNode->GetDisplayNode();
      tempDisplayNode->SetScalarVisibility(true);
    }

    if (std::regex_match (name, portal) || std::regex_match (name, Portal))
    {
      std::cout << "String matched to portal" << std::endl;
      vtkMRMLDisplayNode* tempDisplayNode = tempModelNode->GetDisplayNode();
      tempDisplayNode->SetScalarVisibility(true);
    }

  }
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
