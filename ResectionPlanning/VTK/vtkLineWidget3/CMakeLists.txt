PROJECT(vtkLineWidget3)

set(KIT ${PROJECT_NAME})

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

set(${KIT}_INCLUDE_DIRECTORIES)

set(${KIT}_SRCS
  vtkLineWidget3.cxx
  vtkLineWidget3.h
  vtkLineRepresentation3.cxx
  vtkLineRepresentation3.h
)

set(${KIT}_TARGET_LIBRARIES
  ${VTK_LIBRARIES}
)

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
