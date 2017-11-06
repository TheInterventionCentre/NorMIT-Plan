/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerTableItemWidgetTest1.cxx

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

// QT includes
#include <QString>

// STD includes
#include <string>
#include <iostream>

// This module includes
#include "qSlicerTableItemWidget.h"

//-------------------------------------------------------------------------------
class qSlicerTableItemWidgetTester: public QObject
{
  Q_OBJECT;

private:

private slots:

  void testSetGetResectionName();

  void testSetGetResectionVisibility();

  void testSetGetResectionMargin();

  void testSetGetOpacity();

  void testSetGetWidgetVisibility();
};

//-------------------------------------------------------------------------------
void qSlicerTableItemWidgetTester::testSetGetResectionName()
{
  qSlicerTableItemWidget tableItemWidget;

  QString resection1 = QString("Resection1");
  tableItemWidget.setResectionName(resection1);

  QVERIFY(QString(tableItemWidget.getResectionName()).compare("Resection1") == 0);

  QString resection2 = QString("Resection2");
  tableItemWidget.setResectionName(resection2);
  QVERIFY(QString(tableItemWidget.getResectionName()).compare("Resection1") != 0);
}

//-------------------------------------------------------------------------------
void qSlicerTableItemWidgetTester::testSetGetResectionVisibility()
{
  qSlicerTableItemWidget tableItemWidget;
  tableItemWidget.setResectionVisibility(true);

  QVERIFY(tableItemWidget.getResectionVisibility());

  tableItemWidget.setResectionVisibility(false);
  QVERIFY(!tableItemWidget.getResectionVisibility());
}

//-------------------------------------------------------------------------------
void qSlicerTableItemWidgetTester::testSetGetResectionMargin()
{
  qSlicerTableItemWidget tableItemWidget;

  // Initially the value should be 0.0
  QVERIFY(tableItemWidget.getResectionMargin() == 0.0);

  // Value within the range
  tableItemWidget.setResectionMargin(50.0);
  QVERIFY(tableItemWidget.getResectionMargin() == 50.0);

  // Value out of range
  tableItemWidget.setResectionMargin(150.0);
  QVERIFY(tableItemWidget.getResectionMargin() == 50.0);

  // Value out of range
  tableItemWidget.setResectionMargin(-1.0);
  QVERIFY(tableItemWidget.getResectionMargin() == 50.0);

}

//-------------------------------------------------------------------------------
void qSlicerTableItemWidgetTester::testSetGetOpacity()
{
  qSlicerTableItemWidget tableItemWidget;

  //Initially the value should be 100.0
  QVERIFY(tableItemWidget.getResectionOpacity() == 100);

  // Correct value
  tableItemWidget.setResectionOpacity(50);
  QVERIFY(tableItemWidget.getResectionOpacity() == 50);

  // Value exceeding the limits
  tableItemWidget.setResectionOpacity(101);
  QVERIFY(tableItemWidget.getResectionOpacity() == 50);

  // Value exceeding the limits
  tableItemWidget.setResectionOpacity(-1);
  QVERIFY(tableItemWidget.getResectionOpacity() == 50);
}

//-------------------------------------------------------------------------------
void qSlicerTableItemWidgetTester::testSetGetWidgetVisibility()
{
  qSlicerTableItemWidget tableItemWidget;

  //Initially the value should be true
  QVERIFY(tableItemWidget.getWidgetVisibility());

  // Test for true
  tableItemWidget.setWidgetVisibility(true);
  QVERIFY(tableItemWidget.getWidgetVisibility());

  // Test for false
  tableItemWidget.setWidgetVisibility(false);
  QVERIFY(!tableItemWidget.getWidgetVisibility());
}


//-------------------------------------------------------------------------------
CTK_TEST_MAIN(qSlicerTableItemWidgetTest)
#include "moc_qSlicerTableItemWidgetTest.cxx"
