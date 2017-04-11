/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningSurfacesWidget.cxx

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
// This module includes
#include "qSlicerResectionPlanningSurfacesWidget.h"
#include "ui_qSlicerResectionPlanningSurfacesWidget.h"
#include "qSlicerTableItemWidget.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"

// VTK includes
#include <vtkObject.h>

// QT includes
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QLabel>
#include <QString>
#include <QSharedPointer>
#include <QWidget>
#include <QLabel>

// STD includes
#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ResectionPlanning
class qSlicerResectionPlanningSurfacesWidgetPrivate
  : public Ui_qSlicerResectionPlanningSurfacesWidget
{
  Q_DECLARE_PUBLIC(qSlicerResectionPlanningSurfacesWidget);
protected:
  qSlicerResectionPlanningSurfacesWidget* const q_ptr;

public:
  qSlicerResectionPlanningSurfacesWidgetPrivate(
    qSlicerResectionPlanningSurfacesWidget& object);
  virtual void setupUi(qSlicerResectionPlanningSurfacesWidget*);
};

// --------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidgetPrivate
::qSlicerResectionPlanningSurfacesWidgetPrivate(
  qSlicerResectionPlanningSurfacesWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetPrivate
::setupUi(qSlicerResectionPlanningSurfacesWidget* widget)
{
  this->Ui_qSlicerResectionPlanningSurfacesWidget::setupUi(widget);

}

// --------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::qSlicerResectionPlanningSurfacesWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerResectionPlanningSurfacesWidgetPrivate(*this) )
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);
  d->setupUi(this);

  // connect signals & slots for buttons to add & remove resection surface
  QObject::connect(d->AddSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(onAddSurfaceButtonClicked()));
  QObject::connect(d->RemoveSurfaceButton, SIGNAL(clicked()),
                   this, SLOT(onRemoveSurfaceButtonClicked()));
}

