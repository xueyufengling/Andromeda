# This is a build utility cmake file for all CMakeLists.txt
# Use include(${LIBCXXBASE_CMAKE_DIR}/build.cmake) to include this file.

# ---- Include ----
# WARNING: SOME PACKAGE'S INCLUDE DIR IS SYSTEM INCLUDE DIR, IN THIS CASE INCLUDE IT AGAIN WILL BROKE SYSTEM INCLUDE SEARCH ORDER, AND #include_next CANNOT FIND FILE!

include("${LIBCXXBASE_CMAKE_DIR}/internal.cmake")
include("${LIBCXXBASE_CMAKE_DIR}/echo.cmake")

macro(__include_dirs_foreach__ included_dir)
	string(JOIN " " modifiers_descp ${ARGN})
	target_include_directories(${target_name} ${ARGN} ${included_dir})
	message(STATUS "target '${target_name}' include ${modifiers_descp}: \"${included_dir}\"")
endmacro()

function(include_dirs target_name)
	__modifiers_foreach__(__modifiers_dependency__ __include_dirs_foreach__ ${ARGN})
endfunction()

macro(__include_dirs_in_foreach__ included_dir)
	string(JOIN " " modifiers_descp ${ARGN})
	target_include_directories(${target_name} ${ARGN} "${parent_dir}/${included_dir}")
	message(STATUS "target '${target_name}' include ${modifiers_descp}: \"${parent_dir}/${included_dir}\"")
endmacro()

function(include_dirs_in target_name parent_dir)
	__modifiers_foreach__(__modifiers_dependency__ __include_dirs_in_foreach__ ${ARGN})
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

set(__system_include_dirs__ "${LIBCXXBASE_CXX_SYSTEM_INCLUDE}" "${LIBCXXBASE_C_SYSTEM_INCLUDE}")

function(is_system_include result)
	in_list(${result} __system_include_dirs__ ${ARGN})
	set(${result} ${${result}} PARENT_SCOPE)
endfunction()

function(has_system_include result)
	foreach(arg IN LISTS ARGN)
		is_system_include(${result} arg)
		if(${result})
			set(${result} TRUE PARENT_SCOPE)
			return()
		endif()
	endforeach()
	set(${result} FALSE PARENT_SCOPE)
endfunction()

