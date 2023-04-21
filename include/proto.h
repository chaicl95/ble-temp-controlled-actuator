#ifndef PROTO_H
#define PROTO_H

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <esp_log.h>
#include <inttypes.h>
#include <nvs_flash.h>
#include <driver/i2c.h>
#include <driver/mcpwm_prelude.h>
#include <host/ble_hs.h>
#include <host/ble_hs.h>
#include <host/ble_uuid.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <nimble/ble.h>
#include <services/gap/ble_svc_gap.h>
#include <services/ans/ble_svc_ans.h>
#include <services/gatt/ble_svc_gatt.h>
#include <services/gap/ble_svc_gap.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

// Product Variables
extern char *device_name;
extern float target_temp;
extern float curr_temp;
extern float curr_rh;
extern float percent_angle;

// BLE Variables
extern uint8_t blehr_addr_type;
extern uint16_t target_temp_val_handle;
extern uint16_t temp_val_handle;
extern uint16_t rh_val_handle;
extern uint16_t conn_handle;
extern bool notify_state;

// MCPWM Variables
extern mcpwm_timer_handle_t mcpwm_timer;
extern mcpwm_timer_config_t mcpwm_timer_config;
extern mcpwm_oper_handle_t mcpwm_oper;
extern mcpwm_operator_config_t mcpwm_oper_config;
extern mcpwm_cmpr_handle_t mcpwm_cmpr;
extern mcpwm_comparator_config_t mcpwm_cmpr_config;
extern mcpwm_gen_handle_t mcpwm_gen;
extern mcpwm_generator_config_t mcpwm_gen_config;


#endif