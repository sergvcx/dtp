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
	include/dtp/ringbuffer.h
	)
set(sources
	${public_headers}
	src/dtp.cpp
	src/ringbuffer.c
	src/file.cpp
	src/nm6407/link.c
	src/nm6407/dtpLinkInit.S
	src/nm6407/dtpLinkStart.S
	src/link-core.h
	)

set_source_files_properties(
	src/nm6407/dtpLinkInit.S
	src/nm6407/dtpLinkStart.S
	PROPERTIES
	COMPILE_FLAGS "-mgas")

target_sources(dtp PRIVATE ${sources})
target_include_directories(dtp 
	PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/include
	PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/src
		)

set_target_properties(dtp PROPERTIES
	OUTPUT_NAME dtp${SUFFIX}
	DEBUG_POSTFIX d
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/lib
	)

add_subdirectory(examples)
add_subdirectory(tests)

