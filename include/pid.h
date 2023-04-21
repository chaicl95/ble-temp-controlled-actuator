#include <proto.h>

typedef struct {
    double proportional;
    double integrator;
    double differentiator;
    double lim_max;
    double lim_min;
} pid_control_t;

float pid_compute(pid_control_t *pid, float *current, float *target);
float pid_process_output(pid_control_t *pid, float output);
