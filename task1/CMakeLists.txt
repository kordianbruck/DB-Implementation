cmake_minimum_required(VERSION 2.6)
project(task1)

add_executable(task1 main.cpp neworderrandom.cpp Types.cpp)

install(TARGETS task1 RUNTIME DESTINATION bin)
