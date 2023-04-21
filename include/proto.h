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
#include <host/ble_hs.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include "nimble/ble.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "services/ans/ble_svc_ans.h"
#include "services/gap/ble_svc_gap.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

extern char *device_name;
extern float target_temp;
extern float curr_temp;
extern float curr_rh;
extern uint8_t blehr_addr_type;
extern uint16_t target_temp_val_handle;
extern uint16_t temp_val_handle;
extern uint16_t rh_val_handle;
extern uint16_t conn_handle;
extern bool notify_state;

#endif