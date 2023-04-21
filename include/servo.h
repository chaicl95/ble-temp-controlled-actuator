#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <driver/mcpwm_prelude.h>

#define SERVO_MIN_PULSEWIDTH_US     500
#define SERVO_MAX_PULSEWIDTH_US     2500
#define SERVO_MIN_DEGREE            0
#define SERVO_MAX_DEGREE            90

#define SERVO_PULSE_GPIO             0        // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD        20000    // 20000 ticks, 20m

extern mcpwm_timer_handle_t mcpwm_timer;
extern mcpwm_timer_config_t mcpwm_timer_config;
extern mcpwm_oper_handle_t mcpwm_oper;
extern mcpwm_operator_config_t mcpwm_oper_config;
extern mcpwm_cmpr_handle_t mcpwm_cmpr;
extern mcpwm_comparator_config_t mcpwm_cmpr_config;
extern mcpwm_gen_handle_t mcpwm_gen;
extern mcpwm_generator_config_t mcpwm_gen_config;

void pwm_init(void);
inline uint32_t angle_to_compare(int angle) {
    return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

#endif