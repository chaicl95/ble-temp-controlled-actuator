#ifndef  PID_H
#define PID_H

#include <proto.h>

typedef struct {
    float proportional;
    float integrator;
    float differentiator;
    float lim_max;
    float lim_min;
    float sampling_time;
} pid_control_t;

float pid_compute(pid_control_t *pid, float *current, float *target);
float pid_process_output(pid_control_t *pid, float output);

#endif