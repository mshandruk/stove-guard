set(TEST_TARGET stove_monitor_tests)

add_executable(${TEST_TARGET} StoveMonitorTests.cpp)

target_link_libraries(${TEST_TARGET}
        PRIVATE
        stove_monitor
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})