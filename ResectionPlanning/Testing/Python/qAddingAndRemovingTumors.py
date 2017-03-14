import slicer, qt, time
from qSlicerResectionPlanningModuleWidgetsPythonQt import *

def TestTumorAddAndRemove():
    
  rpw = slicer.modules.resectionplanning.widgetRepresentation()
  surfacesW = qSlicerResectionPlanningSurfacesWidget()

  mNode = slicer.vtkMRMLModelNode()
  mNode.SetName("LRPTumor0Model")
  slicer.mrmlScene.AddNode(mNode)
  
  success = true
   if not success :
     raise Exception('did not manage to add / remove tumors at widget level')
    
testStartTime = time.clock()
TestTumorAddAndRemove()
testEndTime = time.clock()
testTime = testEndTime - testStartTime
print "Test total time = ", testTime