//-----------------------------------------------------------------------------
qSlicerResectionPlanningSurfacesWidget
::~qSlicerResectionPlanningSurfacesWidget()
{

}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::onAddSurfaceButtonClicked()
{
  emit addSurfaceButtonClicked();
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::addResection(vtkMRMLResectionSurfaceNode *resectionNode)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if (!resectionNode)
    {
    std::cerr << "Error: No resection node passed." << std::endl;
    return;
    }

  // Check whether we are handling this node
  if (this->ResectionWidgetMap.find(resectionNode) !=
      this->ResectionWidgetMap.end())
    {
    std::cerr << "Warning: node " << resectionNode->GetID()
              << "already exists" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode *resectionDisplayNode =
    vtkMRMLResectionSurfaceDisplayNode::SafeDownCast(
      resectionNode->GetDisplayNode());

  if (!resectionDisplayNode)
    {
    std::cerr << "Error: no display node." << std::endl;
    return;
    }

  QString resectionName =
    QString("%1").arg(resectionNode->GetName(), 20);


  //Create the table item widget
  qSlicerTableItemWidget *item = new qSlicerTableItemWidget();
  item->setResectionName(resectionName.toStdString().c_str());
  item->setResectionMargin(resectionNode->GetResectionMargin());
  item->setResectionVisibility(resectionDisplayNode->GetVisibility());
  item->setWidgetVisibility(resectionDisplayNode->GetWidgetVisibility());
  item->setOpacity(static_cast<int>(resectionDisplayNode->GetOpacity()*100));

  QObject::connect(item, SIGNAL(visibilityChanged(int)),
                   this, SLOT(changeResectionVisibility(int)));

  QObject::connect(item, SIGNAL(resectionMarginChanged(double)),
                   this, SLOT(changeResectionMargin(double)));

  QObject::connect(item, SIGNAL(resectionOpacityChanged(double)),
                   this, SLOT(changeResectionOpacity(double)));


  //Insert the item widget
  int row = d->TableResectionSurfaces->rowCount();
  d->TableResectionSurfaces->insertRow(row);
  d->TableResectionSurfaces->setCellWidget(row,0, item);

  // Register the nodeId -- itemWidget association
  this->ResectionWidgetMap[resectionNode] = item;
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget
::onRemoveSurfaceButtonClicked()
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  qSlicerTableItemWidget *widget =
    dynamic_cast<qSlicerTableItemWidget*>(
    d->TableResectionSurfaces->
    cellWidget(d->TableResectionSurfaces->currentRow(),0));

  if (!widget)
    {
    std::cerr << "Warning: no row selected" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    this->ResectionWidgetMap.key(widget);
  if (!resectionNode)
    {
    std::cerr << "Warning: no nodeId found for the given widget" << std::endl;
    return;
    }

  emit removeSurface(resectionNode);
}

//-----------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget::
removeResection(vtkMRMLResectionSurfaceNode *resectionNode)
{
  Q_D(qSlicerResectionPlanningSurfacesWidget);

  if (!resectionNode)
    {
    std::cerr << "Error: no node passed" << std::endl;
    return;
    }


  ResectionWidgetIt it = this->ResectionWidgetMap.find(resectionNode);
  if (it == this->ResectionWidgetMap.end())
    {
    std::cerr << "Warning: node " << resectionNode->GetID()
              << "does not exist in the list of resection surfaces"
              << std::endl;
    return;
    }

  int row;
  for(row = 0; row< d->TableResectionSurfaces->rowCount(); ++row)
    {
    qSlicerTableItemWidget *widget =
      dynamic_cast<qSlicerTableItemWidget*>(
        d->TableResectionSurfaces->cellWidget(row,0));
    if (widget)
      {
      if (widget == it.value())
        {
        break;
        }
      }
    }
  d->TableResectionSurfaces->removeCellWidget(row,0);
  d->TableResectionSurfaces->removeRow(row);
  this->ResectionWidgetMap.erase(it);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget::
changeResectionVisibility(int state)
{
  qSlicerTableItemWidget *item =
    dynamic_cast<qSlicerTableItemWidget*>(QObject::sender());

  if (!item)
    {
    std::cerr << "Error: the widget triggering resection visibility "
              << "change is not of type qSlicerTableItemWidget" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    this->ResectionWidgetMap.key(item);

  if (!resectionNode)
    {
    std::cerr << "Error: no resection node associated to the table item"
              << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode *displayNode =
    vtkMRMLResectionSurfaceDisplayNode::
    SafeDownCast(resectionNode->GetDisplayNode());

  if (!displayNode)
    {
    std::cerr << "Error: no display node associated to the resection node"
              << std::endl;
    return;
    }

  displayNode->SetVisibility((state == Qt::Checked)? true: false);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget::
changeResectionMargin(double value)
{
  qSlicerTableItemWidget *item =
    dynamic_cast<qSlicerTableItemWidget*>(QObject::sender());

  if (!item)
    {
    std::cerr << "Error: the widget triggering resection margin change "
              << "is not of type qSlicerTableItemWidget" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    this->ResectionWidgetMap.key(item);

  if (!resectionNode)
    {
    std::cerr << "Error: no resection node associated ot the table item"
              << std::endl;
    return;
    }

  resectionNode->SetResectionMargin(value);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidget::
changeResectionOpacity(double value)
{

 qSlicerTableItemWidget *item =
    dynamic_cast<qSlicerTableItemWidget*>(QObject::sender());

  if (!item)
    {
    std::cerr << "Error: the widget triggering resection margin change "
              << "is not of type qSlicerTableItemWidget" << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceNode *resectionNode =
    this->ResectionWidgetMap.key(item);

  if (!resectionNode)
    {
    std::cerr << "Error: no resection node associated ot the table item"
              << std::endl;
    return;
    }

  vtkMRMLResectionSurfaceDisplayNode *resectionDisplayNode =
    vtkMRMLResectionSurfaceDisplayNode::
    SafeDownCast(resectionNode->GetDisplayNode());

  if (!resectionDisplayNode)
    {
    std::cerr << "Error: no display node associated with the resection node"
              << std::endl;
    return;
    }

  resectionDisplayNode->SetOpacity(value/99.0);
}
