cmake_minimum_required(VERSION 2.8.9)

project(NorMIT-Plan)

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
add_subdirectory(VesselSegmentation)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
