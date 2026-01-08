# This is a build utility cmake file for all CMakeLists.txt
# Use include(${LIBCXXBASE_CMAKE_DIR}/build.cmake) to include this file.

# ---- Include ----

include("${LIBCXXBASE_CMAKE_DIR}/internal.cmake")
include("${LIBCXXBASE_CMAKE_DIR}/echo.cmake")

macro(__include_dirs_foreach__ visibility included_dirs)
	target_include_directories(${target_name} ${visibility} ${included_dirs})
	message(STATUS "target '${target_name}' include ${visibility}: \"${included_dirs}\"")
endmacro()

function(include_dirs target_name)
	__modifier_foreach__(__include_dirs_foreach__ ${ARGN})
endfunction()

macro(__include_dirs_in_foreach__ visibility included_dirs)
	target_include_directories(${target_name} ${visibility} "${parent_dir}/${included_dirs}")
	message(STATUS "target '${target_name}' include ${visibility}: \"${parent_dir}/${included_dirs}\"")
endmacro()

function(include_dirs_in target_name parent_dir)
	__modifier_foreach__(__include_dirs_in_foreach__ ${ARGN})
endfunction()

# Include header files from current(sub project) source dir.
function(include_current target_name)
	include_dirs_in(${target_name} ${CMAKE_CURRENT_SOURCE_DIR} ${ARGN})
endfunction()

# include_current() for the current project target.
function(project_include_current)
	include_current(${PROJECT_NAME} ${ARGN})
endfunction()

# Include and public header files from global(main project) source dir.
function(include_global target_name visibility included_dirs)
	include_dirs_in(${target_name}  ${PROJECT_SOURCE_DIR} ${ARGN})
endfunction()

# include_current() for the current project target.
function(project_include_global visibility included_dirs)
	include_global(${PROJECT_NAME} ${ARGN})
endfunction()

# ---- Target ----

