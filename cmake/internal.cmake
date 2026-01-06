# This is a internal utility and tricks cmake file, mainly used in implementions of other functionalities;
# Use include(${PROJECT_SOURCE_DIR}/cmake/internal.cmake) to include this file.

# Expand the target as text in the context.
# CMake doesn't support mutil-pass macro expand, so the only way is output expanded content into a file, and then include() it.
macro(expand expandable)
	if (NOT COMMAND ${expandable}) # true if this command(callable) is defined
		message(WARNING "'${expandable}' not found, expand failed")
	else()
		set(expand_content_cache "${CMAKE_CACHEFILE_DIR}/CMakeFiles/expandChaches/${expandable}.cmake") # Write expand content into cache files
		string(JOIN " " ${expandable}_params ${ARGN}) # ${ARGN} is separated with ';' when writing to file, so join them into a string with separator ' '
		file(WRITE ${expand_content_cache} "${expandable}(${${expandable}_params})")
		unset(${expandable}_params)
		include(${expand_content_cache})
		unset(expand_temp_cache)
	endif()
endmacro()

# foreach for elements with modifier.
# op is a 2-param callable to process each element and its modifier.
# use it:
# macro(__op__ modifier arg)
# message(STATUS arg)
# endmacro()
# __modifier_foreach__(op ${ARGN})
# where ARGN is like: PRIVATE x.cpp PUBLIC y.cpp INTERFACE z.cpp
function(__modifier_foreach__ op)
	foreach(arg IN LISTS ARGN)
		if(${arg} MATCHES PRIVATE)
			set(modifier PRIVATE)
		elseif(${arg} MATCHES PUBLIC)
			set(modifier PUBLIC)
		elseif(${arg} MATCHES INTERFACE)
			set(modifier INTERFACE)
		else()
			expand(${op} ${modifier} ${arg})
		endif()
	endforeach()
endfunction()