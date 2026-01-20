cmake_minimum_required (VERSION 3.10)

include(${PROJECT_SOURCE_DIR}/cmake/build.cmake)

project(cxxnw)

project_target(static_library)

project_import_package_std(PRIVATE boost_asio)

project_dependency_libs(PUBLIC cxxtricks PUBLIC cxxcomm)

# Include libcxxllo header files
project_include_current(PUBLIC "include")

project_sources_current(PRIVATE "src/*.cpp")