# Create a new build target with specified binary type. you can append any source files after type.
# @param binary_type The binary output type, see valid params below, options can be reordered.
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
	string(REPLACE "_" ";" args_list ${binary_type}) # split options, replace '_' with ';' and convert it to a list
	list(APPEND executable_args executable)
	list(APPEND executable_exclude_args exclude executable)
	list(APPEND executable_win32_args win32 executable)
	list(APPEND executable_win32_exclude_args exclude win32 executable)
	list(APPEND executable_macos_args macos executable)
	list(APPEND executable_macos_exclude_args exclude macos executable)
	list(APPEND executable_imported_args imported executable)
	list(APPEND executable_imported_global_args global imported executable)
	list(APPEND executable_alias_args executable alias)
	list(APPEND library_static_args static library)
	list(APPEND library_static_exclude_args exclude static library)
	list(APPEND library_dynamic_args dynamic library)
	list(APPEND library_dynamic_exclude_args exclude dynamic library)
	list(APPEND library_module_args module library)
	list(APPEND library_module_exclude_args exclude module library)
	list(APPEND library_interface_args interface library)
	list(APPEND library_interface_exclude_args exclude interface library)
	list(APPEND library_interface_symbolic_args symbolic interface library)
	list(APPEND library_object_args library object)
	list(APPEND library_imported_args imported library)
	list(APPEND library_imported_global_args global imported library)
	list(APPEND library_alias_args library alias)
	set_equal(result args_list executable_args)
	if(${result})
		add_executable(${target_name})
		add_sources(${target_name} ${ARGN}) # Add source files.
		set(resolved_type executable_args)
	endif()
	set_equal(result args_list executable_exclude_args)
	if(${result})
		add_executable(${target_name} EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type executable_exclude_args)
	endif()
	set_equal(result args_list executable_win32_args)
	if(${result})
		add_executable(${target_name} WIN32)
		add_sources(${target_name} ${ARGN})
		set(resolved_type executable_win32_args)
	endif()
	set_equal(result args_list executable_win32_exclude_args)
	if(${result})
		add_executable(${target_name} WIN32 EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type executable_win32_exclude_args)
	endif()
	set_equal(result args_list executable_macos_args)
	if(${result})
		add_executable(${target_name} MACOSX_BUNDLE)
		add_sources(${target_name} ${ARGN})
		set(resolved_type executable_macos_args)
	endif()
	set_equal(result args_list executable_macos_exclude_args)
	if(${result})
		add_executable(${target_name} MACOSX_BUNDLE EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type executable_macos_exclude_args)
	endif()
	set_equal(result args_list executable_imported_args)
	if(${result})
		if(${ARGC} EQUAL 0)
			add_executable(${target_name} IMPORTED)
		else()
			echo_fatal("imported excutable cannot have args but found '${ARGN}', target '${target_name}' add failed")
		endif()
		set(resolved_type executable_imported_args)
	endif()
	set_equal(result args_list executable_imported_global_args)
	if(${result})
		if(${ARGC} EQUAL 0)
			add_executable(${target_name} IMPORTED GLOBAL)
		else()
			echo_fatal("global imported excutable cannot have args but found '${ARGN}', target '${target_name}' add failed")
		endif()
		set(resolved_type executable_imported_global_args)
	endif()
	set_equal(result args_list executable_alias_args)
	if(${result})
		if(${ARGC} EQUAL 1)
			add_executable(${target_name} ALIAS ${ARGN})
		else()
			echo_fatal("invalid original target '${ARGN}', executable alias target '${target_name}' add failed")
		endif()
		set(resolved_type executable_alias_args)
	endif()
	set_equal(result args_list library_static_args)
	if(${result})
		add_library(${target_name} STATIC)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_static_args)
	endif()
	set_equal(result args_list library_static_exclude_args)
	if(${result})
		add_library(${target_name} STATIC EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_static_exclude_args)
	endif()
	set_equal(result args_list library_dynamic_args)
	if(${result})
		add_library(${target_name} SHARED)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_dynamic_args)
	endif()
	set_equal(result args_list library_dynamic_exclude_args)
	if(${result})
		add_library(${target_name} SHARED EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_dynamic_exclude_args)
	endif()
	set_equal(result args_list library_module_args)
	if(${result})
		add_library(${target_name} MODULE)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_module_args)
	endif()
	set_equal(result args_list library_module_exclude_args)
	if(${result})
		add_library(${target_name} MODULE EXCLUDE_FROM_ALL)
		add_sources(${target_name} ${ARGN})
		set(resolved_type library_module_exclude_args)
	endif()
	set_equal(result args_list library_interface_args)
	if(${result})
		add_library(${target_name} INTERFACE)
		set(resolved_type library_interface_args)
	endif()
	set_equal(result args_list library_interface_exclude_args)
	if(${result})
		add_library(${target_name} INTERFACE EXCLUDE_FROM_ALL)
		set(resolved_type library_interface_exclude_args)
	endif()
	set_equal(result args_list library_interface_symbolic_args)
	if(${result})
		add_library(${target_name} INTERFACE SYMBOLIC)
		set(resolved_type library_interface_symbolic_args)
	endif()
	set_equal(result args_list library_object_args)
	if(${result})
		if(${ARGC} EQUAL 0)
			add_library(${target_name} OBJECT)
		else()
			echo_fatal("object library cannot have args but found '${ARGN}', target '${target_name}' add failed")
		endif()
		set(resolved_type library_object_args)
	endif()
	set_equal(result args_list library_imported_args)
	if(${result})
		if(${ARGC} EQUAL 0)
			add_library(${target_name} IMPORTED)
		else()
			echo_fatal("imported library cannot have args but found '${ARGN}', target '${target_name}' add failed")
		endif()
		set(resolved_type library_imported_args)
	endif()
	set_equal(result args_list library_imported_global_args) 
	if(${result})
		if(${ARGC} EQUAL 0)
			add_library(${target_name} IMPORTED GLOBAL)
		else()
			echo_fatal("global imported library cannot have args but found '${ARGN}', target '${target_name}' add failed")
		endif()
		set(resolved_type library_imported_global_args)
	endif()
	set_equal(result args_list library_alias_args)
	if(${result})
		if(${ARGC} EQUAL 1)
			add_library(${target_name} ALIAS ${ARGN})
		else()
			echo_fatal("invalid original target '${ARGN}', library alias target '${target_name}' add failed")
		endif()
		set(resolved_type library_alias_args)
	endif()
	# print success or invalid args
	if(resolved_type)
		string(JOIN " " descp ${${resolved_type}})
		echo_prompt("add ${descp} target '${target_name}'")
	else()
		echo_fatal("invalid binary type '${binary_type}', target '${target_name}' add failed")
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
	string(JOIN "\n   " src_files ${SOURCES})
	echo_detailed("source files list:\n   " ${src_files})
endmacro()

# Add source files to target.
function(add_sources target_name)
	__modifier_foreach__(__add_sources_foreach__ ${ARGN})
endfunction()

macro(__add_sources_in_foreach__ visibility src)
	file(GLOB SOURCES "${parent_dir}/${src}")
	target_sources(${target_name} ${visibility} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${visibility}: \"${parent_dir}/${src}\"")
	string(JOIN "\n   " src_files ${SOURCES})
	echo_detailed("source files list:\n   " ${src_files})
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

# Find package and include dirs, specify linking libraries.
function(import_package target_name visibility package_name)
	find_package(${package_name} ${ARGN})
	if(${package_name}_FOUND)
		include_dirs(${target_name} ${visibility} ${${package_name}_INCLUDE_DIRS})
		add_dependencies(${target_name} ${visibility} ${${package_name}_LIBRARIES})
		echo_prompt("target '${target_name}' imported package '${package_name}'")
	else()
		echo_fatal("target '${target_name}' import package '${package_name}' failed, package not found")
		echo_error("CMAKE_PREFIX_PATH: \"${CMAKE_PREFIX_PATH}\"")
		echo_error("CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")
	endif()
endfunction()

function(project_import_package visibility package_name)
	import_package(${PROJECT_NAME} ${visibility} ${package_name} ${ARGN})
endfunction()

# ---- Build options ----

function(add_compiler_flags)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARGN}" PARENT_SCOPE)
endfunction()

function(add_linker_flags)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
	set(CMAKE_LIBRARY_LINKER_FLAGS "${CMAKE_LIBRARY_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
endfunction()