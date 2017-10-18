set(KIT qSlicer${MODULE_NAME}Module)

set(TEST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../Data/testImage3_large.nii.gz")
set(TEST_FILE_SIMILARITY "${CMAKE_CURRENT_SOURCE_DIR}/../Data/testImage3_largeSimilarity.nii.gz")

#-----------------------------------------------------------------------------

set(KIT_TEST_SRCS
  vtkMRMLTestFiducialList.cxx
  vtkMRMLTestFileAndSegmentation.cxx
  EXTRA_INCLUDE vtkTestingOutputWindow.h
)

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
)

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  TARGET_LIBRARIES 
  vtkSlicerVesselSegmentationModuleMRMLDisplayableManager
  vtkSlicerVesselSegmentationModuleLogic
  qSlicerVesselSegmentationModuleWidgets
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_ERROR_OUTPUT_CHECK
)

#-----------------------------------------------------------------------------
#simple_test(qSlicer${MODULE_NAME}ModuleTest)
simple_test( vtkMRMLTestFiducialList ${TEST_FILE} )
simple_test( vtkMRMLTestFileAndSegmentation ${TEST_FILE} ${TEST_FILE_SIMILARITY} )
