cmake_minimum_required(VERSION 2.6)
project(task3)

#FIND_PACKAGE(Boost 1.40 COMPONENTS program_options REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

add_executable(runCompile runCompile.cpp Types.cpp table_types.hpp parser/Schema.cpp parser/Parser.cpp)
add_executable(runDatabase runDatabaseTest.cpp Types.cpp)

#TARGET_LINK_LIBRARIES(runCompile ${Boost_LIBRARIES})
#TARGET_LINK_LIBRARIES(runDatabase ${Boost_LIBRARIES})

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_DEBUG_MACRO -fsanitize=undefined -fsanitize=address -fsanitize=leak -ggdb3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -DNDEBUG -ggdb")

install(TARGETS runCompile RUNTIME DESTINATION bin)
