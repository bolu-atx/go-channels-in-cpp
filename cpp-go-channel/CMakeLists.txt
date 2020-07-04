cmake_minimum_required(VERSION 3.10)
project(cpp_go_channel)
set(CMAKE_CXX_STANDARD 11)

add_executable(cpp_go_channel main.cpp Channel.h)

if (UNIX)
    target_link_libraries(cpp_go_channel pthread)
endif()