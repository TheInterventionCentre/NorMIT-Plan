/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Jean-Christophe Fillion-Robin, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

// FooBar Widgets includes
#include "qSlicerVesselSegmentationFooBarWidget.h"
#include "ui_qSlicerVesselSegmentationFooBarWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VesselSegmentation
class qSlicerVesselSegmentationFooBarWidgetPrivate
  : public Ui_qSlicerVesselSegmentationFooBarWidget
{
  Q_DECLARE_PUBLIC(qSlicerVesselSegmentationFooBarWidget);
protected:
  qSlicerVesselSegmentationFooBarWidget* const q_ptr;

public:
  qSlicerVesselSegmentationFooBarWidgetPrivate(
    qSlicerVesselSegmentationFooBarWidget& object);
  virtual void setupUi(qSlicerVesselSegmentationFooBarWidget*);
};

// --------------------------------------------------------------------------
qSlicerVesselSegmentationFooBarWidgetPrivate
::qSlicerVesselSegmentationFooBarWidgetPrivate(
  qSlicerVesselSegmentationFooBarWidget& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerVesselSegmentationFooBarWidgetPrivate
::setupUi(qSlicerVesselSegmentationFooBarWidget* widget)
{
  this->Ui_qSlicerVesselSegmentationFooBarWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationFooBarWidget methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationFooBarWidget
::qSlicerVesselSegmentationFooBarWidget(QWidget* parentWidget)
  : Superclass( parentWidget )
  , d_ptr( new qSlicerVesselSegmentationFooBarWidgetPrivate(*this) )
{
  Q_D(qSlicerVesselSegmentationFooBarWidget);
  d->setupUi(this);
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationFooBarWidget
::~qSlicerVesselSegmentationFooBarWidget()
{
}
