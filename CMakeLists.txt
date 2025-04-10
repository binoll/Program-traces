cmake_minimum_required(VERSION 3.10)
project(Program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        Program_traces
        main.cpp
        analysis/registry/registry_parser.cpp
        analysis/registry/handles.cpp
        logger/logger.cpp
        analysis/prefetch/parser/prefetch_parser.cpp
        analysis/prefetch/metadata/file_metric.cpp
        analysis/prefetch/metadata/volume_info.cpp
        analysis/prefetch/model/prefetch_data.cpp
        analysis/prefetch/model/prefetch_data_builder.cpp
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
