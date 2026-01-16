cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxmmproc)

project_target(static_library)

project_dependency_libs(PUBLIC cxxtricks PUBLIC cxxsci PUBLIC cxxcomm )

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")