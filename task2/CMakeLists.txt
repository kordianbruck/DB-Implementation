cmake_minimum_required(VERSION 2.6)
project(task2)

add_executable(task2 main.cpp Types.cpp table_types.hpp database.cpp parser/Schema.cpp parser/Parser.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native")

install(TARGETS task2 RUNTIME DESTINATION bin)
