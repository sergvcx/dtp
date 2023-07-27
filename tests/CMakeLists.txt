cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-tests)


if(NM)
add_executable(nm-part-test nm-part.cpp)

set(target_list nm-part-test)

foreach(target ${target_list})
    target_link_libraries(${target} PRIVATE dtp::dtp)

    target_link_directories(${target} PRIVATE $ENV{MC12101}/lib $ENV{HAL}/lib)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include $ENV{HAL}/include)
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src)
    target_link_libraries(${target} PRIVATE hal_mc12101)

    if(NM)
    target_link_options(nm-part-test PUBLIC 
        -Wl,-T${CMAKE_CURRENT_LIST_DIR}/mc12101brd.lds
        -Wl,--whole-archive 
            -lmc12101load_nm 
            -lnm6407_io_nmc
            #-lmc12101_stub_nmc4_float
        -Wl,--no-whole-archive
        )
    endif()
endforeach()
endif()

if(NOT NM)
    add_executable(host-test host.cpp)
    target_link_directories(host-test
        PRIVATE
        $ENV{HAL}/lib
        )
    target_link_libraries(host-test PRIVATE dtp::dtp hal_x86 dl)    
    target_include_directories(host-test
        PUBLIC 
        ${CMAKE_CURRENT_LIST_DIR}/../include
        PRIVATE
        $ENV{HAL}/include
    )
endif()