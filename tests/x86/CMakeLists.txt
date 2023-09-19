cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-tests-x86)

add_executable(dtp-tests-x86-buffer buffer.cpp)

target_link_directories(dtp-tests-x86-buffer 
	PUBLIC 
		$ENV{MC12101}/lib)
target_link_libraries(dtp-tests-x86-buffer 
    PUBLIC
        dtp::dtp
        mc12101load)


if(WIN32)
target_link_libraries(dtp-tests-x86-buffer 
    PUBLIC
        w2_32)
endif()
target_include_directories(dtp-tests-x86-buffer
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/../../nmassert/include
        $ENV{MC12101}/include
        )