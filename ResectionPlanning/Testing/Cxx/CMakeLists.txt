set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS
  vtkMRMLResectionSurfaceNodeTest1.cxx
  vtkMRMLResectionSurfaceDisplayNodeTest1.cxx
  )

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  TARGET_LIBRARIES
  vtkSlicerResectionPlanningModuleMRML
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_ERROR_OUTPUT_CHECK
  )

#-----------------------------------------------------------------------------
simple_test(vtkMRMLResectionSurfaceNodeTest1)
simple_test(vtkMRMLResectionSurfaceDisplayNodeTest1)
