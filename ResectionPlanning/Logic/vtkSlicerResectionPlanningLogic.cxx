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
#include "vtkMRMLResectionSurfaceNode.h"

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
#include <map>
#include <list>

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

  vtkMRMLResectionSurfaceNode* tempResectionNode = vtkMRMLResectionSurfaceNode::SafeDownCast(addedNode);
  if(tempResectionNode != NULL) // check if a resection node
  {
    this->resectionList.push_back(tempResectionNode);

  }
  else // check if a model node
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
        this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAddedToScene,
                          &pair);
        this->tumorList.push_back(tempModelNode);
      }
      found = strNode.find("tumor");
      if (found!=std::string::npos)
      {
        std::cout << "'tumor' found at: " << found << '\n';

        std::pair<vtkMRMLModelNode*, QString> pair;
        pair.first = tempModelNode;
        pair.second = index;
        this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAddedToScene,
                          &pair);
        this->tumorList.push_back(tempModelNode);
      }
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}


void vtkSlicerResectionPlanningLogic
::SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::cout << "'Resection: " << rsNodeName << " associated to tumor: " << tumorNodeName << '\n';
  resectionToTumorMap.insert(std::pair<std::string, std::string>(rsNodeName, tumorNodeName));
}

void vtkSlicerResectionPlanningLogic
::RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::multimap<std::string, std::string>::iterator it;

  for (it=resectionToTumorMap.begin(); it!=resectionToTumorMap.end(); ++it) {
    if(((*it).first == rsNodeName) && ((*it).second == tumorNodeName))
    {
      std::cout << "'Resection: " << (*it).first << " removed association to tumor: " << (*it).second << '\n';
      resectionToTumorMap.erase(it);
    }
  }
}