# include system dirs with -I.
# target_link_libraries() will automatically include its include directories with -isystem, if these directories are system include, then system include search order might be broken,
# which will casue a classical #include_next <stdlib.h> file not found error.
# see https://stackoverflow.com/questions/64344063/cmake-stdlib-include-issue/79866054#79866054
function(system_include target_name)
	target_include_directories(${target_name} SYSTEM PRIVATE "${__system_include_dirs__}")
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
	is_empty(args_empty ${ARGN})
	set(should_add_sources FALSE)
	set_equal(result args_list executable_args)
	if(${result})
		add_executable(${target_name})
		set(should_add_sources TRUE)
		set(resolved_type executable_args)
	endif()
	set_equal(result args_list executable_exclude_args)
	if(${result})
		add_executable(${target_name} EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
		set(resolved_type executable_exclude_args)
	endif()
	set_equal(result args_list executable_win32_args)
	if(${result})
		add_executable(${target_name} WIN32)
		set(should_add_sources TRUE)
		set(resolved_type executable_win32_args)
	endif()
	set_equal(result args_list executable_win32_exclude_args)
	if(${result})
		add_executable(${target_name} WIN32 EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
		set(resolved_type executable_win32_exclude_args)
	endif()
	set_equal(result args_list executable_macos_args)
	if(${result})
		add_executable(${target_name} MACOSX_BUNDLE)
		set(should_add_sources TRUE)
		set(resolved_type executable_macos_args)
	endif()
	set_equal(result args_list executable_macos_exclude_args)
	if(${result})
		add_executable(${target_name} MACOSX_BUNDLE EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
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
		set(should_add_sources TRUE)
		set(resolved_type library_static_args)
	endif()
	set_equal(result args_list library_static_exclude_args)
	if(${result})
		add_library(${target_name} STATIC EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
		set(resolved_type library_static_exclude_args)
	endif()
	set_equal(result args_list library_dynamic_args)
	if(${result})
		add_library(${target_name} SHARED)
		set(should_add_sources TRUE)
		set(resolved_type library_dynamic_args)
	endif()
	set_equal(result args_list library_dynamic_exclude_args)
	if(${result})
		add_library(${target_name} SHARED EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
		set(resolved_type library_dynamic_exclude_args)
	endif()
	set_equal(result args_list library_module_args)
	if(${result})
		add_library(${target_name} MODULE)
		set(should_add_sources TRUE)
		set(resolved_type library_module_args)
	endif()
	set_equal(result args_list library_module_exclude_args)
	if(${result})
		add_library(${target_name} MODULE EXCLUDE_FROM_ALL)
		set(should_add_sources TRUE)
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
		echo_cmd("add ${descp} target '${target_name}'")
	else()
		echo_fatal("invalid binary type '${binary_type}', target '${target_name}' add failed")
	endif()
	# Add sources
	if(should_add_sources AND NOT args_empty)
		add_sources(${target_name} ${ARGN})
	endif()
endfunction()

# Create a new build target for the current project.
function(project_target binary_type)
	add_target(${PROJECT_NAME} ${binary_type})
endfunction()

# ---- Sources ----

# Auto generate a dummy source file for those target without sources, for example link multipe static libraries.
function(target_no_sources target_name)
	set(dummy_source "${CMAKE_BINARY_DIR}/CMakeFiles/${target_name}.dir/__${target_name}_dummy_source__.cpp")
	file(WRITE ${dummy_source} "")
	target_sources(${target_name} PRIVATE ${dummy_source})
endfunction()

macro(__add_sources_foreach__ src)
	file(GLOB SOURCES ${src})
	target_sources(${target_name} ${ARGN} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${ARGN}: \"${src}\"")
	string(JOIN "\n   " src_files ${SOURCES})
	echo_detailed("source files list:\n   " ${src_files})
endmacro()

# Add source files to target.
function(add_sources target_name)
	is_empty(empty ${ARGN})
	if(empty)
		target_no_sources(${target_name})
	else()
		__modifiers_foreach__(__modifiers_visibility__ __add_sources_foreach__ ${ARGN})
	endif()
endfunction()

macro(__add_sources_in_foreach__ src)
	file(GLOB SOURCES "${parent_dir}/${src}")
	target_sources(${target_name} ${ARGN} ${SOURCES})
	message(STATUS "target '${target_name}' sources ${ARGN}: \"${parent_dir}/${src}\"")
	string(JOIN "\n   " src_files ${SOURCES})
	echo_detailed("source files list:\n   " ${src_files})
endmacro()

# Add source files in parent_dir to target.
function(add_sources_in target_name parent_dir)
	is_empty(empty ${ARGN})
	if(empty)
		target_no_sources(${target_name})
	else()
		__modifiers_foreach__(__modifiers_visibility__ __add_sources_in_foreach__ ${ARGN})
	endif()
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

macro(__add_dependency_libs_foreach__ lib)
	target_link_libraries(${target_name} ${ARGN} ${lib})
	message(STATUS "target '${target_name}' dependency ${ARGN}: ${lib}")
endmacro()

# Add link library for the target.
function(add_dependency_libs target_name)
	__modifiers_foreach__(__modifiers_dependency__ __add_dependency_libs_foreach__ ${ARGN})
endfunction()

# Add link library for the current project target.
function(project_dependency_libs)
	add_dependency_libs(${PROJECT_NAME} ${ARGN})
endfunction()

# Find package and include dirs if it's not a system incldue, specify linking libraries.
# Must be called after target has been created.
function(import_package target_name visibility package_name package_prefix include_suffix lib_suffix found_suffix)
	find_package(${package_name} ${ARGN})
	if(NOT package_prefix)
		set(package_prefix ${package_name}) # if no prefix specified, then use its name as prefix
	endif()
	if(NOT found_suffix)
		set(found_suffix _FOUND)
	endif()
	set(found_name ${package_prefix}${found_suffix})
	if(NOT include_suffix)
		set(include_suffix _INCLUDE_DIRS)
	endif()
	set(include_name ${package_prefix}${include_suffix})
	if(NOT lib_suffix)
		set(lib_suffix _LIBRARIES)
	endif()
	set(lib_name ${package_prefix}${lib_suffix})
	if(${found_name})
		set(include_dirs ${${include_name}})
		foreach(inc IN LISTS include_dirs)
			is_system_include(system_include ${inc})
			if(system_include)
				echo_prompt("package '${package_name}' has system include, ignored: \"${inc}\"")
			else()
				include_dirs(${target_name} SYSTEM ${visibility} ${inc})
			endif()
		endforeach()
		system_include(${target_name})
		add_dependency_libs(${target_name} ${visibility} ${${lib_name}})
		echo_prompt("target '${target_name}' imported package '${package_name}'")
	else()
		echo_fatal("target '${target_name}' import package '${package_name}' failed")
		echo_error("CMAKE_PREFIX_PATH: \"${CMAKE_PREFIX_PATH}\"")
		echo_error("CMAKE_INSTALL_PREFIX: \"${CMAKE_INSTALL_PREFIX}\"")
		echo_error("request found ${found_name}: \"${${found_name}}\"")
		echo_error("request include ${include_name}: \"${${include_name}}\"")
		echo_error("request library ${lib_name}: \"${${lib_name}}\"")
	endif()
endfunction()

function(project_import_package visibility package_name)
	import_package(${PROJECT_NAME} ${visibility} ${package_name} ${ARGN})
endfunction()

# Use default suffix.
function(import_package_std target_name visibility package_name)
	import_package(${target_name} ${visibility} ${package_name} "" "" "" "" ${ARGN})
endfunction()

function(project_import_package_std visibility package_name)
	import_package_std(${PROJECT_NAME} ${visibility} ${package_name} ${ARGN})
endfunction()

# Set find_package() search path.
function(add_package_search_path)
	set(search_paths ${CMAKE_PREFIX_PATH})
	list(APPEND search_paths ${ARGN})
	set(CMAKE_PREFIX_PATH "${search_paths}" PARENT_SCOPE)
endfunction()

# ---- Build options ----

# Append compiler flags at the end
function(append_compiler_flags lang_name)
	string(JOIN " " compiler_flags ${ARGN})
	string(TOUPPER ${lang_name} lang_name_upper)
	set(CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS} ${compiler_flags}" PARENT_SCOPE)
endfunction()

# Append compiler flags at the begining
function(prepend_compiler_flags lang_name)
	string(JOIN " " compiler_flags ${ARGN})
	string(TOUPPER ${lang_name} lang_name_upper)
	set(CMAKE_${lang_name_upper}_FLAGS "${compiler_flags} ${CMAKE_${lang_name_upper}_FLAGS}" PARENT_SCOPE)
endfunction()

# Remove compiler flags
function(remove_compiler_flags lang_name)
	string(TOUPPER ${lang_name} lang_name_upper)
	foreach(flag IN LISTS ARGN)
		string(REPLACE ${flag} "" CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS}")
	endforeach()
	set(CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS}" PARENT_SCOPE)
endfunction()

# Cancel all -isystem system include dirs. This function is to prevent compiler from system include order problems.
# When system include path order is not correct, #include_next will cause a fatal error: xxx: No such file or directory.
# For example, if C version stdlib.h was included before C++ version stdlib.h, then no stdlib.h can be found after C++ version.
# In MSYS2, C version is located at "msys64/ucrt64/include", while C++ version is located at "msys64/ucrt64/include/c++/15.2.0"
# Add  g++ flag -v to observe if the C++ version is included BEFORE C version. If not, call this function and specify all system include dirs with -I in correct order.
# Correct system include serach order for C++:
# msys64/ucrt64/include/c++/15.2.0
# msys64/ucrt64/include/c++/15.2.0/x86_64-w64-mingw32
# msys64/ucrt64/include/c++/15.2.0/backward
# msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/15.2.0/include
# msys64/ucrt64/lib/gcc/x86_64-w64-mingw32/15.2.0/include-fixed
# msys64/ucrt64/include
function(include_system_dirs lang_name should_include)
	string(TOUPPER ${lang_name} lang_name_upper)
	if(should_include)
		remove_compiler_flags(${lang_name} "-nostdinc")
		echo_prompt("enable ${lang_name} system include dirs")
	else()
		prepend_compiler_flags(${lang_name} "-nostdinc")
		echo_prompt("disable ${lang_name} system include dirs")
	endif()
	set(CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS}" PARENT_SCOPE)
endfunction()

# Apply compiler flag for each arg and prepend to the begining.
function(apply_prepend_compiler_flags lang_name flag)
	string(TOUPPER ${lang_name} lang_name_upper)
	foreach(arg IN LISTS ARGN)
		message(STATUS "apply compiler flag for ${lang_name}: ${flag} ${arg}")
	endforeach()
	list(REVERSE ARGN) # reverse list to correct flags order
	foreach(arg IN LISTS ARGN)
		prepend_compiler_flags(${lang_name} "${flag} ${arg}")
	endforeach()
	set(CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS}" PARENT_SCOPE)
endfunction()

# Specify include search dir AFTER -isystem system include dirs.
function(post_system_include_dirs lang_name)
	apply_prepend_compiler_flags(${lang_name} "-idirafter" ${ARGN})
	set(CMAKE_${lang_name_upper}_FLAGS "${CMAKE_${lang_name_upper}_FLAGS}" PARENT_SCOPE)
endfunction()

function(add_linker_flags)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
	set(CMAKE_LIBRARY_LINKER_FLAGS "${CMAKE_LIBRARY_LINKER_FLAGS} ${ARGN}" PARENT_SCOPE)
endfunction()

function(set_output_dir static_lib_dir shared_lib_dir exec_dir)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${static_lib_dir}" PARENT_SCOPE)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${shared_lib_dir}" PARENT_SCOPE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${exec_dir}" PARENT_SCOPE)
endfunction()