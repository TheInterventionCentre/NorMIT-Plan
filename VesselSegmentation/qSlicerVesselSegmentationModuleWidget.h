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

#ifndef __qSlicerVesselSegmentationModuleWidget_h
#define __qSlicerVesselSegmentationModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerVesselSegmentationModuleExport.h"

class qSlicerVesselSegmentationModuleWidgetPrivate;
class qSlicerVesselSegmentationModule;
class vtkSlicerVesselSegmentationLogic;
class vtkEventQtSlotConnect;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_VESSELSEGMENTATION_EXPORT qSlicerVesselSegmentationModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerVesselSegmentationModuleWidget(QWidget *parent=0);
  virtual ~qSlicerVesselSegmentationModuleWidget();
  virtual void enter();

signals:

protected:
  QScopedPointer<qSlicerVesselSegmentationModuleWidgetPrivate> d_ptr;

  virtual void setup();
    
protected slots:
  void nodeSelectionChanged(vtkMRMLNode*);

  void PreProcessing();

  void PlaceSeedsSeg();
  void RunSegment();
  void OnHepaticSeg();
  void OnPortalSeg();

  void MergeLabelMaps();
  void PlaceSeedsMerge();
  void RunSeedAssignment();
  void OnHepaticMerge();
  void OnPortalMerge();

  void OnLTSpin(int value);
  void OnUTSpin(int value);
  void OnAlphaSpin(int value);
  void OnBetaSpin(int value);
  void OnConductanceSpin(int value);
  void OnInterationsSpin(int value);

private:
  qSlicerVesselSegmentationModule *Module;
  vtkSlicerVesselSegmentationLogic *ModuleLogic;
  vtkEventQtSlotConnect *connections;

  Q_DECLARE_PRIVATE(qSlicerVesselSegmentationModuleWidget);
  Q_DISABLE_COPY(qSlicerVesselSegmentationModuleWidget);
};

#endif
