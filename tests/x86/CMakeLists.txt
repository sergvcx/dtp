cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-tests-x86)

add_executable(dtp-tests-x86-buffer buffer.cpp)
target_link_libraries(dtp-tests-x86-buffer 
    PUBLIC
        dtp::dtp
        mc12101load)



target_include_directories(dtp-tests-x86-buffer
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
        $ENV{MC12101}
        )