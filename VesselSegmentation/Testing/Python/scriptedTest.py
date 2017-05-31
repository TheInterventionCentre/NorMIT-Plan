import os
import urllib
from __main__ import vtk, qt, ctk, slicer
#
# scriptedTest
#

class scriptedTest():
  downloads = ( ('http://10.163.46.32/midas/download?items=163', '0.nrrd', slicer.util.loadVolume),)

  for url,name,loader in downloads:
    filePath = slicer.app.temporaryPath + '/' + name
      if not os.path.exists(filePath) or os.stat(filePath).st_size == 0:
        #print('Requesting download %s from %s...\n' % (name, url))
        urllib.urlretrieve(url, filePath)
      if loader:
        #print('Loading %s...\n' % (name,))
        loader(filePath)
    #self.delayDisplay('Finished with download and loading\n')
