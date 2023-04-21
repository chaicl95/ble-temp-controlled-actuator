#ifndef PROTO_H
#define PROTO_H

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <esp_log.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Product Variables
extern char *device_name;
extern float target_temp;
extern float curr_temp;
extern float curr_rh;
extern float percent_angle;


#endif