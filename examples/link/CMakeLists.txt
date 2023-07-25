cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-link-example)

add_executable(dtp-link-example0 main0.cpp)
add_executable(dtp-link-example1 main1.cpp)

target_link_libraries(dtp-link-example0 
    PUBLIC 
        dtp::dtp    
    )
target_link_libraries(dtp-link-example1
    PUBLIC 
        dtp::dtp    
    )

target_link_directories(dtp-link-example0 
    PUBLIC
        $ENV{MC12101}/lib
    )
target_link_directories(dtp-link-example1
    PUBLIC
        $ENV{MC12101}/lib
    )
target_include_directories(dtp-link-example0 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
target_include_directories(dtp-link-example1 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)

if(NM)
target_link_options(dtp-link-example0 PUBLIC 
    -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd0.lds
    -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/main0.map
    -Wl,--whole-archive 
        -lmc12101load_nm 
        -lnm6407_io_nmc
    -Wl,--no-whole-archive
    )
target_link_options(dtp-link-example1 PUBLIC 
    -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd1.lds
    -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/main1.map
    -Wl,--whole-archive 
        -lmc12101load_nm 
        -lnm6407_io_nmc
    -Wl,--no-whole-archive
    )
endif()
