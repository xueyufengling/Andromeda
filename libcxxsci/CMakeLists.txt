cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxsci)

project_target(library_interface)

# Include libcxxsci header files
project_include_current(INTERFACE "include")