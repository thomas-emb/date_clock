add_executable(
  clock_test
  test.cpp
  ../src/clock.c
  clock_simulator.cpp
)
target_link_libraries(
  clock_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(clock_test)
