# This is a build utility cmake file for CMakeLists.txt
# use include(${PROJECT_SOURCE_DIR}/cmake/build.cmake) to include this file.

# ---- Include ----

include(${PROJECT_SOURCE_DIR}/cmake/internal.cmake)

macro(__include_dir_foreach__ visibility included_dir)
	target_include_directories(${target_name} ${visibility} ${included_dir})
	message(STATUS "target '${target_name}' include ${visibility}: \"${included_dir}\"")
endmacro()

function(include_dir target_name)
	__visibility_foreach__(__include_dir_foreach__ ${ARGN})
endfunction()

macro(__include_dir_in_foreach__ visibility included_dir)
	target_include_directories(${target_name} ${visibility} "${parent_dir}/${included_dir}")
	message(STATUS "target '${target_name}' include ${visibility}: \"${parent_dir}/${included_dir}\"")
endmacro()

function(include_dir_in target_name parent_dir)
	__visibility_foreach__(__include_dir_in_foreach__ ${ARGN})
endfunction()

# Include header files from current(sub project) source dir.
function(include_current target_name)
	include_dir_in(${target_name} ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN})
endfunction()

# include_current() for the current project target.
function(project_include_current)
	include_current(${PROJECT_NAME} ${ARGN})
endfunction()

# Include and public header files from global(main project) source dir.
function(include_global target_name visibility included_dir)
	include_dir_in(${target_name}  ${PROJECT_SOURCE_DIR} ${ARGN})
endfunction()

# include_current() for the current project target.
function(project_include_global visibility included_dir)
	include_global(${PROJECT_NAME} ${ARGN})
endfunction()

# ---- Target ----

# Create a new build target with specified type and visibility. you can append any source files after visibility.
# @param binary_type The binary output type, see valid params below.
# interface_lib		No source file, interface library.
# exec				Executable file.
# exec_win32		Win32 GUI ecutable file.
# exec_mac			MacOS app bundle.
# dynamic_lib		Dynamic library.
# static_lib		Static library.
function(add_target target_name visibility binary_type)
	if(${binary_type} MATCHES exec)
		add_executable(${target_name} ${visibility})
		message(STATUS "add executable target '${target_name}'")
	elseif(${binary_type} MATCHES exec_win32)
		add_executable(${target_name} ${visibility})
		message(STATUS "add win32 executable target '${target_name}'")
	elseif(${binary_type} MATCHES exec_mac)
		add_executable(${target_name} ${visibility})
		message(STATUS "add mac executable target '${target_name}'")
	elseif(${binary_type} MATCHES dynamic_lib)
		add_library(${target_name} ${visibility} SHARED)
		message(STATUS "add shared library target '${target_name}'")
	elseif(${binary_type} MATCHES static_lib)
		add_library(${target_name} ${visibility} STATIC)
		message(STATUS "add static library target '${target_name}'")
	elseif(${binary_type} MATCHES interface_lib)
		add_library(${target_name} ${visibility} INTERFACE)
		message(STATUS "add interface library target '${target_name}'")
	else()
		message(FATAL_ERROR "invalid binary type '${binary_type}', target '${target_name}' add failed")
	endif()
endfunction()

# Create a new build target for the current project.
function(project_target visibility binary_type)
	add_target(${PROJECT_NAME} ${visibility} ${binary_type})
endfunction()

# ---- Sources ----

macro(__add_sources_foreach__ visibility src)
	file(GLOB SOURCES ${src})
	target_sources(${target_name} ${visibility} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${visibility}: \"${src}\"")
endmacro()

# Add source files to target.
function(add_sources target_name)
	__visibility_foreach__(__add_sources_foreach__ ${ARGN})
endfunction()

macro(__add_sources_in_foreach__ visibility src)
	file(GLOB SOURCES ${src})
	target_sources(${target_name} ${visibility} "${parent_dir}/${SOURCES}")
	message(STATUS "target '${target_name}' sources ${visibility}: \"${parent_dir}/${src}\"")
endmacro()

# Add source files in parent_dir to target.
function(add_sources_in target_name parent_dir)
	__visibility_foreach__(__add_sources_in_foreach__ ${ARGN})
endfunction()

function(add_sources_current target_name)
	add_sources_in(${target_name} ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN})
endfunction()

function(project_sources_current) 
	add_sources_current(${PROJECT_NAME} ${ARGN})
endfunction()

function(add_sources_global target_name visibility src)
	add_sources_in(${target_name}  ${PROJECT_SOURCE_DIR} ${ARGN})
endfunction()

function(project_sources_global)
	add_sources_global(${PROJECT_NAME} ${ARGN})
endfunction()

# ---- Dependencies ----

macro(__add_dependencies_foreach__ visibility lib)
	target_link_libraries(${target_name} ${visibility} ${lib})
	message(STATUS "target '${target_name}' dependencies ${visibility}: ${lib}")
endmacro()

# Add link library for the target.
function(add_dependencies target_name)
	__visibility_foreach__(__add_dependencies_foreach__ ${ARGN})
endfunction()

# Add link library for the current project target.
function(project_dependencies)
	add_dependencies(${PROJECT_NAME} ${ARGN})
endfunction()