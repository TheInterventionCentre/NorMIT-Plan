set(KIT qSlicer${MODULE_NAME}Module)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${MODULE_NAME}/Testing/Cxx/Data")
set(TESTING_DATA "${CMAKE_BINARY_DIR}/${MODULE_NAME}/Testing/Cxx/Data")

# preprocessing test
if(NOT EXISTS "${TESTING_DATA}/original_cropped.nrrd")
  # download image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a38d0dc8d777f5e872f5f9d/download ${TESTING_DATA}/original_cropped.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/preprocessedImage.nrrd")
  # download similarity file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a38d0d38d777f5e872f5f9a/download ${TESTING_DATA}/preprocessedImage.nrrd)
endif()

# segmentation test
if(NOT EXISTS "${TESTING_DATA}/testImage3_large.nrrd")
  # download image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/file/59ee0c9d8d777f31ac64783f/download ${TESTING_DATA}/testImage3_large.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/testImage3_similarity.nrrd")
  # download similarity file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a3a83b68d777f5e872f609a/download ${TESTING_DATA}/testImage3_similarity.nrrd)
endif()

# splitting test
if(NOT EXISTS "${TESTING_DATA}/preprocessedImage_split.nrrd")
  # download split image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a33fa8e8d777f5e872f5df4/download ${TESTING_DATA}/preprocessedImage_split.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/hepaticLabel.nrrd")
  # download split image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a33fa9e8d777f5e872f5dfa/download ${TESTING_DATA}/hepaticLabel.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/portalLabel.nrrd")
  # download split image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a33fa978d777f5e872f5df7/download ${TESTING_DATA}/portalLabel.nrrd)
endif()
if(NOT EXISTS "${TESTING_DATA}/testSplitFinal.nrrd")
  # download split image file from girder
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a379e748d777f5e872f5f46/download ${TESTING_DATA}/testSplitFinal.nrrd)
endif()

set(TEST_FILE "${TESTING_DATA}/testImage3_large.nrrd")

set(TEST_FILE_PREPROCESS "${TESTING_DATA}/original_cropped.nrrd")
set(TEST_FILE_PREPROCESS_SIMILARITY "${TESTING_DATA}/preprocessedImage.nrrd")
set(TEST_PREPROCESS_OUTPUT "${TESTING_DATA}/testPreprocessedOutput.nrrd")

set(TEST_FILE_SEGMENTATION "${TESTING_DATA}/testImage3_large.nrrd")
set(TEST_FILE_SEGMENTATION_SIMILARITY "${TESTING_DATA}/testImage3_similarity.nrrd")
set(TEST_SEGMENTATION_OUTPUT "${TESTING_DATA}/testSegmentationOutput.nrrd")

set(TEST_FILE_SPLIT "${TESTING_DATA}/preprocessedImage_split.nrrd")
set(TEST_LABEL_HEPATIC "${TESTING_DATA}/hepaticLabel.nrrd")
set(TEST_LABEL_PORTAL "${TESTING_DATA}/portalLabel.nrrd")
set(TEST_FILE_SPLIT_SIMILARITY "${TESTING_DATA}/testSplitFinal.nrrd")
set(TEST_SPLIT_OUTPUT "${TESTING_DATA}/testSplitOutput.nrrd")

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
  vtkMRMLVesselSegmentationSeedNodeTest1.cxx
  vtkMRMLVesselSegmentationDisplayableManager2DTest1.cxx
  vtkMRMLVesselSegmentationLogicTest1.cxx
  vtkVesselSegmentationHelperTest1.cxx
  vtkMRMLPreprocessingImageTest.cxx 
  vtkMRMLSegmentationAndSimilarityTest.cxx
  vtkMRMLMergeLabelsAndSplitTest.cxx
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
simple_test(vtkMRMLVesselSegmentationDisplayableManager2DTest1)
simple_test(vtkMRMLVesselSegmentationLogicTest1)
simple_test(vtkVesselSegmentationHelperTest1 ${TEST_FILE})
simple_test(vtkMRMLPreprocessingImageTest ${TEST_FILE_PREPROCESS} ${TEST_FILE_PREPROCESS_SIMILARITY} ${TEST_PREPROCESS_OUTPUT})
simple_test(vtkMRMLSegmentationAndSimilarityTest ${TEST_FILE_SEGMENTATION} ${TEST_FILE_SEGMENTATION_SIMILARITY} ${TEST_SEGMENTATION_OUTPUT})
simple_test(vtkMRMLMergeLabelsAndSplitTest ${TEST_FILE_SPLIT} ${TEST_LABEL_HEPATIC} ${TEST_LABEL_PORTAL} ${TEST_FILE_SPLIT_SIMILARITY} ${TEST_SPLIT_OUTPUT})
