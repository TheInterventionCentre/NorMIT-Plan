import slicer, qt, time
from qSlicerResectionPlanningModuleWidgetsPythonQt import *

def TestTumorAddAndRemove():
    
  rpw = slicer.modules.resectionplanning.widgetRepresentation()
  tumorW = qSlicerResectionPlanningTumorsWidget()

  success = true
   if not success :
     raise Exception('did not manage to add / remove tumors at widget level')
    
testStartTime = time.clock()
TestTumorAddAndRemove()
testEndTime = time.clock()
testTime = testEndTime - testStartTime
print "Test total time = ", testTime
