cmake_minimum_required (VERSION 3.10)

include("./cmake/main.cmake")
include("./cmake/build.cmake")

setup_toolchain(win32 x86_64)

add_package_search_path("C:/msys64/ucrt64/share")

append_compiler_flags(cxx "-v -std=c++11")

echo_prompt("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")

add_linker_flags("-static")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

project(cxxbase)

apply_toolchain()

add_subdirectory("libcxxtricks")
add_subdirectory("libcxxllo")
#add_subdirectory("libcxxsci")
#add_subdirectory("libcxxcomm")
#add_subdirectory("libcxxmmproc")
