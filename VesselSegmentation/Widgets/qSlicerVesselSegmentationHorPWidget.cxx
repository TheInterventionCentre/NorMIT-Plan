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
#include "qSlicerVesselSegmentationHorPWidget.h"
#include "ui_qSlicerVesselSegmentationHorPWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VesselSegmentation
class qSlicerVesselSegmentationHorPWidgetPrivate
: public Ui_qSlicerVesselSegmentationHorPWidget
{
    Q_DECLARE_PUBLIC(qSlicerVesselSegmentationHorPWidget);
protected:
    qSlicerVesselSegmentationHorPWidget* const q_ptr;
    
public:
    qSlicerVesselSegmentationHorPWidgetPrivate(
                                                 qSlicerVesselSegmentationHorPWidget& object);
    virtual void setupUi(qSlicerVesselSegmentationHorPWidget*);
};

// --------------------------------------------------------------------------
qSlicerVesselSegmentationHorPWidgetPrivate
::qSlicerVesselSegmentationHorPWidgetPrivate(
                                               qSlicerVesselSegmentationHorPWidget& object)
: q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void qSlicerVesselSegmentationHorPWidgetPrivate
::setupUi(qSlicerVesselSegmentationHorPWidget* widget)
{
    this->Ui_qSlicerVesselSegmentationHorPWidget::setupUi(widget);
}

//-----------------------------------------------------------------------------
// qSlicerVesselSegmentationHorPWidget methods

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationHorPWidget
::qSlicerVesselSegmentationHorPWidget(QWidget* parentWidget)
: Superclass( parentWidget )
, d_ptr( new qSlicerVesselSegmentationHorPWidgetPrivate(*this) )
{
    Q_D(qSlicerVesselSegmentationHorPWidget);
    d->setupUi(this);
    
    QObject::connect(d->hepaticRadio, SIGNAL(clicked()),
                     this, SLOT(OnHepaticRadioClicked()));
    QObject::connect(d->portalRadio, SIGNAL(clicked()),
                     this, SLOT(OnPortalRadioClicked()));
}

//-----------------------------------------------------------------------------
qSlicerVesselSegmentationHorPWidget
::~qSlicerVesselSegmentationHorPWidget()
{
}

//-----------------------------------------------------------------------------
// qSlicerLiverResectionNodeSelectorWidget slots

//-----------------------------------------------------------------------------
void qSlicerVesselSegmentationHorPWidget
::OnHepaticRadioClicked()
{
    emit HepaticRadioClicked();
}

void qSlicerVesselSegmentationHorPWidget
::OnPortalRadioClicked()
{
    emit PortalRadioClicked();
}
