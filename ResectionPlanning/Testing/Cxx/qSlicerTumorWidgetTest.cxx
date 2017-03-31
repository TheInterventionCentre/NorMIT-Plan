/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerTumorWidgetTest.cxx

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

#include <qSlicerApplication.h>

#include "qSlicerResectionPlanningModule.h"

#include "qSlicerResectionPlanningTumorsWidget.h"
#include "ui_qSlicerResectionPlanningTumorsWidget.h"

#include <QString>

// The actual test function
int qSlicerTumorWidgetTest( int argc, char *argv[] )
{
  qSlicerApplication app(argc, argv);

  qSlicerResectionPlanningModule module;
  module.initialize(0);

  qSlicerResectionPlanningTumorsWidget *tumorsWidget = new qSlicerResectionPlanningTumorsWidget();

  // add tumor to available tumor list
  QString id1 = "idtumor1";
  QString name1 = "tumor1";
  tumorsWidget->AddToTumorList(id1, name1);

  // add 2nd tumor to available tumor list
  QString id2 = "idtumor2";
  QString name2 = "tumor2";
  tumorsWidget->AddToTumorList(id2, name2);

  // check these are both in the list
  bool find1 = false;
  bool find2 = false;
  QList<QString> tumorList = tumorsWidget->GetAvailableTumors();

  for (int i = 0; i < tumorList.size(); ++i) {
      if (tumorList.at(i) == id1)
      {
          cout << "Found id1 in available tumors, at position " << i << endl;
          find1 = true;
      }
      if (tumorList.at(i) == id2)
      {
          cout << "Found id2 in available tumors, at position " << i << endl;
          find2 = true;
      }
  }
  if( !(find1 && find2) )
  {
    std::cerr << "tumor1 and/or tumor1 not successfully added to available tumor list" << std::endl;
    return EXIT_FAILURE;
  }

  // move a tumor over to the resection list
  tumorsWidget->SelectItemInAvailableTumors(id1);
  tumorsWidget->ClickButtonAddTumor();

  // check if its in the list
  QList<QString> resectionList = tumorsWidget->GetResectionTumors();
  bool findInList = false;

  cout << "Resection list length " << resectionList.size() << endl;

  for (int i = 0; i < resectionList.size(); ++i) {
      if (resectionList.at(i) == id1)
      {
          cout << "Found id1 in resection tumors, at position " << i << endl;
          findInList = true;
      }
  }

  if( !findInList )
  {
    std::cerr << "tumor1 not successfully added to resection list" << std::endl;
    return EXIT_FAILURE;
  }

  // remove a tumor over from the resection list
  tumorsWidget->SelectItemInResectionTumors(id1);
  tumorsWidget->ClickButtonRemoveTumor();

  // check if its no longer in the list
  resectionList = tumorsWidget->GetResectionTumors();
  findInList = false;

  for (int i = 0; i < resectionList.size(); ++i) {
      if (resectionList.at(i) == id1)
      {
          cout << "Found id1 in resection tumors, at position " << i << endl;
          findInList = true;
      }
  }

  if( findInList )
  {
    std::cerr << "tumor1 not successfully removed from resection list" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;

}
