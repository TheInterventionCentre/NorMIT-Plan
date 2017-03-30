


#ifndef __qSlicerResectionPlanningReader_h
#define __qSlicerResectionPlanningReader_h

// SlicerQt includes
#include "qSlicerFileReader.h"

class qSlicerResectionPlanningReaderPrivate;
class vtkSlicerResectionPlanningLogic;

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Models
class qSlicerResectionPlanningReader
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerResectionPlanningReader(vtkSlicerResectionPlanningLogic* ResectionPlanningLogic = 0, QObject* parent = 0);
  virtual ~qSlicerResectionPlanningReader();

  void setResectionPlanningLogic(vtkSlicerResectionPlanningLogic* newResectionPlanningLogic);
  vtkSlicerResectionPlanningLogic* resectionPlanningLogic()const;

  virtual QString description()const;
  virtual IOFileType fileType()const;
  virtual QStringList extensions()const;

  virtual bool load(const IOProperties& properties);

protected:
  QScopedPointer<qSlicerResectionPlanningReaderPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerResectionPlanningReader);
  Q_DISABLE_COPY(qSlicerResectionPlanningReader);
};

#endif
