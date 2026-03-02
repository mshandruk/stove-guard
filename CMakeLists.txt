cmake_minimum_required(VERSION 3.16)
project(StoveGuard LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Dev tools
if (EXISTS "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake/DevTools.cmake")
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake")
    include(DevTools)
    add_dev_tools_targets(
            INFRA_DIR "${PROJECT_SOURCE_DIR}/external/cpp-infra"
            CHECK_DIRS "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/tests"
    )
endif ()

# ===== Domain layer (static lib) =====
add_library(stove_guard_lib STATIC
        src/StoveMonitor.cpp
        src/DetectionMapper.cpp
        src/StoveGuardApp.cpp
        src/ConsoleNotifier.cpp
)
target_include_directories(stove_guard_lib
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(stove_guard_lib PUBLIC
        compile_options
)

# ===== Application layer =====
add_executable(stove_guard_app src/main.cpp)
target_link_libraries(stove_guard_app
        PRIVATE
        stove_guard_lib
)

option(ENABLE_TESTS "Enable tests" OFF)

if (ENABLE_TESTS)
    enable_testing()
    include(cmake/Dependencies.cmake)
    add_subdirectory(tests)
endif ()