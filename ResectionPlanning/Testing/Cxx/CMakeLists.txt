set(KIT qSlicer${MODULE_NAME}Module)

#-----------------------------------------------------------------------------
set(TESTING_DATA "${CMAKE_CURRENT_SOURCE_DIR}/../Data")

#-----------------------------------------------------------------------------
set(KIT_TEST_SRCS
  vtkBezierSurfaceWidgetTest1.cxx
  vtkLineWidget3Test1.cxx
  vtkLineWidget3Test2.cxx
  vtkMRMLResectionSurfaceNodeTest1.cxx
  vtkMRMLResectionSurfaceDisplayNodeTest1.cxx
  vtkMRMLResectionInitializationNodeTest1.cxx
  vtkMRMLResectionInitializationDisplayNodeTest1.cxx
  vtkMRMLResectionDisplayableManager3DTest1.cxx
  vtkMRMLResectionDisplayableManager2DTest1.cxx
  vtkMRMLResectionDisplayableManager2DTest2.cxx
  vtkSlicerResectionPlanningLogicTest1.cxx
  qSlicerSurfaceWidgetTest.cxx
  qSlicerTableItemWidgetTest.cxx
  qSlicerResectionPlanningSurfacesWidgetTest.cxx
  )

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
  )

#-----------------------------------------------------------------------------
set(KIT_TEST_GENERATE_MOC_SRCS
  qSlicerTableItemWidgetTest.cxx
  qSlicerResectionPlanningSurfacesWidgetTest.cxx
  )
if(CTK_QT_VERSION VERSION_LESS "5")
  QT4_GENERATE_MOCS(${KIT_TEST_GENERATE_MOC_SRCS})
else()
  QT5_GENERATE_MOCS(${KIT_TEST_GENERATE_MOC_SRCS})
endif()

include_directories( ${CMAKE_CURRENT_BINARY_DIR})

#-----------------------------------------------------------------------------
slicerMacroConfigureModuleCxxTestDriver(
  NAME ${KIT}
  SOURCES ${KIT_TEST_SRCS}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  TARGET_LIBRARIES
  vtkSlicerResectionPlanningModuleMRML
  vtkSlicerResectionPlanningModuleMRMLDisplayableManager
  vtkSlicerResectionPlanningModuleLogic
  vtkSlicerResectionPlanningModuleVTKWidgets
  qSlicerResectionPlanningModuleWidgets
  WITH_VTK_DEBUG_LEAKS_CHECK
  WITH_VTK_DEBUG_ERROR_CHECK
  )

#-----------------------------------------------------------------------------

# Test vtkBezierSurfaceWidget (interaction)
simple_test(vtkBezierSurfaceWidgetTest1
  ${TESTING_DATA}/BezierSurfaceWidgetInteractionInput.log
  ${TESTING_DATA}/BezierSurfaceWidgetReferenceOutput.png)

# Test vtkLineWidget3 (interaction)
simple_test(vtkLineWidget3Test1
  ${TESTING_DATA}/LineWidget3InteractionInput.log
  ${TESTING_DATA}/LineWidget3ReferenceOutput.png)

# Test vtkLineWidget3 (placement)
simple_test(vtkLineWidget3Test2
  ${TESTING_DATA}/LineWidget3ReferenceOutput2.png)

# Test resection surface node
simple_test(vtkMRMLResectionSurfaceNodeTest1)

# Test resection surface display node
simple_test(vtkMRMLResectionSurfaceDisplayNodeTest1)

# Test resection initialization
simple_test(vtkMRMLResectionInitializationNodeTest1)

# Test resection initialization display
simple_test(vtkMRMLResectionInitializationDisplayNodeTest1)

# Test the resection surface displayable manager 3D
simple_test(vtkMRMLResectionDisplayableManager3DTest1)

# Test the resection surface displayable manager 2D
simple_test(vtkMRMLResectionDisplayableManager2DTest1)

# Test the resection surface displayable manager 2D
simple_test(vtkMRMLResectionDisplayableManager2DTest2)

# Test resection planning logic
simple_test(vtkSlicerResectionPlanningLogicTest1)

# Test associating tumors to a resection
simple_test(qSlicerSurfaceWidgetTest)

# Test table item widget
simple_test(qSlicerTableItemWidgetTest)

# Test resection planning surfaces widget
simple_test(qSlicerResectionPlanningsurfacesWidgetTest)
