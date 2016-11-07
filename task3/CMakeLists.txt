cmake_minimum_required(VERSION 2.6)
project(task3)

add_executable(runCompile runCompile.cpp Types.cpp table_types.hpp parser/Schema.cpp parser/Parser.cpp)
add_executable(runDatabase runDatabaseTest.cpp Types.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native")

install(TARGETS runCompile RUNTIME DESTINATION bin)
