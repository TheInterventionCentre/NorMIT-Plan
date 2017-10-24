set(KIT qSlicer${MODULE_NAME}Module)

set(TESTING_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

if(NOT EXISTS "${TESTING_DATA}/testImage3_large.nrrd")
  # download image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/file/59ee0c9d8d777f31ac64783f/download ${TESTING_DATA}/testImage3_large.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/testImage3_largeSimilarity.nrrd")
  # download similarity file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/file/59ee0cb68d777f31ac647842/download ${TESTING_DATA}/testImage3_largeSimilarity.nrrd)
endif()

set(TEST_FILE "${TESTING_DATA}/testImage3_large.nrrd")
set(TEST_FILE_SIMILARITY "${TESTING_DATA}/testImage3_largeSimilarity.nrrd")

#-----------------------------------------------------------------------------
# ITK 
# see cmake list in volumes module: https://github.com/Slicer/Slicer/blob/master/Modules/Loadable/Volumes/Testing/Cxx/CMakeLists.txt

set(${KIT}Testing_ITK_COMPONENTS
  ITKCommon
)
find_package(ITK 4.6 COMPONENTS ${${KIT}Testing_ITK_COMPONENTS} REQUIRED)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER 1) # See Libs/ITKFactoryRegistration/CMakeLists.txt
list(APPEND ITK_LIBRARIES ITKFactoryRegistration)
list(APPEND ITK_INCLUDE_DIRS
  ${ITKFactoryRegistration_INCLUDE_DIRS}
)
include(${ITK_USE_FILE})

#-----------------------------------------------------------------------------

set(KIT_TEST_SRCS
  vtkVesselSegmentationHelperTest1.cxx
  EXTRA_INCLUDE vtkTestingOutputWindow.h
)

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
)

set(${KIT}_TARGET_LIBRARIES
)

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  vtkSlicerVesselSegmentationModuleMRMLDisplayableManager
  vtkSlicerVesselSegmentationModuleLogic
  qSlicerVesselSegmentationModuleWidgets
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_ERROR_OUTPUT_CHECK
)

#-----------------------------------------------------------------------------
# Test seed node
simple_test(vtkVesselSegmentationHelperTest1 ${TEST_FILE})
