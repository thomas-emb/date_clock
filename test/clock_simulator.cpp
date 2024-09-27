#include <algorithm>
#include <assert.h>
#include <vector>

#include "clock_simulator.hpp"

Clock::Clock(uint8_t hour, uint8_t min, uint8_t sec, uint8_t year)
: hands{hour, min, sec, year}
{

}

Clock::Clock(Month month, uint8_t day, uint8_t weekday, uint8_t year)
: hands{uint8_t((month + 7) % 12), uint8_t((day + 44) % 60), uint8_t(36 - (weekday * 2)), year}
{

}

Clock::operator string() const
{
    return
        to_string(((hands[M] + 44) % 60) - 28) + 
        " " + MonthStr[(hands[H] + 4) % 12] + 
        ", day" + to_string((36 - hands[S])/2) + ((hands[S] % 2) ? ".5" : "") +
        (hands[Y]?" non-leap":" leap");
}

Clock::operator uint32_t() const
{
    return hands[H] * 0x1000000 + hands[M] * 0x10000 + hands[S] * 0x100 + hands[Y];
}

void Clock::tick()
{
    hands[S] = (hands[S] + 1) % 60;
    if (hands[S] == 0)
        hands[M] = (hands[M] + 1) % 60;
    if (hands[M] == 0)
        hands[H] = (hands[H] + 1) % 12;
    if (hands[H] == 0)
        hands[Y] = (hands[Y] + 1) % 4;
}

bool operator&(Pin left, Pin right)
{
    //  left | L L L Z Z Z H H H
    // right | L Z H L Z H L Z H
    // ------+-------------------
    //   out | L L X L X H X H H  L&H is a short circuit, Z&Z is undefined
    if ((left == PullDown && right != PullUp) || (left != PullUp && right == PullDown)) return false;
    if ((left == PullUp && right != PullDown) || (left != PullDown && right == PullUp)) return true;
    throw exception();
}

Sensor sense(const Clock& clock, const Pin monthPin)
{   /*
     * Encoder for day-of-month uses two contacts connected to a third contact that is on the week encoder,
     * the week encoder is closed on valid day of week positions, shorting to ground (false),
     * the day-of-month contacts are closed respectively on day 1-30 of the month and 29-31 of the month.
     * The month encoder is contacting to ground (false) for 31 day months, pulled up (true) for 30 day months,
     * and floating (exception) for February, the monthPin is logically combined with this pin;
     * February can be probed for by toggling the sense pin between pull-up and pull-down.
     * The year encoder is shorted to ground (false) for leap years.
     */
    Sensor result;
    vector<uint8_t> weekdays{24, 26, 28, 30, 32, 34, 36};
    bool weekday = find(weekdays.begin(), weekdays.end(),clock.hands[Clock::S]) != weekdays.end();
    result.d1_30 = weekday && (clock.hands[Clock::M] >= 45 || clock.hands[Clock::M] <= 15);
    result.d_29_31 = weekday && (clock.hands[Clock::M] >= 14 && clock.hands[Clock::M] <= 16);
    result.m = monthPin & vector<Pin>{PullDown, PullUp, PullUp, PullDown, PullUp, PullDown, PullUp, PullUp, HiZ, PullUp, PullDown, PullUp}[clock.hands[Clock::H]];
    result.y = clock.hands[Clock::Y] == 0;
    return result;
}