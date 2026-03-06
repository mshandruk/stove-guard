cmake_minimum_required(VERSION 3.16)
project(StoveGuard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

option(BUILD_DEMO "Build demo application" ON)
option(BUILD_TESTS "Build tests" OFF)

# Dev tools
if (EXISTS "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake/DevTools.cmake")
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake")
    include(DevTools)
    add_dev_tools_targets(
            INFRA_DIR "${PROJECT_SOURCE_DIR}/external/cpp-infra"
            CHECK_DIRS "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/tests" "${PROJECT_SOURCE_DIR}/apps"
    )
endif ()

# ===== Domain layer (static lib) =====
add_library(stove_guard_lib STATIC
        src/StoveMonitor.cpp
        src/DetectionMapper.cpp
        src/StoveGuardApp.cpp
        src/ConsoleNotifier.cpp
        src/FrameTimer.cpp
)
target_include_directories(stove_guard_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(stove_guard_lib PUBLIC
        compile_options
)

if (BUILD_TESTS)
    enable_testing()
    include(cmake/Dependencies.cmake)
    add_subdirectory(tests)
endif ()

if (BUILD_DEMO)
    add_subdirectory(apps/demo)
endif ()