/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningModuleWidget.h

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

#ifndef __qSlicerResectionPlanningModuleWidget_h
#define __qSlicerResectionPlanningModuleWidget_h

// This module includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerResectionPlanningModuleExport.h"

// VTK includes
#include <vtkNew.h>

// Qt includes
#include <QString>
#include <QList>

// STD includes
#include <string>
#include <map>

//------------------------------------------------------------------------------
class qSlicerResectionPlanningModuleWidgetPrivate;
class qSlicerResectionPlanningModule;
class vtkSlicerResectionPlanningLogic;
class vtkMRMLResectionSurfaceNode;

/**
 *  \ingroup ResectionPlanning
 *
 * \brief Widget class for the module, contains the interface for the module.
 *
 * This class sends and receives information to/from the subwidgets (such as qSlicerResectionPlanningSurfacesWidget)
 * and also sends and receives information to/from the module logic (vtkSlicerResectionPlanningLogic)
 *
 */
class Q_SLICER_QTMODULES_RESECTIONPLANNING_EXPORT
qSlicerResectionPlanningModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerResectionPlanningModuleWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningModuleWidget();

  /**
   * Get a list of tumors associated with a particular resection
   *
   * @param resection ID
   * @return list of tumor IDs
   */
   std::list<std::string> GetTumorsAssociatedWithResection(std::string &resectionID);

   virtual void enter();
   virtual void exit();

public slots:

  /**
   * Called when a resection is added via the gui from the surfaces widget
   * connected to signal from surfaces widget: AddSurfaceButtonClicked
   * This function sends the parameter information up to the Logic
   */
  void onAddResection();

  /**
   * Called when a resection is removed via the gui from the surfaces widget
   * connected to signal from surfaces widget: RemoveSurfaceButtonClicked
   * This function sends the parameter information up to the Logic
   *
   * @param node pointer to resection node to remove
   */
  void onRemoveResection(vtkMRMLResectionSurfaceNode *node);

  /**
   * Actions to performe when a node is added to the scene
   *
   */
  void onNodeAddedEvent(vtkObject*, vtkObject *node);

  /**
   * Actions to perform when a node is removed from the scene.
   *
   */
  void onNodeRemovedEvent(vtkObject*, vtkObject *node);


protected:
  vtkSlicerResectionPlanningLogic *resectionPlanningLogic();

  QScopedPointer<qSlicerResectionPlanningModuleWidgetPrivate> d_ptr;

  /**
   *  Called on initialization of the widget class
   */
  virtual void setup();

protected slots:
  void nodeSelectionChanged(vtkMRMLNode* node);
  void setMRMLScene(vtkMRMLScene* scene);

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningModuleWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningModuleWidget);

};

#endif
