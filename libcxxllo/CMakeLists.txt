cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxllo)

project_target(PUBLIC static_lib)

project_dependencies(PUBLIC cxxtricks)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")