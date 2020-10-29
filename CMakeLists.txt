cmake_minimum_required(VERSION 3.13)
project(arg_parser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 14)

include_directories(
    src/
    inc/
)

set(
    SRC_FILES
    src/arg_parser.cpp
)

# executable
add_executable(
    arg_parser_exec
    ${SRC_FILES}
    src/main.cpp
)

# library
add_library(
    arg_parser
    SHARED
    ${SRC_FILES}
)
