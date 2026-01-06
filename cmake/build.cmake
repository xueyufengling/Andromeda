# This is a build utility cmake file for CMakeLists.txt
# Use include(${PROJECT_SOURCE_DIR}/cmake/build.cmake) to include this file.

# ---- Include ----

include("${CMAKE_CURRENT_LIST_DIR}/internal.cmake")

macro(__include_dir_foreach__ visibility included_dir)
	target_include_directories(${target_name} ${visibility} ${included_dir})
	message(STATUS "target '${target_name}' include ${visibility}: \"${included_dir}\"")
endmacro()

function(include_dir target_name)
	__modifier_foreach__(__include_dir_foreach__ ${ARGN})
endfunction()

macro(__include_dir_in_foreach__ visibility included_dir)
	target_include_directories(${target_name} ${visibility} "${parent_dir}/${included_dir}")
	message(STATUS "target '${target_name}' include ${visibility}: \"${parent_dir}/${included_dir}\"")
endmacro()

function(include_dir_in target_name parent_dir)
	__modifier_foreach__(__include_dir_in_foreach__ ${ARGN})
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

# Create a new build target with specified binary type. you can append any source files after type.
# @param binary_type The binary output type, see valid params below.
# executable					Executable file.
# executable_exclude			Exclude from all executable file.
# executable_win32				Win32 GUI ecutable file.
# executable_win32_exclude		Exclude from all Win32 GUI ecutable file.
# executable_macos				MacOS app bundle.
# executable_macos_exclude		Exclude from all MacOS app bundle.
# executable_imported			Imported executable file.
# executable_imported_global	Global imported executable file.
# executable_alias				Alias for executable, need an extra parameter as original target.
# library_static				Static library.
# library_static_exclude		Exclude from all static library.
# library_dynamic				Dynamic library.
# library_dynamic_exclude		Exclude from all dynamic library.
# library_module				Module library.
# library_module_exclude		Exclude from all module library.
# library_interface				No source file, interface library.
# library_interface_exclude		Exclude from all  interface library.
# library_interface_symbolic	Symbolic interface library.
# library_object				Compile sources to .o files.
# library_imported				Imported library.
# library_imported_global		Global imported library.
# library_alias					Alias for library, need an extra parameter as original target.
function(add_target target_name binary_type)
	if(${binary_type} MATCHES executable)
		add_executable(${target_name})
		message(STATUS "add executable target '${target_name}'")
		add_sources(${target_name} ${ARGN}) # Add source files.
	elseif(${binary_type} MATCHES executable_exclude)
		add_executable(${target_name} EXCLUDE_FROM_ALL)
		message(STATUS "add exclude executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_win32)
		add_executable(${target_name} WIN32)
		message(STATUS "add win32 executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_win32_exclude)
		add_executable(${target_name} WIN32 EXCLUDE_FROM_ALL)
		message(STATUS "add exclude win32 executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_macos)
		add_executable(${target_name} MACOSX_BUNDLE)
		message(STATUS "add macos executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_macos_exclude)
		add_executable(${target_name} MACOSX_BUNDLE EXCLUDE_FROM_ALL)
		message(STATUS "add exclude macos executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_imported)
		add_executable(${target_name} IMPORTED)
		message(STATUS "add imported executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_imported_global)
		add_executable(${target_name} IMPORTED GLOBAL)
		message(STATUS "add global imported executable target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES executable_alias)
		if(${ARGC} EQUAL 1)
			add_executable(${target_name} ALIAS ${ARGN})
			message(STATUS "add executable alias target '${target_name}' for target '${ARGN}'")
		else()
			message(FATAL_ERROR "invalid arg of original target '${ARGN}', executable alias target '${target_name}' add failed")
		endif()
	elseif(${binary_type} MATCHES library_static)
		add_library(${target_name} STATIC)
		message(STATUS "add static library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_static_exclude)
		add_library(${target_name} STATIC EXCLUDE_FROM_ALL)
		message(STATUS "add exclude static library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_dynamic)
		add_library(${target_name} SHARED)
		message(STATUS "add shared library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_dynamic_exclude)
		add_library(${target_name} SHARED EXCLUDE_FROM_ALL)
		message(STATUS "add exclude shared library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_module)
		add_library(${target_name} MODULE)
		message(STATUS "add module library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_module_exclude)
		add_library(${target_name} MODULE EXCLUDE_FROM_ALL)
		message(STATUS "add exclude module library target '${target_name}'")
		add_sources(${target_name} ${ARGN})
	elseif(${binary_type} MATCHES library_interface)
		add_library(${target_name} INTERFACE)
		message(STATUS "add interface library target '${target_name}'")
	elseif(${binary_type} MATCHES library_interface_exclude)
		add_library(${target_name} INTERFACE EXCLUDE_FROM_ALL)
		message(STATUS "add exclude interface library target '${target_name}'")
	elseif(${binary_type} MATCHES library_interface_symbolic)
		add_library(${target_name} INTERFACE SYMBOLIC)
		message(STATUS "add symbolic interface library target '${target_name}'")
	elseif(${binary_type} MATCHES library_object)
		add_library(${target_name} OBJECT)
		message(STATUS "add object library target '${target_name}'")
	elseif(${binary_type} MATCHES library_imported)
		add_library(${target_name} IMPORTED)
		message(STATUS "add imported library target '${target_name}'")
	elseif(${binary_type} MATCHES library_imported_global)
		add_library(${target_name} IMPORTED GLOBAL)
		message(STATUS "add global imported library target '${target_name}'")
	elseif(${binary_type} MATCHES library_alias)
		if(${ARGC} EQUAL 1)
			add_library(${target_name} ALIAS ${ARGN})
			message(STATUS "add library alias target '${target_name}' for target '${ARGN}'")
		else()
			message(FATAL_ERROR "invalid arg of original target '${ARGN}', library alias target '${target_name}' add failed")
		endif()
	else()
		message(FATAL_ERROR "invalid binary type '${binary_type}', target '${target_name}' add failed")
	endif()
endfunction()

# Create a new build target for the current project.
function(project_target binary_type)
	add_target(${PROJECT_NAME} ${binary_type})
endfunction()

# ---- Sources ----

macro(__add_sources_foreach__ visibility src)
	file(GLOB SOURCES ${src})
	target_sources(${target_name} ${visibility} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${visibility}: \"${src}\"")
	string(JOIN "\n\t" src_files ${SOURCES})
	message(STATUS "source files list:\n\t${src_files}")
endmacro()

# Add source files to target.
function(add_sources target_name)
	__modifier_foreach__(__add_sources_foreach__ ${ARGN})
endfunction()

macro(__add_sources_in_foreach__ visibility src)
	file(GLOB SOURCES "${parent_dir}/${src}")
	target_sources(${target_name} ${visibility} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${visibility}: \"${parent_dir}/${src}\"")
	string(JOIN "\n\t" src_files ${SOURCES})
	message(STATUS "source files list:\n\t${src_files}")
endmacro()

# Add source files in parent_dir to target.
function(add_sources_in target_name parent_dir)
	__modifier_foreach__(__add_sources_in_foreach__ ${ARGN})
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
	__modifier_foreach__(__add_dependencies_foreach__ ${ARGN})
endfunction()

# Add link library for the current project target.
function(project_dependencies)
	add_dependencies(${PROJECT_NAME} ${ARGN})
endfunction()

# ---- Build options ----

function(add_compiler_flags)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARGN}" PARENT_SCOPE)
endfunction()

function(add_linker_flags)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
	set(CMAKE_LIBRARY_LINKER_FLAGS "${CMAKE_LIBRARY_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
endfunction()