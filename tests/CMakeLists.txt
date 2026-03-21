set(TEST_TARGET stove_guard_tests)

add_executable(${TEST_TARGET}
        StoveMonitorTests.cpp
        SceneMapperTests.cpp
        SafetyServiceTests.cpp
        DetectionFilterTests.cpp
)

target_link_libraries(${TEST_TARGET}
        PRIVATE
        stove_guard_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})