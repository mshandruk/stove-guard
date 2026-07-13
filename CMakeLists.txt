cmake_minimum_required(VERSION 3.16)
project(StoveGuard LANGUAGES CXX)

cmake_policy(SET CMP0144 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(BUILD_TESTS "Build tests" OFF)
option(BUILD_DEMO "Build demo application" ON)
option(BUILD_VIDEO_DEMO "Build video demo application" ON)

# Dev tools
if (EXISTS "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake/DevTools.cmake")
    list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/external/cpp-infra/cmake")
    include(DevTools)

    add_dev_tools_targets(
            INFRA_DIR "${PROJECT_SOURCE_DIR}/external/cpp-infra"
            CHECK_DIRS
            "${PROJECT_SOURCE_DIR}/src"
            "${PROJECT_SOURCE_DIR}/tests"
            "${PROJECT_SOURCE_DIR}/apps"
    )
endif ()

if (WIN32)
    set(ONNXRUNTIME_ROOT
            "${CMAKE_SOURCE_DIR}/third_party/onnxruntime-win-x64"
    )
elseif (APPLE)
    set(ONNXRUNTIME_ROOT
            "${CMAKE_SOURCE_DIR}/third_party/onnxruntime-macos-arm64"
    )
else ()
    set(ONNXRUNTIME_ROOT
            "${CMAKE_SOURCE_DIR}/third_party/onnxruntime-linux-x64"
    )
endif ()

find_package(OpenCV REQUIRED)
find_package(OnnxRuntime REQUIRED)

# ===== Domain layer (static lib) =====
add_library(stove_guard_lib STATIC
        src/StoveMonitor.cpp
        src/SafetyService.cpp
        src/SceneMapper.cpp
        src/ConsoleNotifier.cpp
        src/FrameTimer.cpp
        src/RealClock.cpp
        src/VideoPipeline.cpp
        src/DetectionFilter.cpp
        src/YoloFrameAnalyzer.cpp
)

target_include_directories(stove_guard_lib
        PUBLIC
        "${PROJECT_SOURCE_DIR}/src"
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(stove_guard_lib PUBLIC
        compile_options
        ${OpenCV_LIBS}
        onnxruntime::onnxruntime
)

if (BUILD_TESTS)
    enable_testing()
    include(cmake/Dependencies.cmake)
    add_subdirectory(tests)
endif ()

if (BUILD_VIDEO_DEMO)
    add_subdirectory(apps/common)
    add_subdirectory(apps/video_demo)
endif ()