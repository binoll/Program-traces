cmake_minimum_required(VERSION 3.10)
project(program_traces)

set(CMAKE_CXX_STANDARD 23)

# Базовый путь к библиотекам
set(LIBS_BASE_PATH ${CMAKE_SOURCE_DIR}/libs/macos)

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

# Подключение заголовков
target_include_directories(program_traces PRIVATE
        ${LIBS_BASE_PATH}/libregf/include
        ${LIBS_BASE_PATH}/libscca/include
        ${LIBS_BASE_PATH}/libevtx/include
        ${LIBS_BASE_PATH}/libevt/include
        ${LIBS_BASE_PATH}/libspdlog/include
        ${LIBS_BASE_PATH}/libfmt/include
        ${CMAKE_SOURCE_DIR}
)

# Линковка библиотек
target_link_libraries(program_traces
        ${LIBS_BASE_PATH}/libregf/libregf.a
        ${LIBS_BASE_PATH}/libscca/libscca.a
        ${LIBS_BASE_PATH}/libevtx/libevtx.a
        ${LIBS_BASE_PATH}/libevt/libevt.a
        ${LIBS_BASE_PATH}/libspdlog/libspdlog.a
        ${LIBS_BASE_PATH}/libfmt/libfmt.a
)

# Для macOS
if (APPLE)
    find_library(CF_LIB CoreFoundation)
    find_library(IOKIT_LIB IOKit)
    target_link_libraries(program_traces ${CF_LIB} ${IOKIT_LIB})
endif ()
