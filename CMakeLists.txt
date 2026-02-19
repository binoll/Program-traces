cmake_minimum_required(VERSION 3.20)

project(ProgramTraces LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(LIBS_BASE_PATH ${CMAKE_SOURCE_DIR}/libs/macos)

file(GLOB_RECURSE CORE_SOURCES
    core/*.cpp
)
file(GLOB_RECURSE ARTIFACT_SOURCES
    artifacts/*.cpp
)
file(GLOB_RECURSE PARSER_SOURCES
    parsers/*.cpp
)
file(GLOB_RECURSE IO_SOURCES
    io/*.cpp
)
file(GLOB_RECURSE CONFIG_SOURCES
    config/*.cpp
)
file(GLOB_RECURSE UTILS_SOURCES
    utils/*.cpp
)

set(ALL_SOURCES
    main.cpp
    ${CORE_SOURCES}
    ${ARTIFACT_SOURCES}
    ${PARSER_SOURCES}
    ${IO_SOURCES}
    ${CONFIG_SOURCES}
    ${UTILS_SOURCES}
)

add_executable(program_traces ${ALL_SOURCES})

target_include_directories(program_traces PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${LIBS_BASE_PATH}/libregf/include
    ${LIBS_BASE_PATH}/libscca/include
    ${LIBS_BASE_PATH}/libevtx/include
    ${LIBS_BASE_PATH}/libevt/include
    ${LIBS_BASE_PATH}/libspdlog/include
    ${LIBS_BASE_PATH}/libfmt/include
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(program_traces PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wnon-virtual-dtor
    )
endif()

target_link_libraries(program_traces PRIVATE
    ${LIBS_BASE_PATH}/libregf/libregf.a
    ${LIBS_BASE_PATH}/libscca/libscca.a
    ${LIBS_BASE_PATH}/libevtx/libevtx.a
    ${LIBS_BASE_PATH}/libevt/libevt.a
    ${LIBS_BASE_PATH}/libspdlog/libspdlog.a
    ${LIBS_BASE_PATH}/libfmt/libfmt.a
)

if (APPLE)
    find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)
    find_library(IOKIT_LIB IOKit REQUIRED)

    target_link_libraries(program_traces PRIVATE
        ${COREFOUNDATION_LIB}
        ${IOKIT_LIB}
    )
endif()

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${ALL_SOURCES})
