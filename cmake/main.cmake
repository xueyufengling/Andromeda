# This is a toolchain setup cmake file for main project CMakeLists.txt
# This file MUST be included BEFORE project() of main project.
# All other .cmake files in cmake dir MUST be include() AFTER setup_toolchain().
# Use include("./cmake/main.cmake") to include this file, do not include this file in subdirectory.

# cmake dir for the main project.
# Note that ${CMAKE_CURRENT_LIST_DIR} in function or macro, the path is caller's ${CMAKE_CURRENT_LIST_DIR}.
# When A.cmake include(B.cmake), the ${CMAKE_CURRENT_LIST_DIR} defined in B.cmake is still the dir of B, but not A.

set(LIBCXXBASE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "libcxxbase cmake directory" FORCE) # Set cmake build scripts dir in GLOBAL scope.
message(STATUS "set LIBCXXBASE_CMAKE_DIR to \"${LIBCXXBASE_CMAKE_DIR}\"")

include("${LIBCXXBASE_CMAKE_DIR}/echo.cmake")
include("${LIBCXXBASE_CMAKE_DIR}/internal.cmake")

# Setup build toolchain.
# @param target_platform target platform type, valid type are win32, posix
# @param target_architechture target architecture type, valid type are x86_64, aarch64
function(setup_toolchain target_platform target_architechture)
	set(CMAKE_C_COMPILER_FORCED TRUE PARENT_SCOPE) # force to avoid compiler test failed
	set(CMAKE_CXX_COMPILER_FORCED TRUE PARENT_SCOPE)
	# Platform
	if(${target_platform} MATCHES win32)
		
	elseif(${target_platform} MATCHES posix)
		
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
	set(CMAKE_TOOLCHAIN_FILE "${LIBCXXBASE_CMAKE_DIR}/toolchain/${target_architechture}-${target_platform}-toolchain.cmake" CACHE PATH "path to the toolchain file" FORCE)
	message(STATUS "configured toolchain: \"${CMAKE_TOOLCHAIN_FILE}\"")
endfunction()

# Cache toolchain variables, or run "cmake --build ." will report no such a file or directory, indicates that all these toolchain programs' paths are empty.
# Must be called AFTER project(), because variables set in toolchain is applied after project().
function(apply_toolchain)
	set(CMAKE_ASM_COMPILER ${CMAKE_ASM_COMPILER} CACHE PATH "assembler path" FORCE)
	message(STATUS "toolchain assembler path: \"${CMAKE_ASM_COMPILER}\"")
	set(CMAKE_C_COMPILER ${CMAKE_C_COMPILER} CACHE PATH "c compiler path" FORCE)
	message(STATUS "toolchain c compiler path: \"${CMAKE_C_COMPILER}\"")
	set(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE PATH "c++ compiler path" FORCE)
	message(STATUS "toolchain c++ compiler path: \"${CMAKE_CXX_COMPILER}\"")
	set(CMAKE_LINKER ${CMAKE_LINKER} CACHE PATH "linker path" FORCE)
	message(STATUS "toolchain linker path: \"${CMAKE_LINKER}\"")
	set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM} CACHE PATH "make path" FORCE)
	message(STATUS "toolchain make path: \"${CMAKE_MAKE_PROGRAM}\"")
	dir_of(env_dir ${CMAKE_CXX_COMPILER}) # get bin path of c++ compiler
	get_filename_component(env_dir "${env_dir}/.." ABSOLUTE)
	set(LIBCXXBASE_TOOLCHAIN_ENV_DIR "${env_dir}" CACHE PATH "toolchain programs environment path" FORCE)
	file(GLOB_RECURSE stdlib_paths "${LIBCXXBASE_TOOLCHAIN_ENV_DIR}/stdlib.h") # find c++ and c system include dir in LIBCXXBASE_TOOLCHAIN_ENV_DIR
	# ${stdlib_paths} is like this: C:/msys64/ucrt64/include/c++/15.2.0/stdlib.h;C:/msys64/ucrt64/include/c++/15.2.0/tr1/stdlib.h;C:/msys64/ucrt64/include/stdlib.h
	list(GET stdlib_paths 0 cxx_stdlib_path) # the first is always c++ include
	dir_of(cxx_system_include_path ${cxx_stdlib_path})
	set(LIBCXXBASE_CXX_SYSTEM_INCLUDE "${cxx_system_include_path}" CACHE PATH "c++ system include dir" FORCE)
	list(GET stdlib_paths -1 c_stdlib_path) # the last is always c include
	dir_of(c_system_include_path ${c_stdlib_path})
	set(LIBCXXBASE_C_SYSTEM_INCLUDE "${c_system_include_path}" CACHE PATH "c system include dir" FORCE)
endfunction()