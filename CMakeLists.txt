cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp)

add_library(dtp)

set(public_headers
	include/dtp/dtp.h
	)
set(sources
	${public_headers}
	src/dtp.cpp
	)

target_sources(dtp PRIVATE ${sources})
