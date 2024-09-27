#ifndef __CLOCK_H__
#define __CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef uint16_t
#define uint16_t unsigned short
#endif

typedef struct _Sensor {
    uint16_t d1_30 : 1;
    uint16_t d_29_31: 1;
    uint16_t m : 1;
    uint16_t y : 1;
} Sensor;

void read_sensors(Sensor values);

#ifdef __cplusplus
}
#endif

#endif