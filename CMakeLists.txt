cmake_minimum_required (VERSION 3.10)

include("./cmake/main.cmake")
include("./cmake/build.cmake")

setup_toolchain(win32 x86_64)
set_output_dir(liba libso bin)

add_package_search_path("C:/msys64/ucrt64/share" "C:/msys64/ucrt64/lib/cmake")

append_compiler_flags(cxx "-std=c++11")

echo_prompt("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")

add_linker_flags("-static")

project(cxxbase)

apply_toolchain()

add_subdirectory("libcxxtricks")
add_subdirectory("libcxxllo")
add_subdirectory("libcxxsci")
add_subdirectory("libcxxcomm")
add_subdirectory("libcxxmmproc")
add_subdirectory("libcxxactx")
add_subdirectory("libcxxgctx")

# Build all targets as a dynamic lib
project_target(dynamic_library)

project_sources_current()

project_dependency_libs(
    PUBLIC cxxtricks
    PUBLIC cxxllo
    PUBLIC cxxsci
    PUBLIC cxxcomm
    PUBLIC cxxmmproc
    PUBLIC cxxactx
    PUBLIC cxxgctx
)