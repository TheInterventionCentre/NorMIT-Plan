/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerSurfaceWidgetTest.cxx

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

#include "qSlicerResectionPlanningSurfacesWidget.h"
#include "ui_qSlicerResectionPlanningSurfacesWidget.h"

#include <QString>

// The actual test function
int qSlicerSurfaceWidgetTest( int argc, char *argv[] )
{
  qSlicerApplication app(argc, argv);

  qSlicerResectionPlanningModule module;
  module.initialize(0);

  qSlicerResectionPlanningSurfacesWidget *surfacesWidget = new qSlicerResectionPlanningSurfacesWidget();

  // add a resection
  QString idresection1 = "idresection1";
  QString nameresection1 = "resection1";
  surfacesWidget->AddToResectionList(idresection1, nameresection1);

  // add a resection
  QString idresection2 = "idresection2";
  QString nameresection2 = "resection2";
  surfacesWidget->AddToResectionList(idresection2, nameresection2);

  QList<QString> resectionList = surfacesWidget->GetResections();
  bool findResection1 = false;
  bool findResection2 = false;

  for (int i = 0; i < resectionList.size(); ++i) {
      if (resectionList.at(i) == idresection1)
      {
          cout << "Found resection1 in resection tumors, at position " << i << endl;
          findResection1 = true;
      }
      if (resectionList.at(i) == idresection2)
      {
          cout << "Found resection2 in resection tumors, at position " << i << endl;
          findResection2 = true;
      }
  }
  if(resectionList.size() != 2)
  {
    std::cerr << "resection list length is not equal to 2, length: " << resectionList.size() << std::endl;
    return EXIT_FAILURE;
  }
  if(!(findResection1 && findResection2))
  {
    std::cerr << "resection1 and/or resection2 or not successfully added to resection list" << std::endl;
    return EXIT_FAILURE;
  }

  // check that no resection is currently selected
  QString currentResection = surfacesWidget->GetCurrentResectionID();
  if(currentResection != NULL)
  {
    std::cerr << "resection1 not successfully selected as current resection" << std::endl;
    return EXIT_FAILURE;
  }

  // select the resection, and check its the current resection
  surfacesWidget->SelectResection(idresection1);
  currentResection = surfacesWidget->GetCurrentResectionID();
  if(currentResection != idresection1)
  {
    std::cerr << "resection1 not successfully selected as current resection" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
