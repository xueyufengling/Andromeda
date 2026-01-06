cmake_minimum_required (VERSION 3.5)

# Build shared lib when set to true, or static lib when set to false.
set(BUILD_TARGET_TYPE static_lib)
# Set target platform type, valid type are Windows, Unix.
set(BUILD_TARGET_PLATFORM Windows)
# Set target architecture type, valid type are x86_64, aarch64.
set(BUILD_TARGET_ARCHITECTURE x86_64)

if(BUILD_TARGET_PLATFORM MATCHES "Windows")
	if(BUILD_TARGET_ARCHITECTURE MATCHES "x86_64")
		message(STATUS "Building target x86_64 Windows")
		set(CMAKE_TOOLCHAIN_FILE "./cmake/x86_64-windows-toolchain.cmake")
	elseif(BUILD_TARGET_ARCHITECTURE MATCHES "aarch64")
		message(STATUS "Building target aarch64 Windows")
	else()
		message(STATUS "Unknown target architechture")
	endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Wno-deprecated-declarations -Wno-pointer-arith -Wno-stringop-overflow" )
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

project(cxxbase)

add_subdirectory("libcxxtricks")
add_subdirectory("libcxxllo")
add_subdirectory("libcxxsci")
add_subdirectory("libcxxcomm")
