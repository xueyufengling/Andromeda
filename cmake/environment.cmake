# This is a environment setup cmake file for main project CMakeLists.txt
# Use include("./cmake/environment.cmake") to include this file, do not include this file in subdirectory.

# Setup build environment.
# @param target_platform target platform type, valid type are windows, unix
# @param target_architechture target architecture type, valid type are x86_64, aarch64
function(setup_environment target_platform target_architechture)
	message(STATUS "setup environment in \"${CMAKE_CURRENT_LIST_DIR}\"")
	# Platform
	if(${target_platform} MATCHES windows)
		message(STATUS "target platform: ${target_platform}")
	elseif(${target_platform} MATCHES unix)
		message(STATUS "target platform: ${target_platform}")
	else()
		message(FATAL_ERROR "unknow target platform: ${target_platform}")
	endif()
	# Architechture
	if(${target_architechture} MATCHES x86_64)
		message(STATUS "target architechture: ${target_architechture}")
	elseif(${target_architechture} MATCHES aarch64)
		message(STATUS "target architechture: ${target_architechture}")
	else()
		message(FATAL_ERROR "unknow target architechture: ${target_architechture}")
	endif()
	set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/${target_architechture}-${target_platform}-toolchain.cmake" PARENT_SCOPE)
	message(STATUS "configured toolchain: \"${CMAKE_TOOLCHAIN_FILE}\"")
endfunction()