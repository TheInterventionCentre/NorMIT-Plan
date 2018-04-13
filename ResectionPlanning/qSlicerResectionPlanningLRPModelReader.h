/*=========================================================================

  Program: NorMIT-Plan
  Module: qSlicerResectionPlanningLRPModelReader.h

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

#ifndef __qSlicerResectionPlanningReader_h
#define __qSlicerResectionPlanningReader_h

// SlicerQt includes
#include "qSlicerFileReader.h"

class qSlicerResectionPlanningLRPModelReaderPrivate;
class vtkSlicerResectionPlanningLogic;

//-------------------------------------------------------------------------------
class qSlicerResectionPlanningLRPModelReader
: public qSlicerFileReader
{
  Q_OBJECT

 public:
  typedef qSlicerFileReader Superclass;

  /**
   * Constructor
   *
   * @param ResectionPlanningLogic  pointer to resection planning logic
   * @param parent parent object
   *
   */
  qSlicerResectionPlanningLRPModelReader(vtkSlicerResectionPlanningLogic* ResectionPlanningLogic=0,
                                 QObject* parent=0);
  /**
   * Destructor
   *
   */
  virtual ~qSlicerResectionPlanningLRPModelReader();

  /**
   * Set a new resection planning logic
   *
   * @param newResectionPlanningLogic pointer to new resection planning logic to be set.
   */
  void setResectionPlanningLogic(vtkSlicerResectionPlanningLogic* newResectionPlanningLogic);

  /**
   * Get the current resection planning logic.
   *
   *
   * @return assigned resection planning logic.
   */
  vtkSlicerResectionPlanningLogic* resectionPlanningLogic() const;

  /**
   * Get description
   *
   *
   * @return obtains the reader description
   */
  virtual QString description() const;

  /**
   * File type
   *
   *
   * @return input/output file types
   */
  virtual IOFileType fileType() const;

  /**
   * List of extensions associated with the reader
   *
   *
   * @return list of available extensions.
   */
  virtual QStringList extensions() const;

  /**
   * Load
   *
   * @param properties loading parameters
   *
   * @return true if the file was loaded, false otherwise.
   */
  virtual bool load(const IOProperties& properties);

protected:

  QScopedPointer<qSlicerResectionPlanningLRPModelReaderPrivate> d_ptr;

private:

  Q_DECLARE_PRIVATE(qSlicerResectionPlanningLRPModelReader);
  Q_DISABLE_COPY(qSlicerResectionPlanningLRPModelReader);

};


#endif
