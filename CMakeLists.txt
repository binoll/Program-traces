cmake_minimum_required(VERSION 3.10)
project(program_traces)

set(CMAKE_CXX_STANDARD 23)

add_executable(
        program_traces
        main.cpp
        parsers/prefetch/data_model/data.cpp
        parsers/prefetch/data_model/data_builder.cpp
        parsers/prefetch/metadata/file_metric.cpp
        parsers/prefetch/metadata/volume_info.cpp
        parsers/prefetch/parser/parser.cpp
        parsers/registry/data_types/key.cpp
        parsers/registry/data_types/value.cpp
        parsers/registry/data_model/data.cpp
        parsers/registry/data_model/data_builder.cpp
        parsers/registry/data_model/storage/data_storage.cpp
        parsers/registry/enums/value_type_utils.cpp
        parsers/registry/parser/parser.cpp
        parsers/logs/evtx/parser/parser.cpp
        parsers/logs/evt/parser/parser.cpp
        parsers/logs/data_model/data.cpp
        core/analysis/os_detection/os_detection.cpp
        utils/logging/logger.cpp
        utils/config/config.cpp
        core/analysis/program_analysis/autorun/autorun_analyzer.cpp
        core/analysis/program_analysis/amcache/amcache_analyzer.cpp
        core/analysis/program_analysis/prefetch/prefetch_analyzer.cpp
        core/analysis/program_analysis/logs/eventlog_analyzer.cpp
        core/analysis/program_analysis/windows_disk_analyzer.cpp
        utils/export/csv_exporter.cpp
)

find_package(spdlog REQUIRED)
find_library(REGF_LIBRARY NAMES regf libregf)
find_library(SCCA_LIBRARY NAMES scca libscca)
find_library(EVTX_LIBRARY NAMES evtx libevtx)
find_library(EVT_LIBRARY NAMES evt libevt)

if (NOT REGF_LIBRARY)
    message(FATAL_ERROR "Библиотека libregf не найдена. Установите ее или укажите путь вручную.")
endif ()

if (NOT SCCA_LIBRARY)
    message(FATAL_ERROR "Библиотека libscca не найдена. Установите ее или укажите путь вручную.")
endif ()

if (NOT EVTX_LIBRARY)
    message(FATAL_ERROR "Библиотека libevtx не найдена. Установите ее или укажите путь вручную.")
endif ()

if (NOT EVT_LIBRARY)
    message(FATAL_ERROR "Библиотека libevt не найдена. Установите ее или укажите путь вручную.")
endif ()

message(STATUS "Найдена библиотека libregf: ${REGF_LIBRARY}")
message(STATUS "Найдена библиотека libscca: ${SCCA_LIBRARY}")
message(STATUS "Найдена библиотека libscca: ${EVTX_LIBRARY}")
message(STATUS "Найдена библиотека libregf: ${EVT_LIBRARY}")

target_link_libraries(program_traces
        spdlog::spdlog
        ${REGF_LIBRARY}
        ${SCCA_LIBRARY}
        ${EVTX_LIBRARY}
        ${EVT_LIBRARY}
)
