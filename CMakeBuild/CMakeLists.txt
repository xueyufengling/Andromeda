cmake_minimum_required (VERSION 3.5)

# Build shared lib when set to true, or static lib when set to false.
set(BUILD_SHARED_LIB false)
# Set target OS type, valid type are Windows, Linux.
set(BUILD_TARGET_SYSTEM Windows)
# Set target architecture type, valid type are x86_64, aarch64.
set(BUILD_TARGET_ARCHITECTURE x86_64)

if(BUILD_TARGET_SYSTEM MATCHES "Windows")
	if(BUILD_TARGET_ARCHITECTURE MATCHES "x86_64")
		message(STATUS "Building target x86_64 Windows")
		set(CMAKE_TOOLCHAIN_FILE "./cmake/x86_64-windows-toolchain.cmake")
	elseif(BUILD_TARGET_ARCHITECTURE MATCHES "aarch64")
		message(STATUS "Building target aarch64 Windows")
	else()
		message(STATUS "Unknown target architechture")
	endif()
endif()

project(andromeda)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes -Wno-deprecated-declarations -Wno-pointer-arith -Wno-stringop-overflow" )
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

file(GLOB SOURCES
	"dep/opengl/glad/*.c"
	"src/andromeda/*.cpp"
	"src/andromeda/app/*.cpp"
	"src/andromeda/audio/*.cpp"
	"src/andromeda/font/*.cpp"
	"src/andromeda/graphics/*.cpp"
	"src/andromeda/io/*.cpp"
	"src/andromeda/media/*.cpp"
	"src/andromeda/thread/*.cpp"
	"src/andromeda/util/*.cpp"
)

if(BUILD_SHARED_LIB)
	add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
	add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()

# Include Andomeda header files
target_include_directories(${PROJECT_NAME} PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
# Include dependency lib header files
target_include_directories(${PROJECT_NAME} PRIVATE  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/dep>)
