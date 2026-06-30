set(TEST_TARGET stove_guard_tests)

add_executable(${TEST_TARGET}
        StoveMonitorTests.cpp
        SceneMapperTests.cpp
        DetectionFilterTests.cpp
        SafetyServiceTests.cpp
        VideoPipelineTests.cpp
        StateStabilizerTests.cpp
)

target_link_libraries(${TEST_TARGET}
        PRIVATE
        apps_common
        GTest::gtest
        GTest::gtest_main

)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})