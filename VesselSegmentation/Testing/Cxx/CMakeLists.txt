set(KIT qSlicer${MODULE_NAME}Module)

set(TESTING_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

if(NOT EXISTS "${TESTING_DATA}/testImage3_large.nii.gz")
  # download image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/59e70a2c8d777f31ac647673/download ${TESTING_DATA}/testImage3_large.nii.gz)
endif()
if(NOT EXISTS "${TESTING_DATA}/testImage3_largeSimilarity.nii.gz")
  # download similarity file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/59e70a368d777f31ac647676/download ${TESTING_DATA}/testImage3_largeSimilarity.nii.gz)
endif()

set(TEST_FILE "${TESTING_DATA}/testImage3_large.nii.gz")
set(TEST_FILE_SIMILARITY "${TESTING_DATA}/testImage3_largeSimilarity.nii.gz")

#-----------------------------------------------------------------------------

set(KIT_TEST_SRCS
  vtkMRMLVesselSegmentationSeedNodeTest1.cxx
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
  vtkSlicerVesselSegmentationModuleMRML
  vtkSlicerVesselSegmentationModuleMRMLDisplayableManager
  vtkSlicerVesselSegmentationModuleLogic
  qSlicerVesselSegmentationModuleWidgets
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_ERROR_OUTPUT_CHECK
)

#-----------------------------------------------------------------------------
# Test seed node
simple_test(vtkMRMLVesselSegmentationSeedNodeTest1)
simple_test(vtkMRMLTestFileAndSegmentation ${TEST_FILE} ${TEST_FILE_SIMILARITY})
