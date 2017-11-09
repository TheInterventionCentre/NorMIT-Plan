/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningSurfacesWidgetTest.cxx

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

// CTK includes
#include <ctkTest.h>

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkNew.h>

// QT includes
#include <QString>
#include <QSignalSpy>

//This module includes
#include "qSlicerResectionPlanningSurfacesWidget.h"
#include "qSlicerTableItemWidget.h"
#include "vtkMRMLResectionSurfaceNode.h"
#include "vtkMRMLResectionSurfaceDisplayNode.h"

Q_DECLARE_METATYPE(vtkMRMLResectionSurfaceNode*);

//------------------------------------------------------------------------------
class qSlicerResectionPlanningSurfacesWidgetTester: public QObject
{
  Q_OBJECT;

 private:

 private slots:

  void testWidgetCreation();

  void testAddResection1();
  void testAddResection2();
  void testAddResection3();
  void testAddResection4();

  void testSelectTableRow();

  void testGetNumberOfResections();

  void testOnRemoveSurfaceButtonClicked1();

  void testGetResectionNode();

  void testRemoveResection1();
  void testRemoveResection2();

  void testGetTableItemWidget();

  void testChangeResectionVisibility();

  void testChangeWidgetVisibility();

  void testChangeResectionMargin();

  void testChangeResectionOpacity();
};

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testWidgetCreation()
{
  new qSlicerResectionPlanningSurfacesWidget(NULL);

  new qSlicerResectionPlanningSurfacesWidget(new QWidget());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testAddResection1()
{

  qSlicerResectionPlanningSurfacesWidget* surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());

  // No node passed.
  QTest::ignoreMessage(QtWarningMsg, "No resection node passed.");
  surfacesWidget->addResection(NULL);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testAddResection2()
{
  vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode;
  scene->AddNode(displayNode.GetPointer());

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode;
  surfaceNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  scene->AddNode(surfaceNode.GetPointer());

  // Add a node
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode.GetPointer());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testAddResection3()
{
  vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode;
  scene->AddNode(surfaceNode.GetPointer());

  // Add node without associated display node
  QTest::ignoreMessage(QtWarningMsg, "No resection display node associated to resection node.");
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode.GetPointer());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testAddResection4()
{
  vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode;
  scene->AddNode(displayNode.GetPointer());

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode;
  surfaceNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  scene->AddNode(surfaceNode.GetPointer());

  // Add a node
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode.GetPointer());

  // Try to add the same node
  QTest::ignoreMessage(QtWarningMsg, "Resection node already exists.");
  surfacesWidget->addResection(surfaceNode.GetPointer());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testSelectTableRow()
{
  vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode;
  scene->AddNode(displayNode.GetPointer());

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode;
  surfaceNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  scene->AddNode(surfaceNode.GetPointer());

  // Add a node
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode.GetPointer());

  // Try to select a row with invalid index
  QTest::ignoreMessage(QtWarningMsg, "Wrong index for row selection in table.");
  surfacesWidget->selectTableRow(-1);

  // Try to select a row with invalid index
  QTest::ignoreMessage(QtWarningMsg, "Wrong index for row selection in table.");
  surfacesWidget->selectTableRow(1);

  // Try to select a row with a valid index
  surfacesWidget->selectTableRow(0);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testGetNumberOfResections()
{
  vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode;
  scene->AddNode(displayNode.GetPointer());

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testOnRemoveSurfaceButtonClicked1()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);

  // Delete surface 2
  surfacesWidget->selectTableRow(1);
  surfacesWidget->onRemoveSurfaceButtonClicked();

  QVERIFY(qvariant_cast<vtkMRMLResectionSurfaceNode*>(spy.at(0).at(0)) == surfaceNode2.GetPointer());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testGetResectionNode()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);

  // Get the second node and test its value
  QVERIFY(surfacesWidget->getResectionNode(1) == surfaceNode2.GetPointer());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testRemoveResection1()
{

  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);

  // Delete surface 2
  surfacesWidget->selectTableRow(1);
  surfacesWidget->onRemoveSurfaceButtonClicked();

  QVERIFY(qvariant_cast<vtkMRMLResectionSurfaceNode*>(spy.at(0).at(0)) == surfaceNode2.GetPointer());

  // Try remove the resection
  QTest::ignoreMessage(QtWarningMsg, "No resection node  passed.");
  surfacesWidget->removeResection(NULL);
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testRemoveResection2()
{

  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);

  // Delete surface 2
  surfacesWidget->selectTableRow(1);
  surfacesWidget->onRemoveSurfaceButtonClicked();

  QVERIFY(qvariant_cast<vtkMRMLResectionSurfaceNode*>(spy.at(0).at(0)) == surfaceNode2.GetPointer());

  // Try remove the resection
  surfacesWidget->removeResection(surfaceNode2.GetPointer());

  // Veriry there is only 1 surface
  QVERIFY(surfacesWidget->getNumberOfResections() == 1);

  // Get the first node and test its value
   QVERIFY(surfacesWidget->getResectionNode(0) == surfaceNode1.GetPointer());

}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testGetTableItemWidget()
{
vtkNew<vtkMRMLScene> scene;

  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode;
  scene->AddNode(displayNode.GetPointer());

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  surfaceNode1->SetName("Resection1");
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qRegisterMetaType<vtkMRMLResectionSurfaceNode*>("vtkMRMLResectionSurfaceNode");
  QSignalSpy spy(surfacesWidget, SIGNAL(removeSurface(vtkMRMLResectionSurfaceNode*)));

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode2;
  scene->AddNode(displayNode2.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode2;
  surfaceNode2->SetAndObserveDisplayNodeID(displayNode2->GetID());
  surfaceNode2->SetName("Resection2");
  scene->AddNode(surfaceNode2.GetPointer());

  // Add a node to the table
  surfacesWidget->addResection(surfaceNode2.GetPointer());

  // Veriry 2 surfaces
  QVERIFY(surfacesWidget->getNumberOfResections() == 2);

  qSlicerTableItemWidget *item = surfacesWidget->getTableItemWidget(0);
  QVERIFY(item->getResectionName().simplified().compare(surfaceNode1->GetName())==0);

  qSlicerTableItemWidget *item2 = surfacesWidget->getTableItemWidget(1);
  QVERIFY(item2->getResectionName().simplified().compare(surfaceNode2->GetName())==0);
}


//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testChangeResectionVisibility()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qSlicerTableItemWidget *item = surfacesWidget->getTableItemWidget(0);

  // Check that visibility flag is the same
  QVERIFY(item->getResectionVisibility() == displayNode1->GetVisibility());

  // Change visibility
  item->setResectionVisibility(0);
  item->onVisibilityCheckBoxChanged(Qt::Unchecked);

  // Check that visibility flag is the same
  QVERIFY(item->getResectionVisibility() == displayNode1->GetVisibility());

  // Change visibility
  item->setResectionVisibility(1);
  item->onVisibilityCheckBoxChanged(Qt::Checked);

  // Check that visibility flag is the same
  QVERIFY(item->getResectionVisibility() == displayNode1->GetVisibility());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testChangeWidgetVisibility()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qSlicerTableItemWidget *item = surfacesWidget->getTableItemWidget(0);

  // Check that visibility flag is the same
  QVERIFY(item->getWidgetVisibility() == displayNode1->GetWidgetVisibility());

  // Change visibility
  item->setWidgetVisibility(0);
  item->onWidgetVisibilityCheckboxChanged(Qt::Unchecked);

  // Check that visibility flag is the same
  QVERIFY(item->getWidgetVisibility() == displayNode1->GetWidgetVisibility());

  // Change visibility
  item->setWidgetVisibility(1);
  item->onWidgetVisibilityCheckboxChanged(Qt::Checked);

  // Check that visibility flag is the same
  QVERIFY(item->getWidgetVisibility() == displayNode1->GetWidgetVisibility());
}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testChangeResectionMargin()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qSlicerTableItemWidget *item = surfacesWidget->getTableItemWidget(0);

  // Check that visibility flag is the same
  QVERIFY(item->getResectionMargin() == surfaceNode1->GetResectionMargin());

  item->setResectionMargin(50.0);
  QVERIFY(item->getResectionMargin() == 50.0 && surfaceNode1->GetResectionMargin() == 50.0);

  item->setResectionMargin(10.0);
  QVERIFY(item->getResectionMargin() == 10.0 && surfaceNode1->GetResectionMargin() == 10.0);

}

