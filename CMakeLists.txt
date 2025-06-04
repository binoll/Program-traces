cmake_minimum_required(VERSION 3.10)
project(program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        program_traces
        main.cpp
        analysis/prefetch/model/data.cpp
        analysis/prefetch/model/data_builder.cpp
        analysis/prefetch/metadata/file_metric.cpp
        analysis/prefetch/metadata/volume_info.cpp
        analysis/prefetch/parser/parser.cpp
        analysis/registry/data_types/key.cpp
        analysis/registry/data_types/value.cpp
        analysis/registry/data_model/data.cpp
        analysis/registry/data_model/data_builder.cpp
        analysis/registry/data_model/storage/data_storage.cpp
        analysis/registry/enums/value_type_utils.cpp
        analysis/registry/parser/parser.cpp
        core/os_detection/os_detection.cpp
        utils/logging/logger.cpp
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
