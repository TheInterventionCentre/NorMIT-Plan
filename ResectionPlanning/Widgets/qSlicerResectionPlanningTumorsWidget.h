/*=========================================================================
Program: NorMIT-Nav
Module: qSlicerResectionPlanningTumorsWidget.h
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

#ifndef __qSlicerResectionPlanningTumorsWidget_h
#define __qSlicerResectionPlanningTumorsWidget_h

// Qt includes
#include <QWidget>
#include <QPointer>

#include <map>

// FooBar Widgets includes
#include "qSlicerResectionPlanningModuleWidgetsExport.h"

class qSlicerResectionPlanningTumorsWidgetPrivate;
class QString;
class QListWidget;
class QListWidgetItem;

/* \ingroup ResectionPlanning
 *
 * \brief Contains widgets for adding/removing resections, and changing which tumors are associated with those resections
 *
 * This class sends and receives information to/from qSlicerResectionPlanningModuleWidget
 */
class Q_SLICER_MODULE_RESECTIONPLANNING_WIDGETS_EXPORT qSlicerResectionPlanningTumorsWidget
  : public QWidget
{
  Q_OBJECT
public:
  typedef QWidget Superclass;
  qSlicerResectionPlanningTumorsWidget(QWidget *parent=0);
  virtual ~qSlicerResectionPlanningTumorsWidget();

public slots:
  /**
   * Functions for exposing functionality for testing
   */
  QString SelectItemInAvailableTumors(QString&);
  QString SelectItemInResectionTumors(QString&);

  /**
   * Adds a tumor to the list of available tumors (listTumorsToAdd)
   *
   * @param tumor node name
   */
  void AddToTumorList(QString&,QString&);

  /**
   * Removes a tumor to the list of available tumors (listTumorsToAdd)
   *
   * @param tumor node name
   */
  void RemoveFromTumorList(QString&,QString&);

signals:
  /**
   * Signal emited when the button to add tumors to a resection is clicked
   *
   * @param first string is resection node name, second string is tumor node name
   */
  void AddTumorButtonClicked(QString&,QString&);

  /**
   * Signal emited when the button to remove tumors from a resection is clicked
   *
   * @param first string is resection node name, second string is tumor node name
   */
  void RemoveTumorButtonClicked(QString&,QString&);

protected slots:
  /**
   * Triggered when the button for adding a tumor to the resection is clicked
   */
  void OnAddTumorButtonClicked();

  /**
   * Triggered when the button for removing a tumor to the resection surface is clicked
   */
  void OnRemoveTumorButtonClicked();

protected:
  QScopedPointer<qSlicerResectionPlanningTumorsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningTumorsWidget);
  Q_DISABLE_COPY(qSlicerResectionPlanningTumorsWidget);

  std::map<QString, QListWidgetItem*> tumorIDtoItemMap;
  std::multimap<std::string, std::string> resectionToTumorMap;
};

#endif
