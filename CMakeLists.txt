cmake_minimum_required(VERSION 3.10)
project(Program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        Program_traces
        main.cpp
        registry_analysis/registry_analyzer.cpp
        logger/logger.cpp
)

find_package(spdlog REQUIRED)
find_library(REGF_LIBRARY NAMES regf libregf)

if (NOT REGF_LIBRARY)
    message(FATAL_ERROR "Библиотека libref не найдена. Установите ее или укажите путь вручную.")
endif ()

message(STATUS "Найдена библиотека libregf: ${REGF_LIBRARY}")

target_link_libraries(Program_traces
        spdlog::spdlog
        ${REGF_LIBRARY}
)