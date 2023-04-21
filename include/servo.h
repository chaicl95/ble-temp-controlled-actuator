#ifndef SERVO_H
#define SERVO_H
#include <proto.h>

#define SERVO_MIN_PULSEWIDTH_US     500
#define SERVO_MAX_PULSEWIDTH_US     2500
#define SERVO_MIN_DEGREE            0
#define SERVO_MAX_DEGREE            90

#define SERVO_PULSE_GPIO             0        // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20m

void pwm_init(void);
inline uint32_t angle_to_compare(int angle) {
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

#endif