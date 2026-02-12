cmake_minimum_required(VERSION 3.16)
project(GasStoveGuard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXE_NAME gas_stove_guard)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/cpp-infra/cmake")
include(DevTools)
add_dev_tools_targets(
        INFRA_DIR "${CMAKE_SOURCE_DIR}/external/cpp-infra"
        CHECK_DIRS "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/tests"
)

set(SOURCES
        src/main.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${EXE_NAME})
