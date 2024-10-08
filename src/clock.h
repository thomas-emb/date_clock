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

extern uint16_t enable;
extern uint16_t monthPin;
extern Sensor sensor;

void wake(void);
void tick(void);

#ifdef __cplusplus
}
#endif

#endif