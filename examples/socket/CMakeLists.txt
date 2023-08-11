cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

project(dtp-examples-socket)

add_executable(dtp-examples-socket-server server.cpp)
add_executable(dtp-examples-socket-client client.cpp)

target_link_libraries(dtp-examples-socket-client
    PUBLIC
        dtp::dtp)

target_link_libraries(dtp-examples-socket-server
    PUBLIC
        dtp::dtp)
