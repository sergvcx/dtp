cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-file-example)

add_executable(dtp-file-example main.cpp)

target_link_libraries(dtp-file-example 
    PUBLIC 
        dtp::dtp    
    )

target_link_directories(dtp-file-example 
    PUBLIC
        $ENV{MC12101}/lib
    )
target_include_directories(dtp-file-example PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)

if(NM)
target_link_options(dtp-file-example PUBLIC 
    -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd.lds
    -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/demo.map
    -Wl,--whole-archive 
        -lmc12101load_nm 
        -lnm6407_io_nmc
    -Wl,--no-whole-archive
    )
endif()
