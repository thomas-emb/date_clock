#include <gtest/gtest.h>

#include "clock_simulator.hpp"

typedef tuple<Clock, string, Sensor, bool> Expectations;

vector<Expectations> expectations = {
    {{ 0,  0,  0}, "16 Jun, day18 non-leap", {0,0,0,0}, false}, // non-valid weekday
    {{ 0,  0, 36}, "16 Jun, day0 non-leap",  {1,0,0,0}, false}, // week day 0
    {{ 0,  0, 34}, "16 Jun, day1 non-leap",  {1,0,0,0}, false}, // week day 1
    {{ 0,  0, 32}, "16 Jun, day2 non-leap",  {1,0,0,0}, false}, // week day 2
    {{ 0,  0, 30}, "16 Jun, day3 non-leap",  {1,0,0,0}, false}, // week day 3
    {{ 0,  0, 28}, "16 Jun, day4 non-leap",  {1,0,0,0}, false}, // week day 4
    {{ 0,  0, 26}, "16 Jun, day5 non-leap",  {1,0,0,0}, false}, // week day 5
    {{ 0,  0, 24}, "16 Jun, day6 non-leap",  {1,0,0,0}, false}, // week day 6
    {{ 0,  0, 31},"16 Jun, day2.5 non-leap", {1,0,0,0}, false}, // in-between valid weekdays
    {{ 0, 45, 36},  "1 Jun, day0 non-leap",  {1,0,0,0}, false}, // 1st of month
    {{ 0, 15, 36}, "31 Jun, day0 non-leap",  {1,0,0,0}, false}, // 31st of month (no range checking)
    {{ 0, 44, 36},  "0 Jun, day0 non-leap",  {1,0,0,0}, false}, // before 1st of month
    {{ 0, 43, 36}, "-1 Jun, day0 non-leap",  {1,0,0,0}, false}, // even further back
    {{ 0, 16, 36},"-28 Jun, day0 non-leap",  {1,0,0,0}, false}, // past last of month

};

class TestSimulator: public ::testing::TestWithParam<Expectations>
{};


bool operator==(const Sensor& left, const Sensor& right)
{
    return (left.d1_30 == right.d1_30) && (left.d_29_31 == right.d_29_31) && (left.m == right.m) && (left.y == right.y);
}


TEST_P(TestSimulator, checkString)
{
    auto param = GetParam();
    Clock clock(get<0>(param));
    EXPECT_EQ(string(clock), get<1>(param));
    //EXPECT_EQ(sense(clock, HiZ), get<2>(param));
}

INSTANTIATE_TEST_SUITE_P
(
    Strings,
    TestSimulator,
    ::testing::ValuesIn
    (
        expectations
    )
);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}