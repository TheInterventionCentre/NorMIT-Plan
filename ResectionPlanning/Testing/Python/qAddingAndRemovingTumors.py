import slicer, qt, time
from qSlicerResectionPlanningModuleWidgetsPythonQt import *

def TestTumorAddAndRemove():
  
  success = true
    
  rpw = slicer.modules.resectionplanning.widgetRepresentation()
  tumorW = qSlicerResectionPlanningTumorsWidget()

  # add a bunch of tumors to to the available tumor list
  tumorW.AddToTumorList("id1","tumor1")
  tumorW.AddToTumorList("id2","tumor2")
  tumorW.AddToTumorList("id3","tumor3")

  # select the 2nd tumor
  success = tumorW.SelectItemInAvailableTumors("id2")
  tumorW.ClickButtonAddTumor()

  # check the tumor has been added to the resection list
  success = tumorW.CheckIfInResectionTumors("id2")

  # select and remove the tumor from the resection list
  success = tumorW.SelectItemInResectionTumors("id2")
  tumorW.ClickButtonRemoveTumor()
    
  # check the tumor has been removed from the resection list
  success = not(tumorW.CheckIfInResectionTumors("id2"))
    
  # add the other tumors to the resection list
  success = tumorW.SelectItemInAvailableTumors("id1")
  tumorW.ClickButtonAddTumor()
  success = tumorW.SelectItemInAvailableTumors("id3")
  tumorW.ClickButtonAddTumor()

  # check they are both there
  success = tumorW.CheckIfInResectionTumors("id1")
  success = tumorW.CheckIfInResectionTumors("id3")
    
  # remove one of them
  success = tumorW.SelectItemInResectionTumors("id1")
  tumorW.ClickButtonRemoveTumor()
  success = not(tumorW.CheckIfInResectionTumors("id1"))
    
  # check the other is still there 
  success = tumorW.CheckIfInResectionTumors("id3")

  if not success :
    raise Exception('did not manage to add / remove tumors at widget level')
    
testStartTime = time.clock()
TestTumorAddAndRemove()
testEndTime = time.clock()
testTime = testEndTime - testStartTime
print "Test total time = ", testTime
