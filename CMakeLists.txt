cmake_minimum_required(VERSION 3.1)

project(soren)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
    "main.cpp"

    "core/types.h"
    "core/offset-map.h"

    "core/por-bytecode.h"
    "core/por-bytecode.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})
