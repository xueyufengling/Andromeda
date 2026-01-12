cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxllo)

project_target(static_library)

if(WIN32)
project_import_package(PRIVATE dlfcn-win32)
endif()

project_dependency_libs(PUBLIC cxxtricks)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")