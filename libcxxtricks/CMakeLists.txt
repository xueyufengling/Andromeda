cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxtricks)

#libcxxtricks is a header-only library
project_target(interface_library)

project_import_package_std(INTERFACE ppmp)
project_import_package_std(INTERFACE tplmp)

# Include libcxxcomm header files
project_include_current(INTERFACE "include")