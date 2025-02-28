cmake_minimum_required(VERSION 3.10)
project(Program_traces)

set(CMAKE_CXX_STANDARD 20)

add_executable(
        Program_traces
        main.cpp
        file_reader/file_reader.cpp

)
