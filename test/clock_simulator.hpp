#pragma once

#include <array>
#include <string>

#include "../src/clock.h"

using namespace std;

enum Pin { Low, PullDown, HiZ, PullUp, High };
bool operator&(Pin left, Pin right);

class Clock {
public:
    enum Month {
        Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    };
    enum Hands { H, M, S, Y };

    Clock(uint8_t hour, uint8_t min, uint8_t sec, uint8_t year = 1);
    Clock(Month month, uint8_t day, uint8_t weekday, uint8_t year = 1);
    operator string() const;
    operator uint32_t() const;
    void tick();
    Sensor sense(const Pin monthPin) const;

    array<uint8_t, 4> hands;
};
