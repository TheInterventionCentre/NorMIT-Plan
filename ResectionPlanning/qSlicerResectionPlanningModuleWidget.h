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

#ifndef __qSlicerResectionPlanningModuleWidget_h
#define __qSlicerResectionPlanningModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerResectionPlanningModuleExport.h"

class qSlicerResectionPlanningModuleWidgetPrivate;
class vtkMRMLNode;
class qSlicerResectionPlanningModule;
class vtkSlicerResectionPlanningLogic;
class vtkEventQtSlotConnect;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_RESECTIONPLANNING_EXPORT qSlicerResectionPlanningModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerResectionPlanningModuleWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningModuleWidget();

  /// Reimplemented for internal reasons
  void setMRMLScene(vtkMRMLScene* scene);

public slots:


protected:
  QScopedPointer<qSlicerResectionPlanningModuleWidgetPrivate> d_ptr;

  virtual void setup();

protected slots:
  void nodeSelectionChanged(vtkMRMLNode* node);

  void OnTumorAdded(vtkObject* object,
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
