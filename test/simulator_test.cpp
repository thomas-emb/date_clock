#include <gtest/gtest.h>

#include "clock_simulator.hpp"

typedef tuple<Clock, string, Sensor, bool> EncodingExpectations;

vector<EncodingExpectations> weeks = {
    {{ 0,  0, 38}, "16 Jun, day-1 non-leap", {0,0,0,0}, false}, // non-valid weekday
    {{ 0,  0, 36}, "16 Jun, day0 non-leap",  {1,0,0,0}, false}, // week day 0
    {{ 0,  0, 34}, "16 Jun, day1 non-leap",  {1,0,0,0}, false}, // week day 1
    {{ 0,  0, 32}, "16 Jun, day2 non-leap",  {1,0,0,0}, false}, // week day 2
    {{ 0,  0, 31},"16 Jun, day2.5 non-leap", {0,0,0,0}, false}, // in-between valid weekdays
    {{ 0,  0, 30}, "16 Jun, day3 non-leap",  {1,0,0,0}, false}, // week day 3
    {{ 0,  0, 28}, "16 Jun, day4 non-leap",  {1,0,0,0}, false}, // week day 4
    {{ 0,  0, 26}, "16 Jun, day5 non-leap",  {1,0,0,0}, false}, // week day 5
    {{ 0,  0, 24}, "16 Jun, day6 non-leap",  {1,0,0,0}, false}, // week day 6
    {{ 0,  0, 22}, "16 Jun, day7 non-leap",  {0,0,0,0}, false}, // non-valid weekday
};
vector<EncodingExpectations> days = {
    {{ 0, 43, 36}, "-1 Jun, day0 non-leap",  {0,0,0,0}, false}, // 2 before 1sth of month
    {{ 0, 44, 36},  "0 Jun, day0 non-leap",  {0,0,0,0}, false}, // 1 before 1st of month
    {{ 0, 45, 36},  "1 Jun, day0 non-leap",  {1,0,0,0}, false}, // 1st of month
    {{ 0, 12, 36}, "28 Jun, day0 non-leap",  {1,0,0,0}, false}, // 28th of month
    {{ 0, 13, 36}, "29 Jun, day0 non-leap",  {1,1,0,0}, false}, // 29th of month
    {{ 0, 14, 36}, "30 Jun, day0 non-leap",  {1,1,0,0}, false}, // 30th of month
    {{ 0, 15, 36}, "31 Jun, day0 non-leap",  {0,1,0,0}, false}, // 31st of month (no range checking)
    {{ 0, 16, 36},"-28 Jun, day0 non-leap",  {0,0,0,0}, false}, // past last of month
};
vector<EncodingExpectations> months = {
    {{ 7,  0, 36}, "16 Jan, day0 non-leap",  {1,0,1,0}, false}, // January (31)
    {{ 8,  0, 36}, "16 Feb, day0 non-leap",  {1,0,1,0},  true}, // February (28/29)
    {{10,  0, 36}, "16 Apr, day0 non-leap",  {1,0,0,0}, false}, // April (30)
};
vector<EncodingExpectations> years = {
    {{ 0,  0, 36, 0}, "16 Jun, day0 leap",     {1,0,0,1}, false}, // leap year
    {{ 0,  0, 36, 1}, "16 Jun, day0 non-leap", {1,0,0,0}, false}, // non-leap year
    {{ 0,  0, 36, 2}, "16 Jun, day0 non-leap", {1,0,0,0}, false}, // non-leap year
    {{ 0,  0, 36, 3}, "16 Jun, day0 non-leap", {1,0,0,0}, false}, // non-leap year
};
vector<EncodingExpectations> constructor = {
    {{ Clock::Month::Jan,  1, 3, 0},  "1 Jan, day3 leap", {1,0,1,1}, false},
    {{ Clock::Month::Jun, 24, 0, 0}, "24 Jun, day0 leap", {1,0,0,1}, false},
};


class TestSimulator: public ::testing::TestWithParam<EncodingExpectations>{};


bool operator==(const Sensor& left, const Sensor& right)
{
    return (left.d1_30 == right.d1_30) && (left.d_29_31 == right.d_29_31) && (left.m == right.m) && (left.y == right.y);
}


TEST_P(TestSimulator, test)
{
    auto param = GetParam();
    Clock clock(get<0>(param));
    EXPECT_EQ(string(clock), get<1>(param));
    EXPECT_EQ(clock.sense(PullUp), get<2>(param));
    EXPECT_EQ(clock.sense(PullDown).m, get<2>(param).m ^ get<3>(param));
}

INSTANTIATE_TEST_SUITE_P(weeks, TestSimulator, ::testing::ValuesIn(weeks));
INSTANTIATE_TEST_SUITE_P(days, TestSimulator, ::testing::ValuesIn(days));
INSTANTIATE_TEST_SUITE_P(months, TestSimulator, ::testing::ValuesIn(months));
INSTANTIATE_TEST_SUITE_P(years, TestSimulator, ::testing::ValuesIn(years));
INSTANTIATE_TEST_SUITE_P(constuctor, TestSimulator, ::testing::ValuesIn(constructor));


class TestPin: public ::testing::TestWithParam<tuple<Pin, Pin, bool, bool>>{};

vector<tuple<Pin, Pin, bool, bool>> pinPermutations = {
    // inputA    inputB  output error
    {    High,     High,  true, false},
    {    High,   PullUp,  true, false},
    {    High,      HiZ,  true, false},
    {    High, PullDown,  true, false},
    {    High,      Low,  true,  true},

    {  PullUp,     High,  true, false},
    {  PullUp,   PullUp,  true, false},
    {  PullUp,      HiZ,  true, false},
    {  PullUp, PullDown,  true,  true},
    {  PullUp,      Low, false, false},

    {     HiZ,     High,  true, false},
    {     HiZ,   PullUp,  true, false},
    {     HiZ,      HiZ,  true,  true},
    {     HiZ, PullDown, false, false},
    {     HiZ,      Low, false, false},

    {PullDown,     High,  true, false},
    {PullDown,   PullUp,  true,  true},
    {PullDown,      HiZ, false, false},
    {PullDown, PullDown, false, false},
    {PullDown,      Low, false, false},
    
    {     Low,     High,  true,  true},
    {     Low,   PullUp, false, false},
    {     Low,      HiZ, false, false},
    {     Low, PullDown, false, false},
    {     Low,      Low, false, false},
};

TEST_P(TestPin, and)
{
    auto param = GetParam();
    if (get<3>(param))
        EXPECT_THROW(get<0>(param) & get<1>(param), exception);
    else
        ASSERT_EQ(get<0>(param) & get<1>(param), get<2>(param));
}

INSTANTIATE_TEST_SUITE_P(, TestPin, ::testing::ValuesIn(pinPermutations));