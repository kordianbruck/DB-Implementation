cmake_minimum_required(VERSION 2.6)
project(task1)

add_definitions(-std=c++11)
add_executable(task1 main.cpp Types.cpp table_types.hpp)

install(TARGETS task1 RUNTIME DESTINATION bin)
