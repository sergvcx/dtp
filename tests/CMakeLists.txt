cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-tests)

add_executable(shmem-test shmem.cpp)
add_executable(link-test link.cpp)

set(target_list shmem-test link-test)

foreach(target ${target_list})
    target_link_libraries(${target} PRIVATE dtp::dtp)

    target_link_directories(${target} PRIVATE $ENV{MC12101}/lib)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src)

    if(NM)
    target_link_options(shmem-test PUBLIC 
        -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd.lds
        -Wl,--whole-archive 
            -lmc12101load_nm 
            -lnm6407_io_nmc
            #-lmc12101_stub_nmc4_float
        -Wl,--no-whole-archive
        )
    endif()
endforeach()
