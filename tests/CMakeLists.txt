cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-tests)



if(NM)
    add_subdirectory(nm6407)
else()
    add_subdirectory(mc12101)
    add_subdirectory(x86)
endif()