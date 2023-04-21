#include <pid_control.h>

float pid_compute(pid_control_t *pid, float *current, float *target) {
    static float prev_error = 0;
    static float integral = 0;
    float p,i,d;
    float error = *target - *current;

    p = pid->proportional * error;
    integral += error * (pid->sampling_time_ms * 1000);
    i = pid->integrator * integral;
    d = pid->differentiator * (error - prev_error)/(pid->sampling_time_ms * 1000);
    prev_error = error;
    
    return p + i + d;
}

float pid_process_output(pid_control_t *pid, float output) {
    if (output > pid->lim_max) {
        return pid->lim_max;
    }
    if (output< pid->lim_min) {
        return pid->lim_min;
    }
    return output;
}

