cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxsci)

project_target(static_library)

project_dependency_libs(PUBLIC cxxtricks)

# Include libcxxsci header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")