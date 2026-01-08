cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxgctx)

project_target(static_library)

project_dependencies(PUBLIC cxxtricks PUBLIC cxxllo PUBLIC cxxcomm PUBLIC cxxmmproc)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")