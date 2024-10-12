#include "clock.h"

enum {
    seekFirst = 0,
    seekWeek = 2,
    seekMonth = 4,
    seekWeekAfterMonth = 6,
    seekForward = 8,
    seekStates = 8
} State;

uint16_t enable;
uint16_t monthPin;
Sensor sensor;
static uint16_t count;
static enum State state;

#ifndef __even_in_range
#include <assert.h>
#define __even_in_range(val, max) (assert(!(((val) & 1) || (val) > max)), (val))
#endif

#define weekday 2
#define oneDay (30 * weekday)
#define advanceWeekday (1 * weekday)
#define loopbackWeek (7 * weekday)

void wake(void)
{
    enable = 1;
    monthPin = 1;
    count = oneDay - advanceWeekday;
    if (sensor.d1_30 || sensor.d_29_31)
        state = seekFirst;
    else
        state = seekForward;
}

void tick(void)
{
    count--;
    if (count == 0)
        if (sensor.d1_30 || sensor.d_29_31)
            if (!sensor.d1_30 && !sensor.m)
                count = oneDay;
            else
                enable = 0;
        else
        switch(__even_in_range(state, seekStates))
        {
            case seekFirst:
                count += loopbackWeek;
                state = seekWeek;
                break;
            case seekWeek:
                count += 29 * oneDay - loopbackWeek; // revert week loopback in case we went over
                state = seekMonth;
                break;
            case seekMonth:
                count += loopbackWeek;
                state = seekWeekAfterMonth;
                break;
        }
}