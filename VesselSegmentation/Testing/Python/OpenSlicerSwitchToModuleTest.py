import os
import urllib
from __main__ import vtk, qt, ctk, slicer
#
# scriptedTest
#

class OpenSlicerSwitchToModuleTest():
  """
  Tests the case of closing a scene with a displayable node in it while a GUI is up.
  """    
  
  #
  # first, get some sample data    
  #
  import SampleData
  sampleDataLogic = SampleData.SampleDataLogic()
  head = sampleDataLogic.downloadMRHead()    
  print('Load data')

  #
  # enter the vessel segmentation module
  #
  mainWindow = slicer.util.mainWindow()
  mainWindow.moduleSelector().selectModule('VesselSegmentation')
  print('Switch to vessel segmentation module')

  #
  # close the scene
  #    
  slicer.mrmlScene.Clear(0)
