cmake_minimum_required(VERSION 2.6)
project(task1)

add_executable(task1 main.cpp Types.cpp table_types.hpp database.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native")

install(TARGETS task1 RUNTIME DESTINATION bin)
