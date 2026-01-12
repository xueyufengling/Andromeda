# This is a internal utility and tricks cmake file, mainly used in implementions of other functionalities;
# Use include(${LIBCXXBASE_CMAKE_DIR}/internal.cmake) to include this file.

# Expand the target as text in the context.
# CMake doesn't support mutil-pass macro expand, so the only way is output expanded content into a file, and then include() it.
macro(expand expandable)
	if (NOT COMMAND ${expandable}) # true if this command(callable) is defined
		message(WARNING "'${expandable}' not found, expand failed")
	else()
		set(expand_content_cache "${CMAKE_CACHEFILE_DIR}/CMakeFiles/expandChaches/${expandable}.cmake") # write expand content into cache files
		string(JOIN " " ${expandable}_params ${ARGN}) # ${ARGN} is separated with ';' when writing to file, so join them into a string with separator ' '
		file(WRITE ${expand_content_cache} "${expandable}(${${expandable}_params})")
		unset(${expandable}_params)
		include(${expand_content_cache})
		unset(expand_temp_cache)
	endif()
endmacro()

# Check if all ${ARGN} extra args are in list.
function(in_list result target_list)
	foreach(arg IN LISTS ARGN)
		list(FIND ${target_list} ${arg} i)
		if(${i} EQUAL -1)
			set(${result} FALSE PARENT_SCOPE)
			return()
		endif()
	endforeach()
	set(${result} TRUE PARENT_SCOPE)
endfunction()

# Get directory of a file
function(dir_of dir_path file_path)
	get_filename_component(${dir_path} "${file_path}" DIRECTORY)
	set(${dir_path} ${${dir_path}} PARENT_SCOPE)
endfunction()

set(__modifiers_visibility__ PUBLIC PRIVATE INTERFACE)
set(__modifiers_dependency__ SYSTEM AFTER BEFORE ${__modifiers_visibility__})

# Split args as [modifier11] [modifier12] arg1; [modifier21] arg2
# If a token is not a modifier(not in list modifiers_reserved_list), then it is an arg.
# args of op: first is splited arg and rest are modifiers of it.
# use it:
# macro(__op__ arg)
# message(STATUS "${ARGN} ${arg}")
# endmacro()
# __modifiers_foreach__(__modifiers_visibility__ __op__ ${ARGN})
# where ARGN is like: PRIVATE x.cpp PUBLIC y.cpp INTERFACE z.cpp
function(__modifiers_foreach__ modifiers_reserved_list op)
	set(modifiers)
	foreach(arg IN LISTS ARGN)
		in_list(is_modifier ${modifiers_reserved_list} ${arg})
		if(is_modifier)
			list(APPEND modifiers ${arg})
		else() # this token is not a modifier, then it is an arg
			expand(${op} ${arg} ${modifiers})
			set(modifiers) # clear modifiers for the next arg
		endif()
	endforeach()
endfunction()

# Filter a list and append them to filtered_args. cond is a macro that receives 2 params.
# The first arg indicates this arg should be reserved or not, available values are TRUE and FALSE.
function(filter cond filtered_args)
	foreach(arg IN LISTS ARGN)
		expand(${cond} reserved ${arg})
		if(${reserved})
			list(APPEND ${filtered_args} ${arg})
		endif()
	endforeach()
	set(${filtered_args} ${${filtered_args}} PARENT_SCOPE)
endfunction()

macro(__filter_empty__ reserved arg)
	if(arg STREQUAL "")
		set(reserved FALSE PARENT_SCOPE)
	else()
		set(reserved TRUE PARENT_SCOPE)
	endif()
endmacro()

function(filter_empty filtered_args)
filter(__filter_empty__ ${filtered_args} ${ARGN})
set(${filtered_args} ${${filtered_args}} PARENT_SCOPE)
endfunction()

# Compare if the 2 list has the same elements' values, lists can be reordered.
function(set_equal result list1 list2)
	list(SORT ${list1})
	list(SORT ${list2})
	if(${list1} STREQUAL ${list2})
		set(${result} TRUE PARENT_SCOPE)
	else()
		set(${result} FALSE PARENT_SCOPE)
	endif()
endfunction()

# Get all direct sub directories as a list.
function(subdirs_of dirs_list)
	foreach(parent_dir IN LISTS ARGN)
		file(GLOB current_dirs_list LIST_DIRECTORIES TRUE "${parent_dir}/*")
		list(APPEND ${dirs_list} ${current_dirs_list})
	endforeach()
	set(${dirs_list} ${${dirs_list}} PARENT_SCOPE)
endfunction()

function(first_subdir_of dir_path parent_dir)
	subdirs_of(dirs_list ${parent_dir})
	list(GET dirs_list 0 ${dir_path})
	set(${dir_path} ${${dir_path}} PARENT_SCOPE)
endfunction()