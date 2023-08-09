cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp LANGUAGES C CXX)
if(NM)
enable_language(ASM)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

add_library(dtp)
add_library(dtp::dtp ALIAS dtp)

set(public_headers
	include/dtp/dtp.h
	include/dtp/mc12101-host.h
	include/dtp/file.h
	)

set(sources
	${public_headers}
	src/dtp.cpp
	src/file.c
	)

if(NM)
set(SUFFIX -nm)
list(APPEND sources 
	src/nm/mc12101.cpp
	src/nm/ringbuffer.cpp
	)
else()
set(SUFFIX -x64)
list(APPEND sources 
	src/pc/mc12101.cpp
	src/pc/ringbuffer.cpp
	)
target_include_directories(dtp PRIVATE $ENV{MC12101}/include)
endif()

if(UNIX)
	target_compile_options(dtp PUBLIC -fpermissive)
endif()

target_sources(dtp PRIVATE ${sources})
target_include_directories(dtp 
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/include
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/src
		$ENV{HAL}/include
		)

set_target_properties(dtp PROPERTIES
	OUTPUT_NAME dtp${SUFFIX}
	#DEBUG_POSTFIX d
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/lib
	)

add_subdirectory(examples)