//------------------------------------------------------------------------------
void qSlicerResectionPlanningSurfacesWidgetTester::testChangeResectionOpacity()
{
  vtkNew<vtkMRMLScene> scene;

  // Add a node with associated display node
  vtkNew<vtkMRMLResectionSurfaceDisplayNode> displayNode1;
  scene->AddNode(displayNode1.GetPointer());
  vtkNew<vtkMRMLResectionSurfaceNode> surfaceNode1;
  surfaceNode1->SetAndObserveDisplayNodeID(displayNode1->GetID());
  scene->AddNode(surfaceNode1.GetPointer());

  // Add a node to the table
  qSlicerResectionPlanningSurfacesWidget *surfacesWidget =
    new qSlicerResectionPlanningSurfacesWidget(new QWidget());
  surfacesWidget->addResection(surfaceNode1.GetPointer());

  qSlicerTableItemWidget *item = surfacesWidget->getTableItemWidget(0);

  QVERIFY(item->getResectionOpacity() == displayNode1->GetOpacity()*100.0);

  item->setResectionOpacity(50.0);
  QVERIFY(item->getResectionOpacity() == 50.0 && displayNode1->GetOpacity()*100.0 == 50.0);

  item->setResectionOpacity(10.0);
  QVERIFY(item->getResectionOpacity() == 10.0 && displayNode1->GetOpacity()*100.0 == 10.0);

}

//------------------------------------------------------------------------------
CTK_TEST_MAIN(qSlicerResectionPlanningSurfacesWidgetTest)
#include "moc_qSlicerResectionPlanningSurfacesWidgetTest.cxx"
