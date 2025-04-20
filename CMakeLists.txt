cmake_minimum_required(VERSION 3.10)
project(program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        program_traces
        main.cpp
        analysis/parsers/prefetch/model/data.cpp
        analysis/parsers/prefetch/model/data_builder.cpp
        analysis/parsers/prefetch/metadata/file_metric.cpp
        analysis/parsers/prefetch/metadata/volume_info.cpp
        analysis/parsers/prefetch/parser/parser.cpp
        analysis/parsers/registry/parser/parser.cpp
        analysis/parsers/registry/handle/key.cpp
        analysis/parsers/registry/handle/value.cpp
        utils/logger/logger.cpp
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

target_link_libraries(program_traces
        spdlog::spdlog
        ${REGF_LIBRARY}
        ${SCCA_LIBRARY}
)
