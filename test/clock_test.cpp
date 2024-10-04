#include <gtest/gtest.h>

#include "../src/clock.h"
#include "clock_simulator.hpp"


class TestClock: public ::testing::TestWithParam<Clock>{};

vector<Clock> happyFlow = {
    {Clock::Month::Jan, 1, 0},
};

TEST_P(TestClock, check)
{
    auto clock = GetParam();
    read_sensors(clock.sense(PullUp));
}

INSTANTIATE_TEST_SUITE_P(happlyFlow, TestClock, ::testing::ValuesIn(happyFlow));
