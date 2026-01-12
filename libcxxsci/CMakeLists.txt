cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxsci)

project_target(interface_library)

# Include libcxxsci header files
project_include_current(INTERFACE "include")