cmake_minimum_required(VERSION 3.10)
project(Program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        Program_traces
        main.cpp
        file_reader/file_reader.cpp
        registry_analysis/registry_analysis.cpp
        logger/logger.cpp
)

find_package(spdlog REQUIRED)
target_link_libraries(Program_traces spdlog::spdlog)
