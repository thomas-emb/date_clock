#include <gtest/gtest.h>

#include "../src/clock.h"
#include "clock_simulator.hpp"


class TestClock: public ::testing::TestWithParam<tuple<Clock, Clock>>{};

vector<tuple<Clock, Clock>> happyFlow = {
    {{Clock::Month::Oct,  1, 0}, {Clock::Month::Oct, 2, 1}},
    {{Clock::Month::Oct,  1, 6}, {Clock::Month::Oct, 2, 0}},
    {{Clock::Month::Oct, 31, 0}, {Clock::Month::Nov, 1, 1}},
    {{Clock::Month::Oct, 31, 6}, {Clock::Month::Nov, 1, 0}},
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
