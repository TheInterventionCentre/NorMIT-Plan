set(KIT qSlicer${MODULE_NAME}Module)

set(TESTING_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

if(NOT EXISTS "${TESTING_DATA}/testImage3_large.gipl")
  # download image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/59ea6cec8d777f31ac6477c6/download ${TESTING_DATA}/testImage3_large.gipl)
endif()
if(NOT EXISTS "${TESTING_DATA}/testImage3_largeSimilarity.gipl")
  # download similarity file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/59ea6d0a8d777f31ac6477c9/download ${TESTING_DATA}/testImage3_largeSimilarity.gipl)
endif()

set(TEST_FILE "${TESTING_DATA}/testImage3_large.gipl")
set(TEST_FILE_SIMILARITY "${TESTING_DATA}/testImage3_largeSimilarity.gipl")

#-----------------------------------------------------------------------------

set(KIT_TEST_SRCS
  vtkMRMLVesselSegmentationSeedNodeTest1.cxx
  vtkMRMLTestFileAndSegmentation.cxx
  EXTRA_INCLUDE vtkTestingOutputWindow.h
)

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
  ${ITK_INCLUDE_DIRS}
)

set(${KIT}_TARGET_LIBRARIES
  ${ITK_LIBRARIES}
)
message( itk libraries: )
message( ${${KIT}_TARGET_LIBRARIES} )

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
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
