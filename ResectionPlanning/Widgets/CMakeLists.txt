project(qSlicer${MODULE_NAME}ModuleWidgets)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES
  )

set(${KIT}_SRCS
  qSlicer${MODULE_NAME}SurfacesWidget.cxx
  qSlicer${MODULE_NAME}SurfacesWidget.h
  qSlicer${MODULE_NAME}TumorsWidget.cxx
  qSlicer${MODULE_NAME}TumorsWidget.h
  qSlicer${MODULE_NAME}VolumesWidget.cxx
  qSlicer${MODULE_NAME}VolumesWidget.h
  qSlicerTableItemWidget.cxx
  qSlicerTableItemWidget.h
  )

set(${KIT}_MOC_SRCS
  qSlicer${MODULE_NAME}SurfacesWidget.h
  qSlicer${MODULE_NAME}TumorsWidget.h
  qSlicer${MODULE_NAME}VolumesWidget.h
  qSlicerTableItemWidget.h
  )

set(${KIT}_UI_SRCS
  ../Resources/UI/qSlicer${MODULE_NAME}SurfacesWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}TumorsWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}VolumesWidget.ui
  ../Resources/UI/qSlicerTableItemWidget.ui
  )

set(${KIT}_RESOURCES
  ../Resources/UI/qSlicer${MODULE_NAME}SurfacesWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}TumorsWidget.ui
  ../Resources/UI/qSlicer${MODULE_NAME}VolumesWidget.ui
  ../Resources/UI/qSlicerTableItemWidget.ui
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
