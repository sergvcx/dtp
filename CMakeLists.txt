cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp)

add_library(dtp)
add_library(dtp::dtp ALIAS dtp)

set(public_headers
	include/dtp/dtp.h
	)
set(sources
	${public_headers}
	src/dtp.cpp
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

