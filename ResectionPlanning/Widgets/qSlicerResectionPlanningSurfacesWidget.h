/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningSurfacesWidget.h
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

#ifndef __qSlicerResectionPlanningSurfacesWidget_h
#define __qSlicerResectionPlanningSurfacesWidget_h

// Qt includes
#include <QWidget>
#include <QPointer>

#include <map>

// FooBar Widgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class qSlicerResectionPlanningSurfacesWidgetPrivate;
class QString;
class QListWidget;
class QListWidgetItem;

/* \ingroup ResectionPlanning
 *
 * \brief Contains widgets for adding/removing resections, and changing which tumors are associated with those resections
 *
 * This class sends and receives information to/from qSlicerResectionPlanningModuleWidget
 */
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerResectionPlanningSurfacesWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerResectionPlanningSurfacesWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningSurfacesWidget();

  /**
   * Adds a tumor to the list of available tumors (listTumorsToAdd)
   *
   * @param tumor node name
   */
  void AddToTumorList(QPair<QString&,QString&>&);

  /**
   * Removes a tumor to the list of available tumors (listTumorsToAdd)
   *
   * @param tumor node name
   */
  void RemoveFromTumorList(QPair<QString&,QString&>&);

signals:

  /**
   * Signal emited when the button to add tumors to a resection is clicked
   *
   * @param first string is resection node name, second string is tumor node name
   */
  void AddTumorButtonClicked(QPair<QString&,QString&>&);

  /**
   * Signal emited when the button to remove tumors from a resection is clicked
   *
   * @param first string is resection node name, second string is tumor node name
   */
  void RemoveTumorButtonClicked(QPair<QString&,QString&>&);

protected slots:
  /**
   * Triggered when the button for adding a resection surface is clicked
   */
  void OnAddSurfaceButtonClicked();

  /**
   * Triggered when the button for removing a resection surface is clicked
   */
  void OnRemoveSurfaceButtonClicked();

  /**
   * Triggered when the button for adding a tumor to the resection is clicked
   */
  void OnAddTumorButtonClicked();

  /**
   * Triggered when the button for removing a tumor to the resection surface is clicked
   */
  void OnRemoveTumorButtonClicked();

protected:
  QScopedPointer<qSlicerResectionPlanningSurfacesWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningSurfacesWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningSurfacesWidget);

  std::map<QString, QListWidgetItem*> tumorIDtoItemMap;
};

#endif
