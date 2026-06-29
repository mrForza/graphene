cmake_minimum_required(VERSION 3.20)
project(SmallCppProgram)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(third_party/yaml-cpp-0.8.0)

add_executable(SmallCppProgram
        main.cpp
        config.cpp
        graph_generators.cpp
        graph_utils.cpp
)

target_include_directories(SmallCppProgram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SmallCppProgram PRIVATE yaml-cpp)
string(REPLACE "-fmodules-ts" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fmodule-mapper" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fdeps-format=p1689r5" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
