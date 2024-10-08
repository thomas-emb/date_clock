#include "clock.h"

#define seekFirst 0
#define seekWeek 1
#define seekMonth 2
#define seekWeekAfterMonth 3

uint16_t enable;
uint16_t monthPin;
Sensor sensor;
static uint16_t count;
static uint16_t state;

#define weekday 2
#define oneDay (30 * weekday)
#define advanceWeekday (1 * weekday)
#define loopbackWeek (7 * weekday)

void wake(void)
{
    enable = 1;
    monthPin = 1;
    count = oneDay - advanceWeekday;
    state = seekFirst;
}

void tick(void)
{
    count--;
    if (count == 0)
        if (sensor.d1_30 || sensor.d_29_31)
            enable = 0;
        else
        {
            if (state == seekFirst)
            {
                count += loopbackWeek;
                state = seekWeek;
            }
            else if (state == seekWeek)
            {
                count += 29 * oneDay - loopbackWeek; // revert week loopback in case we went over
                state = seekMonth;
            }
            else if (state == seekMonth)
            {
                count += loopbackWeek;
                state = seekWeekAfterMonth;
            }
        }
}