cmake_minimum_required (VERSION 3.5)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxllo)

if(WIN32)
project_import_package(PRIVATE dlfcn )
endif()

project_target(static_library)

project_dependencies(PUBLIC cxxtricks)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")