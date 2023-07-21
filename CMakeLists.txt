cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)

add_library(dtp)
add_library(dtp::dtp ALIAS dtp)

set(public_headers
	include/dtp/dtp.h
	)
set(sources
	${public_headers}
	src/dtp.cpp
	src/ringbuffer.c
	src/ringbuffer.h
	)

target_sources(dtp PRIVATE ${sources})
target_include_directories(dtp PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

set_target_properties(dtp PROPERTIES
	OUTPUT_NAME dtp
	DEBUG_POSTFIX d
	ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/lib
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/lib
	)

add_subdirectory(examples)

