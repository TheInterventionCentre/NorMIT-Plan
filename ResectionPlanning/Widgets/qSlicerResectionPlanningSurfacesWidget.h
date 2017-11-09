/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningSurfacesWidget.h

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
#ifndef __qSlicerResectionPlanningSurfacesWidget_h
#define __qSlicerResectionPlanningSurfacesWidget_h

// This module includes
#include "vtkSlicerResectionPlanningLogic.h"

// Qt includes
#include <QWidget>
#include <QPointer>
#include <QMap>

// FooBar Widgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

//------------------------------------------------------------------------------
class qSlicerResectionPlanningSurfacesWidgetPrivate;
class QString;
class QTableWidget;
class QTableWidgetItem;
class vtkObject;
class qSlicerTableItemWidget;
class vtkMRMLResectionSurfaceNode;

/**
 *  \ingroup ResectionPlanning
 *
 * \brief Contains widgets for adding/removing resections, and changing which tumors are associated with those resections
 *
 * This class sends and receives information to/from qSlicerResectionPlanningModuleWidget
 */
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerResectionPlanningSurfacesWidget
  : public QWidget
{
  Q_OBJECT;

 public:
  typedef QWidget Superclass;
  qSlicerResectionPlanningSurfacesWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningSurfacesWidget();

  /**
   * Add resection widget entry
   *
   * @param node resection node-
   */
  void addResection(vtkMRMLResectionSurfaceNode *node);

  /**
   * Remove resection widget entry
   *
   * @param node resection node
   */
  void removeResection(vtkMRMLResectionSurfaceNode *node);

  public slots:

  /**
   * Select the row of the table specified by index
   *
   * @param index index to table row
   *
   */
  void selectTableRow(int index);

  /**
   * Returns the number of resections registered in the widget.
   *
   *
   * @return number of resections.
   */
  int getNumberOfResections() const;

   /**
    * Returns a TableItemWidget object
    *
    * @param index index to TableItemWidget
    *
    * @return corresponding tableItemWidget
    */
  qSlicerTableItemWidget *getTableItemWidget(unsigned int index);

  /**
   * Get a pointer to the resection node registered in the i-th entry of the widget.
   *
   * @param i index to entry in the widget.
   *
   * @return pointer to a vtkMRMLResectionSurfaceNode.
   */
  vtkMRMLResectionSurfaceNode* getResectionNode(unsigned int i);

  /**
   * Triggered when the button for removing a resection surface is clicked
   */
  void onRemoveSurfaceButtonClicked();

 signals:
  /**
   * Signal emited when the button to add a resection is clicked
   * No parameters, since no resection node exists yet
   */
  void addSurfaceButtonClicked();

  /**
   * Signal emited when the button to remove a resection is clicked
   *
   * @param node pointer to resection node.
   */
 void removeSurface(vtkMRMLResectionSurfaceNode* node);

 protected slots:
  /**
   * Triggered when the button for adding a resection surface is clicked
   */
  void onAddSurfaceButtonClicked();


  /**
   * Changes the visibility state of a resection node.
   *
   * @param state new visibility state.
   */
  void changeResectionVisibility(int state);

  /**
   * Chages the visibility state of the resection modification widget.
   *
   * @param state new visibility state.
   */
  void changeResectionWidgetVisibility(int state);

  /**
   * Changes the resection margin of a resection surface.
   *
   * @param margin double value representing the resection margin
   */
  void changeResectionMargin(double margin);

  /**
   * Changes the opacity of the resection for visualization
   *
   * @param opacity opacity value [0.0-1.0].
   */
  void changeResectionOpacity(double opacity);

 protected:
  QScopedPointer<qSlicerResectionPlanningSurfacesWidgetPrivate> d_ptr;

 private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningSurfacesWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningSurfacesWidget);

  QMap<vtkMRMLResectionSurfaceNode*, qSlicerTableItemWidget*> ResectionWidgetMap;
  typedef QMap<vtkMRMLResectionSurfaceNode*, qSlicerTableItemWidget*>::
    iterator ResectionWidgetIt;
};

#endif
