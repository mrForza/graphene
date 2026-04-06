cmake_minimum_required(VERSION 3.20)
project(SmallCppProgram)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(SmallCppProgram
        main.cpp
        graph_generators.cpp
        graph_utils.cpp
        dijkstra.h
        bellman_ford.h
        graph_types.h
        bmssp.h
        graph_generators.h
        graph_utils.h
        benchmark.h
)

target_include_directories(SmallCppProgram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fmodule-mapper" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fdeps-format=p1689r5" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
