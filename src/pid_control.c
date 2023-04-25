#include <pid_control.h>

float pid_compute(pid_control_t *pid, float *current, float *target) {
    static float prev_error = 0;
    static float integral = 0;
    static float error = 0;

    float p,i,d,res;
    error = *target - *current;
    p = pid->proportional * error;
    integral += error * (pid->sampling_time_ms * 1000);
    if (integral >= INTEGRAL_MAXLIMIT) {
        integral = INTEGRAL_MAXLIMIT;
    }
    if (integral <= INTEGRAL_MINLIMIT) {
        integral = INTEGRAL_MINLIMIT;
    }
    i = pid->integrator * integral;
    d = pid->differentiator * (error - prev_error)/(pid->sampling_time_ms * 1000);
    prev_error = error;
    
    res = p + i + d;
    return res;
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

