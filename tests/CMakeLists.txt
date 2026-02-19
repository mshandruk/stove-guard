set(TEST_TARGET stove_monitor_tests)

add_executable(${TEST_TARGET} StoveMonitorTests.cpp DetectionMapperTests.cpp StoveGuardAppTests.cpp)

target_link_libraries(${TEST_TARGET}
        PRIVATE
        stove_guard_lib
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})