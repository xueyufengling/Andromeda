# This is a internal exec function cmake file, provide toolchain call entries;
# Use include(${LIBCXXBASE_CMAKE_DIR}/exec.cmake) to include this file.

# Execute command
function(execute)
execute_process(COMMAND ${ARGN})
endfunction()

# cmake
function(cmake)
execute(${CMAKE_COMMAND} ${ARGN})
endfunction()

# printf
function(printf)
execute(printf ${ARGN})
endfunction()

# c compiler
function(cc)
execute(${CMAKE_C_COMPILER} ${ARGN})
endfunction()

# c++ compiler
function(cxxc)
execute(${CMAKE_CXX_COMPILER} ${ARGN})
endfunction()

# linker
function(ld)
execute(${CMAKE_LINKER} ${ARGN})
endfunction()