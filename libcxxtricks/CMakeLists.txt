cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxtricks)

#libcxxtricks is a header-only library
project_target(interface_library)

# Include libcxxcomm header files
project_include_current(INTERFACE "include")