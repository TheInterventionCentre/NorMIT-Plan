cmake_minimum_required(VERSION 2.8.9)

project(NorMIT-Plan)

set(NORMIT_PLAN_VERSION "4.6.0")

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/NorMIT-Plan")
set(EXTENSION_CATEGORY "NorMIT-Plan")
set(EXTENSION_CONTRIBUTORS "Rafael Palomar (The Intervention Centre, Oslo University Hospital), Louise Oram (The Intervention Centre, Oslo University Hospital), Rahul Kumar (The Intervention Centre, Oslo University Hospital), Anonymous")
set(EXTENSION_DESCRIPTION "Segmentation, 3D modeling and planning of liver resection procedures.")
set(EXTENSION_ICONURL "")
set(EXTENSION_SCREENSHOTURLS "")
set(EXTENSION_DEPENDS "NA") # Specified as a space separated string, a list or 'NA' if any

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(ResectionPlanning)

## NEXT_MODULE

#-----------------------------------------------------------------------------
# Documentation
find_package(Doxygen QUIET)

if(DOXYGEN_DOT_FOUND)
  mark_as_superbuild(DOXYGEN_EXECUTABLE)

  set(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Where documentation archives should be stored")
  mark_as_advanced(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)
  mark_as_superbuild(DOCUMENTATION_ARCHIVES_OUTPUT_DIRECTORY)

  option(DOCUMENTATION_TARGET_IN_ALL "Include the custom target for building documentation in 'all'" OFF)
  mark_as_advanced(DOCUMENTATION_TARGET_IN_ALL)
  mark_as_superbuild(DOCUMENTATION_TARGET_IN_ALL)
endif()

# Automatically generate documentation at build time
set(all_arg)
if(DOCUMENTATION_TARGET_IN_ALL)
  set(all_arg "ALL")
endif()
add_custom_target(normit-doc ${all_arg})
add_custom_target(normit-doc-tarballs ${all_arg})

add_subdirectory(Doxygen)


#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
