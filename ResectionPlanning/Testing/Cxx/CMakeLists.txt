set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------
set(TESTING_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS
  vtkBezierSurfaceWidgetTest1.cxx
  vtkLineWidget3Test1.cxx
  vtkMRMLResectionSurfaceNodeTest1.cxx
  vtkMRMLResectionSurfaceDisplayNodeTest1.cxx
  )

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  TARGET_LIBRARIES
  vtkSlicerResectionPlanningModuleMRML
  vtkSlicerResectionPlanningModuleVTKWidgets
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_ERROR_OUTPUT_CHECK
  )

#-----------------------------------------------------------------------------
simple_test(vtkBezierSurfaceWidgetTest1
  ${TESTING_DATA}/BezierSurfaceWidgetInteractionInput.log
  ${TESTING_DATA}/BezierSurfaceWidgetReferenceOutput.png)
simple_test(vtkLineWidget3Test1
  ${TESTING_DATA}/LineWidget3InteractionInput.log
  ${TESTING_DATA}/LineWidget3ReferenceOutput.png)
simple_test(vtkMRMLResectionSurfaceNodeTest1)
simple_test(vtkMRMLResectionSurfaceDisplayNodeTest1)
