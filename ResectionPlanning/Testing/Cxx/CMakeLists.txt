set(KIT qSlicer${MODULE_NAME}Module)

if (CTK_QT_VERSION VERSION_GREATER 4)
find_package(Qt5Test REQUIRED)
endif()

set(QT5_LIBS "")

#-----------------------------------------------------------------------------
# Here we basically create the testing data directory and populate it with
# the testing data downloaded.

set(TESTING_DATA "${CMAKE_BINARY_DIR}/${MODULE_NAME}/Testing/Cxx/Data")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${MODULE_NAME}/Testing/Cxx/Data")

if(NOT EXISTS "${TESTING_DATA}/BezierSurfaceWidgetInteractionInput.log")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad0558d777f31ac64eebd/download
    ${TESTING_DATA}/BezierSurfaceWidgetInteractionInput.log)
endif()

if(NOT EXISTS "${TESTING_DATA}/BezierSurfaceWidgetReferenceOutput.png")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad0538d777f31ac64eeb7/download
    ${TESTING_DATA}/BezierSurfaceWidgetReferenceOutput.png)
endif()

if(NOT EXISTS "${TESTING_DATA}/LineWidget3InteractionInput.log")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad0558d777f31ac64eec0/download
    ${TESTING_DATA}/LineWidget3InteractionInput.log)
endif()

if(NOT EXISTS "${TESTING_DATA}/LineWidget3ReferenceOutput.png")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad0568d777f31ac64eec3/download
    ${TESTING_DATA}/LineWidget3ReferenceOutput.png)
endif()

if(NOT EXISTS "${TESTING_DATA}/LineWidget3ReferenceOutput2.png")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad0548d777f31ac64eeba/download
    ${TESTING_DATA}/LineWidget3ReferenceOutput2.png)
endif()

if(NOT EXISTS "${TESTING_DATA}/SampleModel.vtk")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ad5f58d777f31ac64eec6/download
    ${TESTING_DATA}/SampleModel.vtk)
endif()

if(NOT EXISTS "${TESTING_DATA}/ResectionSurface.vtk")
  file(DOWNLOAD https://data.kitware.com/api/v1/item/5a0ada528d777f31ac64eec9/download
    ${TESTING_DATA}/ResectionSurface.vtk)
endif()

#-----------------------------------------------------------------------------

set(TEMP_DIR "${CMAKE_BINARY_DIR}/${MODULE_NAME}/Testing/Cxx/TempData")
file(MAKE_DIRECTORY ${TEMP_DIR})

#-----------------------------------------------------------------------------
# Create configuration files for some tests

set(SAMPLE_MODEL_FILE "${TESTING_DATA}/SampleModel.vtk")
set(RESECTION_SURFACE_FILE "${TESTING_DATA}/ResectionSurface.vtk")

configure_file(
  vtkMRMLResectionSurfaceStorageNodeTest1Config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/vtkMRMLResectionSurfaceStorageNodeTest1Config.h
  )

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
  vtkMRMLResectionSurfaceStorageNodeTest1.cxx
  vtkMRMLResectionLRPModelNodeTest1.cxx
  vtkMRMLResectionLRPModelDisplayNodeTest1.cxx
  vtkSlicerResectionPlanningLogicTest1.cxx
  qSlicerSurfaceWidgetTest.cxx
  qSlicerTableItemWidgetTest.cxx
  qSlicerResectionPlanningSurfacesWidgetTest.cxx
  )

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
 )

if (CTK_QT_VERSION VERSION_GREATER 4)
   list(APPEND ${KIT}_INCLUDE_DIRECTORIES  ${Qt5Test_INCLUDE_DIRS})
   list(APPEND QT5_LIBS "Qt5::Test")
endif()

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
  ${QT5_LIBS}
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

# Unit testing for the surface storage node
simple_test(vtkMRMLResectionSurfaceStorageNodeTest1
  ${TESTING_DATA}/SampleModel.vtk
  ${TESTING_DATA}/ResectionSurface.vtk)

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

# Test LRP model node
simple_test(vtkMRMLResectionLRPModelNodeTest1)

# Test LRP model node
simple_test(vtkMRMLResectionLRPModelDisplayNodeTest1)
