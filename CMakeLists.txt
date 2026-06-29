cmake_minimum_required(VERSION 3.20)
project(SmallCppProgram)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(SmallCppProgram
        main.cpp
        config.cpp
        graph_generators.cpp
        graph_utils.cpp
)

target_include_directories(SmallCppProgram PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
