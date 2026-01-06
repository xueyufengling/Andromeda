cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxsci)

project_target(PUBLIC interface_lib)

# Include libcxxsci header files
project_include_current(PUBLIC "include")