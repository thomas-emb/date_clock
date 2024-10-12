#include <gtest/gtest.h>

#include "../src/clock.h"
#include "clock_simulator.hpp"


class TestClock: public ::testing::TestWithParam<tuple<Clock, Clock>>{};

vector<tuple<Clock, Clock>> happyFlow = {
    {{Clock::Month::Oct,  1, 0}, {Clock::Month::Oct, 2, 1}}, // advance one day
    {{Clock::Month::Oct,  1, 6}, {Clock::Month::Oct, 2, 0}}, // wrap around week
    {{Clock::Month::Oct, 31, 0}, {Clock::Month::Nov, 1, 1}}, // advance to next month
    {{Clock::Month::Oct, 31, 6}, {Clock::Month::Nov, 1, 0}}, // advance to next month, and wrap around week
    {{Clock::Month::Nov, 30, 0}, {Clock::Month::Dec, 1, 1}}, // advance to next month, skipping day 31
    {{Clock::Month::Nov, 30, 6}, {Clock::Month::Dec, 1, 0}}, // advance to next month, skipping day 31, and wrap around week
    {{Clock::Month::Feb, 28, 0}, {Clock::Month::Mar, 1, 1}}, // advance from Febrary, skipping days 29-31
    {{Clock::Month::Feb, 28, 6}, {Clock::Month::Mar, 1, 0}}, // advance from February, skipping days 29-31, and wrap around week
    {{Clock::Month::Feb, 28, 0, 0}, {Clock::Month::Mar, 1, 1, 0}}, // advance from leap-year's Febrary, skipping days 30-31
    {{Clock::Month::Feb, 28, 6, 0}, {Clock::Month::Mar, 1, 0, 0}}, // advance from leap-year's February, skipping days 30-31, and wrap around week
    {{Clock::Month::Jun, 40, 0}, {Clock::Month::Jul, 1, 0}}, // seek forward to first valid day, crossing month boundary
    {{Clock::Month::Jun, 50, 0}, {Clock::Month::Jun, 1, 0}}, // seek forward to first valid day, not crossing month boundary
    {{Clock::Month::Jun, 1, 8}, {Clock::Month::Jun, 1, 6}}, // seek forward to first valid day, not crossing day boundary
    {{Clock::Month::Jun, 1, 29}, {Clock::Month::Jun, 2, 6}}, // seek forward to first valid day, crossing day boundary
};

TEST_P(TestClock, check)
{
    auto param = GetParam();
    auto clock = get<0>(param);
    enable = false;
    sensor = clock.sense(PullUp);
    wake();
    while(enable)
    {
        clock.tick();
        sensor = clock.sense(PullUp);
        tick();
    }
    cout << string(clock) << endl;
    ASSERT_EQ(clock, get<1>(param));
}

INSTANTIATE_TEST_SUITE_P(happlyFlow, TestClock, ::testing::ValuesIn(happyFlow));
