cmake_minimum_required(VERSION 2.6)
project(minihekaton)

add_executable(minihekaton main.cpp)

install(TARGETS minihekaton RUNTIME DESTINATION bin)
