/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningModuleWidget.h
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

#ifndef __qSlicerResectionPlanningModuleWidget_h
#define __qSlicerResectionPlanningModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"
#include "qSlicerResectionPlanningModuleExport.h"

#include <QString>

class qSlicerResectionPlanningModuleWidgetPrivate;
class qSlicerResectionPlanningModule;
class vtkSlicerResectionPlanningLogic;
class vtkEventQtSlotConnect;

/**
 *  \ingroup ResectionPlanning
 *
 * \brief Widget class for the module, contains the interface for the module.
 *
 * This class sends and receives information to/from the subwidgets (such as qSlicerResectionPlanningSurfacesWidget)
 * and also sends and receives information to/from the module logic (vtkSlicerResectionPlanningLogic)
 *
 */
class Q_SLICER_QTMODULES_RESECTIONPLANNING_EXPORT qSlicerResectionPlanningModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerResectionPlanningModuleWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningModuleWidget();

public slots:

protected:
  QScopedPointer<qSlicerResectionPlanningModuleWidgetPrivate> d_ptr;

  /**
   *  Called on initialization of the widget class
   */
  virtual void setup();

protected slots:
  void nodeSelectionChanged(vtkMRMLNode* node);
  void setMRMLScene(vtkMRMLScene* scene);

  /**
   * Called when a tumor is added to a resection via the gui in the Surfaces widget
   * connected to signal from surfaces widget: AddTumorButtonClicked
   * This function sends the parameter information up to the Logic
   *
   * @param resection node name
   * @param tumor node name
   */
  void OnAddTumorFromWidget(QPair<QString&,QString&>&);

  /**
   * Called when a tumor is removed from a resection via the gui in the Surfaces widget
   * connected to signal from surfaces widget: RemoveTumorButtonClicked
   * This function sends the parameter information up to the Logic
   *
   * @param resection node name
   * @param tumor node name
   */
  void OnRemoveTumorFromWidget(QPair<QString&,QString&>&);

  /**
   * Called when a model node, that is identified as a tumor, is added in the logic
   * connected to the event: vtkSlicerResectionPlanningLogic::TumorNodeAdded
   *
   * @param typically the vtk object that triggered the event (not used)
   * @param the event (not used)
   * @param client data (not used)
   * @param callData passes a pair of char* and QString,
   * which are the ID of node that has been added, and the name of the node
   */
  void OnTumorAdded(vtkObject* object,
                        unsigned long event,
                        void *clientData,
                        void *callData);

  /**
   * Called when a model node, that is identified as a tumor, is removed in the logic
   * connected to the event: vtkSlicerResectionPlanningLogic::TumorNodeRemoved
   *
   * @param typically the vtk object that triggered the event (not used)
   * @param the event (not used)
   * @param client data (not used)
   * @param callData passes a pair of char* and QString,
   * which are the ID of node that has been removed, and the name of the node
   */
  void OnTumorRemoved(vtkObject* object,
                        unsigned long event,
                        void *clientData,
                        void *callData);

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningModuleWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningModuleWidget);

  qSlicerResectionPlanningModule *Module;
  vtkSlicerResectionPlanningLogic *ModuleLogic;
  vtkEventQtSlotConnect *Connections;
};

#endif
