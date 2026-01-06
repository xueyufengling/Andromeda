cmake_minimum_required (VERSION 3.5)

include("./cmake/environment.cmake")
include("./cmake/build.cmake")

setup_environment(windows x86_64)

add_compiler_flags("-std=c++11 -Wno-attributes -Wno-deprecated-declarations -Wno-pointer-arith -Wno-stringop-overflow" )

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

project(cxxbase)

add_subdirectory("libcxxtricks")
add_subdirectory("libcxxllo")
add_subdirectory("libcxxsci")
add_subdirectory("libcxxcomm")
