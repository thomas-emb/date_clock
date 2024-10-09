#include <algorithm>
#include <assert.h>
#include <vector>

#include "clock_simulator.hpp"

bool operator&(Pin left, Pin right)
{
    if ((left == High && right == Low) || (left == Low && right == High) || (left == HiZ && right == HiZ))
        throw exception();
    if (left == High || right == High) return true;
    if (left == Low || right == Low) return false;
    if ((left == PullDown && right == PullUp) || (left == PullUp && right == PullDown))
        throw exception();
    if (left == PullUp || right == PullUp) return true;
    if (left == PullDown || right == PullDown) return false;
    throw exception();
}

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
    static const array<string, 12> MonthString {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    return
        to_string(((hands[M] + 44) % 60) - 28) + 
        " " + MonthString[(hands[H] + 5) % 12] + 
        ", day" + to_string((36 - hands[S])/2) + ((hands[S] % 2) ? ".5" : "") +
        (hands[Y]?" non-leap":" leap");
}

void Clock::tick()
{
    hands[S] = (hands[S] + 1) % 60;
    if (hands[S] == 0)
    {
        hands[M] = (hands[M] + 1) % 60;
        if (hands[M] == 30) // note: the hour hand [2] indicates closest hour, not digital time hour
        {
            hands[H] = (hands[H] + 1) % 12;
            if (hands[H] == 0)
                hands[Y] = (hands[Y] + 1) % 4;
        }
    }
}

Sensor Clock::sense(const Pin monthPin) const
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
    bool weekday = std::find(weekdays.begin(), weekdays.end(), hands[S]) != weekdays.end();
    result.d1_30 = weekday && (hands[M] >= 45 || hands[M] <= 14);
    result.d_29_31 = weekday && (hands[M] >= 13 && hands[M] <= 15);
    result.m = monthPin & vector<Pin>{Low, High, High, Low, High, Low, High, High, HiZ, High, Low, High}[hands[H]];
    result.y = hands[Y] == 0;
    return result;
}

bool operator==(const Clock& left, const Clock& right)
{
    return left.hands == right.hands;
}

std::ostream& operator<<(std::ostream& stream, const Clock& clock)
{
    return stream << "["
        << int(clock.hands[0]) << ", "
        << int(clock.hands[1]) << ", "
        << int(clock.hands[2]) << ", "
        << int(clock.hands[3]) << "]";
}