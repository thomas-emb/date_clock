#include <gtest/gtest.h>

#include "clock_simulator.hpp"

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