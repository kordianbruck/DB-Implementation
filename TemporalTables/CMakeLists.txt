cmake_minimum_required(VERSION 2.6)
project(task5)

FIND_PACKAGE(Boost 1.63 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

set(SOURCE_FILES
        utils/Types.cpp utils/DatabaseTools.cpp
        parser/Schema.cpp parser/SchemaParser.cpp parser/Query.cpp parser/IU.h parserNew/SQLLexer.cpp parserNew/SQLParser.cpp
        utils/md5.cpp
        operators/TableScan.cpp operators/TableScan.h operators/Selection.cpp
        operators/Selection.h operators/HashJoin.cpp operators/HashJoin.h operators/Print.cpp
        operators/Print.h operators/Operator.cpp operators/Operator.h )

add_executable(runCompile RunCompile.cpp ${SOURCE_FILES})

add_executable(runBtree BtreeTest.cpp)

add_executable(runParser TestParser.cpp  ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(runCompile ${Boost_LIBRARIES})

SET(CMAKE_CXX_FLAGS "-std=c++1y")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fshow-column -pipe -march=native -ldl -lrt")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_DEBUG_MACRO -fsanitize=undefined -fsanitize=address -fsanitize=leak -ggdb3 -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -flto -DNDEBUG -ggdb")

install(TARGETS runCompile RUNTIME DESTINATION bin)
