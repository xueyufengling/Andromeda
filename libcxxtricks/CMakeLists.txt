cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxtricks)

#libcxxtricks is a header-only library
project_target(PUBLIC interface_lib)

# Include libcxxcomm header files
project_include_current(PUBLIC "include")