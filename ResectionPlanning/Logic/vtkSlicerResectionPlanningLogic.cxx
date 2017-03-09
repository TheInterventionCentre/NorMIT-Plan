/*=========================================================================
  Program: NorMIT-Plan
  Module: vtkSlicerResectionPlanningLogic.cxx
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
      // get name of node
      const char* name = tempModelNode->GetName();
      const char* ID = tempModelNode->GetID();

      std::string strNode (name);
      QString nameQ = name;
      QString idQ = ID;

      std::cout << "Logic: model node name = " << strNode << std::endl;

      std::size_t found = strNode.find("Tumor");
      if (found!=std::string::npos)
      {
        std::cout << "'Tumor' found at: " << found << '\n';

        std::pair<QString, QString> pair;
        pair.first = idQ;
        pair.second = nameQ;
        this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAdded,
                          &pair);
        this->tumorList.push_back(tempModelNode);
      }
      found = strNode.find("tumor");
      if (found!=std::string::npos)
      {
        std::cout << "'tumor' found at: " << found << '\n';

        std::pair<QString, QString> pair;
        pair.first = idQ;
        pair.second = nameQ;
        this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeAdded,
                          &pair);
        this->tumorList.push_back(tempModelNode);
      }
    }
  }
}

//---------------------------------------------------------------------------
void vtkSlicerResectionPlanningLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* addedNode)
{
  vtkMRMLResectionSurfaceNode* tempResectionNode = vtkMRMLResectionSurfaceNode::SafeDownCast(addedNode);
   if(tempResectionNode != NULL) // check if a resection node
   {
     this->resectionList.remove(tempResectionNode);
   }
   else // check if a model node
   {
     vtkMRMLModelNode* tempModelNode = vtkMRMLModelNode::SafeDownCast(addedNode);
     if(tempModelNode != NULL)
     {
       // get name of node
       const char* name = tempModelNode->GetName();
       const char* ID = tempModelNode->GetID();

       std::string strNode (name);
       QString nameQ = name;
       QString idQ = ID;

       std::cout << "Logic: model node name = " << strNode << std::endl;

       std::size_t found = strNode.find("Tumor");
       if (found!=std::string::npos)
       {
         std::cout << "'Tumor' found at: " << found << '\n';

         std::pair<QString, QString> pair;
         pair.first = idQ;
         pair.second = nameQ;
         this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeRemoved,
                           &pair);
         this->tumorList.remove(tempModelNode);
       }
       found = strNode.find("tumor");
       if (found!=std::string::npos)
       {
         std::cout << "'tumor' found at: " << found << '\n';

         std::pair<QString, QString> pair;
         pair.first = idQ;
         pair.second = nameQ;
         this->InvokeEvent(vtkSlicerResectionPlanningLogic::TumorNodeRemoved,
                           &pair);
         this->tumorList.remove(tempModelNode);
       }
     }
   }
}


void vtkSlicerResectionPlanningLogic
::SetTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::cout << "'Resection: " << rsNodeName << ", associated to tumor: " << tumorNodeName << '\n';
  this->resectionToTumorMap.insert(std::pair<std::string, std::string>(rsNodeName, tumorNodeName));
}

void vtkSlicerResectionPlanningLogic
::RemoveTumorToResectionAssociation(std::string rsNodeName, std::string tumorNodeName)
{
  std::multimap<std::string, std::string>::iterator it;

  for (it=resectionToTumorMap.begin(); it!=resectionToTumorMap.end(); ++it) {
    if(((*it).first == rsNodeName) && ((*it).second == tumorNodeName))
    {
      std::cout << "'Resection: " << (*it).first << ", removed association to tumor: " << (*it).second << '\n';
      this->resectionToTumorMap.erase(it);
    }
  }
}
