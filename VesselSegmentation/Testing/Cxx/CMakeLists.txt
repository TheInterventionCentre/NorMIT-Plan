set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------

set(KIT_TEST_SRCS
  vtkMRMLVesselSegmentationSeedNodeTest1.cxx
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
