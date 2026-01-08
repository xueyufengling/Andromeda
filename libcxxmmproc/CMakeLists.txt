cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(libcxxmmproc)

project_target(static_library)

project_dependencies(PUBLIC cxxtricks)
project_dependencies(PUBLIC cxxcomm)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")