cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(debr)

add_library(debr)

set(public_headers
	include/debr/debr.h
	)
set(sources
	${public_headers}
	src/debr.cpp
	)

target_sources(debr PRIVATE ${sources})
