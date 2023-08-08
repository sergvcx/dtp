cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-examples)

add_subdirectory(file)
add_subdirectory(file2)
if(NOT NM)
#add_subdirectory(buffer)
endif()