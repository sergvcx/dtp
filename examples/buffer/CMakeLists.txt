cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-buffer-example)
include(nm-build.cmake)

# mc12101_executable(dtp-buffer-example-nm0 nm0/main.cpp 
#     LINKER_SCRIPT mc12101brd-nmc0.lds 
#     LIBDIRS 
#         -L${CMAKE_CURRENT_LIST_DIR}/nm0
#         -L${CMAKE_CURRENT_LIST_DIR}/../../lib
#     INCLUDE_DIRS 
#         -I${CMAKE_CURRENT_LIST_DIR}/../../include
#         -I$ENV{HAL}/include
#         -I$ENV{MC12101}/include
#     LIBS
#         -ldtp-nm)
# mc12101_executable(dtp-buffer-example-nm1 nm1/main.cpp 
#     LINKER_SCRIPT mc12101brd-nmc1.lds 
#     LIBDIRS 
#         -L${CMAKE_CURRENT_LIST_DIR}/nm1
#         -L${CMAKE_CURRENT_LIST_DIR}/../../lib
#     INCLUDE_DIRS 
#         -I${CMAKE_CURRENT_LIST_DIR}/../../include
#         -I$ENV{HAL}/include
#     LIBS
#         -ldtp-nm)


add_executable(dtp-buffer-example pc/pc-main.cpp)

target_link_libraries(dtp-buffer-example 
    PUBLIC 
        dtp-x64d
        mc12101load   
    )
target_include_directories(dtp-buffer-example PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/../../include
    $ENV{MC12101}/include
    )
target_link_directories(dtp-buffer-example PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../lib
    $ENV{MC12101}/lib
    )
#endif()