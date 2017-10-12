MESSAGE(STATUS "ITK build directory: " ${ITK_DIR})
FILE(READ ${ITK_DIR}/CMakeCache.txt ITK_cmake)
STRING(FIND "${ITK_cmake}" "ITK_USE_GPU:BOOL=ON" gpu_flag)
if(${gpu_flag} EQUAL -1)
    MESSAGE(STATUS "ITK_USE_GPU: off (not defined)")
    # ITK_USE_GPU will be false
    configure_file(
      VesselSegmentationITKuseGPU.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/VesselSegmentationITKuseGPU.h
    )
else()
    MESSAGE(STATUS "ITK_USE_GPU: on")
    # set ITK_USE_GPU to be true
    set(ITK_USE_GPU 1)
    configure_file(
      VesselSegmentationITKuseGPU.h.in
      ${CMAKE_CURRENT_BINARY_DIR}/VesselSegmentationITKuseGPU.h
    )
endif()

#-----------------------------------------------------------------------------
set(MODULE_NAME VesselSegmentation)
set(MODULE_TITLE ${MODULE_NAME})

string(TOUPPER ${MODULE_NAME} MODULE_NAME_UPPER)

#-----------------------------------------------------------------------------
add_subdirectory(MRMLDM)
add_subdirectory(Logic)
add_subdirectory(Widgets)

#include_directories(ITKFilters)

#-----------------------------------------------------------------------------
set(MODULE_EXPORT_DIRECTIVE "Q_SLICER_QTMODULES_${MODULE_NAME_UPPER}_EXPORT")

# Current_{source,binary} and Slicer_{Libs,Base} already included
set(MODULE_INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Logic
  ${CMAKE_CURRENT_BINARY_DIR}/Logic
  ${CMAKE_CURRENT_SOURCE_DIR}/Widgets
  ${CMAKE_CURRENT_BINARY_DIR}/Widgets
  )

include_directories(${MODULE_INCLUDE_DIRECTORIES})
message(STATUS "IncludeDirs: " ${MODULE_INCLUDE_DIRECTORIES})

set(MODULE_SRCS
  qSlicer${MODULE_NAME}Module.cxx
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.cxx
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_MOC_SRCS
  qSlicer${MODULE_NAME}Module.h
  qSlicer${MODULE_NAME}ModuleWidget.h
  )

set(MODULE_UI_SRCS
  Resources/UI/qSlicer${MODULE_NAME}ModuleWidget.ui
  )

set(MODULE_TARGET_LIBRARIES
  vtkSlicer${MODULE_NAME}ModuleLogic
  qSlicer${MODULE_NAME}ModuleWidgets
  vtkSlicer${MODULE_NAME}ModuleMRMLDisplayableManager
  )

set(MODULE_RESOURCES
  Resources/qSlicer${MODULE_NAME}Module.qrc
  )

#-----------------------------------------------------------------------------
slicerMacroBuildLoadableModule(
  NAME ${MODULE_NAME}
  TITLE ${MODULE_TITLE}
  EXPORT_DIRECTIVE ${MODULE_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${MODULE_INCLUDE_DIRECTORIES}
  SRCS ${MODULE_SRCS}
  MOC_SRCS ${MODULE_MOC_SRCS}
  UI_SRCS ${MODULE_UI_SRCS}
  TARGET_LIBRARIES ${MODULE_TARGET_LIBRARIES}
  RESOURCES ${MODULE_RESOURCES}
  WITH_GENERIC_TESTS
  )

#-----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(Testing)
endif()
