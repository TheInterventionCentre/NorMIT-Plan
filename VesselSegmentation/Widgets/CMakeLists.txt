project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  ${CMAKE_BINARY_DIR}/${MODULE_NAME}
  )

set(${KIT}_SRCS
  qSlicer${MODULE_NAME}PreprocessingWidget.cxx
  qSlicer${MODULE_NAME}PreprocessingWidget.h
  qSlicer${MODULE_NAME}SegmentationWidget.cxx
  qSlicer${MODULE_NAME}SegmentationWidget.h
  qSlicer${MODULE_NAME}SplittingWidget.cxx
  qSlicer${MODULE_NAME}SplittingWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicer${MODULE_NAME}PreprocessingWidget.h
  qSlicer${MODULE_NAME}SegmentationWidget.h
  qSlicer${MODULE_NAME}SplittingWidget.h
  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicer${MODULE_NAME}PreprocessingWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}SegmentationWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}SplittingWidget.ui
  )

set(${KIT}_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleWidgets(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  UI_SRCS ${${KIT}_UI_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  RESOURCES ${${KIT}_RESOURCES}
  WRAP_PYTHONQT
  )
