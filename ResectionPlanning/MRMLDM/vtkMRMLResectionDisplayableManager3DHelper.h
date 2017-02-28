/*=========================================================================

  Program: NorMIT-Plan
  Module: vtkMRMLResectionDisplayableManager3DHelper.h

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

#ifndef __vtkMRMLResectionDisplayableManager3DHelper_h
#define __vtkMRMLResectionDisplayableManager3DHelper_h

// This module includes
#include "vtkSlicerResectionPlanningModuleMRMLDisplayableManagerExport.h"

// VTK includes
#include <vtkObject.h>
#include <vtkNew.h>

// STD includes
#include <vector>
#include <map>

//-------------------------------------------------------------------------------
class vtkMRMLResectionSurfaceNode;

class vtk3DWidget;
class vtkCollection;


//-------------------------------------------------------------------------------
class VTK_SLICER_RESECTIONPLANNING_MODULE_MRMLDISPLAYABLEMANAGER_EXPORT
vtkMRMLResectionDisplayableManager3DHelper :
public vtkObject
{
 public:

  // Description:
  // VTK required methods.
  static vtkMRMLResectionDisplayableManager3DHelper *New();
  vtkTypeMacro(vtkMRMLResectionDisplayableManager3DHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get a vtk3DWidget* given a node.
  vtk3DWidget* GetWidget(vtkMRMLResectionSurfaceNode* node);

  // Description:
  // List of nodes handled by the displayable manager and associated iterator.
  std::vector<vtkMRMLResectionSurfaceNode*> ResectionSurfaceNodeList;
  typedef std::vector<vtkMRMLResectionSurfaceNode*>::iterator
    ResectionSurfaceNodeListIt;

  // Description:
  // Map holding the relationships between widgets and nodes, and associated
  // iterator.
  std::map<vtkMRMLResectionSurfaceNode*, vtk3DWidget*> Widgets;
  typedef std::map<vtkMRMLResectionSurfaceNode*, vtk3DWidget*>::iterator
    WidgetsIt;

  // Description:
  // Keeps track of the relationship between widgets and nodes
  void RecordWidgetForNode(vtk3DWidget *widget,
                           vtkMRMLResectionSurfaceNode* node);


 protected:

  // Description:
  // Constructor & destructor.
  vtkMRMLResectionDisplayableManager3DHelper();
  ~vtkMRMLResectionDisplayableManager3DHelper();

 private:

  // Description:
  // Copy constructor & copy operator. Not implemented.
  vtkMRMLResectionDisplayableManager3DHelper(vtkMRMLResectionDisplayableManager3DHelper&);
  void operator=(vtkMRMLResectionDisplayableManager3DHelper&);

  // Description:
  // Collection holding the widgets. The reader should note that the
  // relationship between the widgets and the nodes is kept separately in the
  // std::map Widgets
  vtkNew<vtkCollection> WidgetsCollection;
};


#endif
