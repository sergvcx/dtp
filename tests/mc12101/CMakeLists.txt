cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-mc12101-tests)

include(../cmake/nm-build.cmake)

mc12101_executable(nm-part-test0 nm-part0.cpp 
    LINKER_SCRIPT mc12101brd0.lds 
    INCLUDE_DIRS -I${CMAKE_CURRENT_LIST_DIR}/../../include -I${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
    LIBS -ldtp-nm 
    LIBDIRS -L${CMAKE_CURRENT_LIST_DIR}/../../lib)

mc12101_executable(nm-part-test1 nm-part1.cpp 
    LINKER_SCRIPT mc12101brd1.lds 
    INCLUDE_DIRS -I${CMAKE_CURRENT_LIST_DIR}/../../include -I${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
    LIBS -ldtp-nm 
    LIBDIRS -L${CMAKE_CURRENT_LIST_DIR}/../../lib)


add_executable(host-test host.cpp)
target_link_directories(host-test
    PRIVATE
    $ENV{MC12101}/lib
    ${CMAKE_CURRENT_LIST_DIR}/../../lib
    )
#target_link_libraries(host-test PRIVATE dtp::dtp mc12101load)    
target_link_libraries(host-test PRIVATE dtp-x64d mc12101load)    
target_include_directories(host-test
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    PRIVATE
    $ENV{HAL}/include
	$ENV{MC12101}/include
    ${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
)

#add_test()