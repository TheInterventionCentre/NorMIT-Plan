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

#include <QString>

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
  if(tempModelNode != NULL)
  {
    std::cout << "Logic: Model node added!" << std::endl;

    // get name of node
    const char* name = tempModelNode->GetName();

    std::string strNode (name);
    QString index = name;

    std::cout << "Logic: model node name = " << strNode << std::endl;

    std::size_t found = strNode.find("Tumor");
    if (found!=std::string::npos)
    {
      std::cout << "'Tumor' found at: " << found << '\n';

      std::pair<vtkMRMLModelNode*, QString> pair;
      pair.first = tempModelNode;
      pair.second = name;
      this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAdded,
                        &pair);
    }
    found = strNode.find("tumor");
    if (found!=std::string::npos)
    {
      std::cout << "'tumor' found at: " << found << '\n';

      std::pair<vtkMRMLModelNode*, QString> pair;
      pair.first = tempModelNode;
      pair.second = index;
      this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAdded,
                        &pair);
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
