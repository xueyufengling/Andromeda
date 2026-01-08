cmake_minimum_required (VERSION 3.5)

include("./cmake/main.cmake")

setup_environment(windows x86_64)

include("./cmake/build.cmake")

add_compiler_flags("-std=c++11 -Wno-attributes -Wno-deprecated-declarations -Wno-pointer-arith -Wno-stringop-overflow" )

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

project(cxxbase)

add_subdirectory("libcxxtricks")
add_subdirectory("libcxxllo")
add_subdirectory("libcxxsci")
add_subdirectory("libcxxcomm")
add_subdirectory("libcxxmmproc")