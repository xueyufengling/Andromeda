# This is a toolchain setup cmake file for main project CMakeLists.txt
# This file MUST be included BEFORE project() of main project.
# All other .cmake files in cmake dir MUST be include() AFTER setup_toolchain().
# Use include("./cmake/main.cmake") to include this file, do not include this file in subdirectory.

# cmake dir for the main project.
# Note that ${CMAKE_CURRENT_LIST_DIR} in function or macro, the path is caller's ${CMAKE_CURRENT_LIST_DIR}.
# When A.cmake include(B.cmake), the ${CMAKE_CURRENT_LIST_DIR} defined in B.cmake is still the dir of B, but not A.

set(LIBCXXBASE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE STRING "libcxxbase cmake directory") # Set cmake build scripts dir in GLOBAL scope.
message(STATUS "set LIBCXXBASE_CMAKE_DIR to \"${LIBCXXBASE_CMAKE_DIR}\"")

include("${LIBCXXBASE_CMAKE_DIR}/echo.cmake")

# Setup build toolchain.
# @param target_platform target platform type, valid type are windows, unix
# @param target_architechture target architecture type, valid type are x86_64, aarch64
function(setup_toolchain target_platform target_architechture)
	echo_important("setup toolchain")
	# Platform
	if(${target_platform} MATCHES windows)
		
	elseif(${target_platform} MATCHES unix)
		
	else()
		message(FATAL_ERROR "unknow target platform: ${target_platform}")
	endif()
	echo_important("target platform: ${target_platform}")
	# Architechture
	if(${target_architechture} MATCHES x86_64)
		
	elseif(${target_architechture} MATCHES aarch64)
		
	else()
		message(FATAL_ERROR "unknow target architechture: ${target_architechture}")
	endif()
	echo_important("target architechture: ${target_architechture}")
	set(CMAKE_TOOLCHAIN_FILE "${LIBCXXBASE_CMAKE_DIR}/toolchain/${target_architechture}-${target_platform}-toolchain.cmake" PARENT_SCOPE)
	echo_important("configured toolchain: \"${CMAKE_TOOLCHAIN_FILE}\"")
endfunction()