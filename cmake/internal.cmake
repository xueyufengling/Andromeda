# This is a internal utility and tricks cmake file, mainly used in implementions of other functionalities;
# use include(${PROJECT_SOURCE_DIR}/cmake/internal.cmake) to include this file.

# Execute command
function(execute)
execute_process(COMMAND ${ARGN})
endfunction()

# Execute cmake command
function(cmake)
execute(${CMAKE_COMMAND} ${ARGN})
endfunction()

# Expand the target as text in the context.
# CMake doesn't support mutil-pass macro expand, so the only way is output expanded content into a file, and then include() it.
macro(expand expandable)
	if (NOT COMMAND ${expandable}) # true if this command(callable) is defined
		message(WARNING "'${expandable}' not found, expand failed")
	else()
		set(expand_temp_cache "${CMAKE_CACHEFILE_DIR}/CMakeFiles/expandChaches/${expandable}.cmake") # Write expand content into cache files
		string(JOIN " " ${expandable}_params ${ARGN}) # ${ARGN} is separated with ';' when writing to file, so join them into a string with separator ' '
		file(WRITE ${expand_temp_cache} "${expandable}(${${expandable}_params})")
		unset(${expandable}_params)
		include(${expand_temp_cache})
		unset(expand_temp_cache)
	endif()
endmacro()

# foreach for elements with visibility.
# op is a 2-param callable to process each element and its visibility.
# use it:
# macro(__op__ visibility arg)
# message(STATUS arg)
# endmacro()
# __visibility_foreach__(op ${ARGN})
# where ARGN is like: PRIVATE x.cpp PUBLIC y.cpp
function(__visibility_foreach__ op)
	foreach(arg IN LISTS ARGN)
		if(${arg} MATCHES PRIVATE)
			set(visibility PRIVATE)
		elseif(${arg} MATCHES PUBLIC)
			set(visibility PUBLIC)
		else()
			expand(${op} ${visibility} ${arg})
		endif()
	endforeach()
endfunction()