cmake_minimum_required(VERSION 2.6)
project(task2)

add_executable(runCompile runCompile.cpp Types.cpp table_types.hpp parser/Schema.cpp parser/Parser.cpp)
add_executable(runDatabase runDatabaseTest.cpp Types.cpp)

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -DNDEBUG")

install(TARGETS runCompile RUNTIME DESTINATION bin)
