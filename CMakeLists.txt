cmake_minimum_required(VERSION 3.10)
project(Program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        Program_traces
        main.cpp
        analysis/registry/registry_parser.cpp
        logger/logger.cpp
        analysis/prefetch_file/prefetch_file_parser.cpp
        analysis/prefetch_file/prefetch_file_info.cpp
        analysis/prefetch_file/info/volume.cpp
        analysis/prefetch_file/info/file_metric.cpp
        analysis/registry/registry_parser.cpp
        analysis/registry/handles.cpp
)

find_package(spdlog REQUIRED)

find_library(REGF_LIBRARY NAMES regf libregf)
find_library(SCCA_LIBRARY NAMES scca libscca)

if (NOT REGF_LIBRARY)
    message(FATAL_ERROR "Библиотека libregf не найдена. Установите ее или укажите путь вручную.")
endif ()

if (NOT SCCA_LIBRARY)
    message(FATAL_ERROR "Библиотека libscca не найдена. Установите ее или укажите путь вручную.")
endif ()

message(STATUS "Найдена библиотека libregf: ${REGF_LIBRARY}")
message(STATUS "Найдена библиотека libscca: ${SCCA_LIBRARY}")

target_link_libraries(Program_traces
        spdlog::spdlog
        ${REGF_LIBRARY}
        ${SCCA_LIBRARY}
)
