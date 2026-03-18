cmake_minimum_required(VERSION 3.16)
project(AppsCommon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(apps_common STATIC
        FakeFrameAnalyzer.cpp
)
target_include_directories(apps_common
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(apps_common
        PUBLIC
        stove_guard_lib
)