#ifndef BLEPRPH_H
#define BLEPRPH_H

#include <proto.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <host/ble_hs.h>
#include <host/ble_hs.h>
#include <host/ble_uuid.h>
#include <nimble/ble.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <services/gap/ble_svc_gap.h>
#include <services/ans/ble_svc_ans.h>
#include <services/gatt/ble_svc_gatt.h>
#include <services/gap/ble_svc_gap.h>


/* Heart-rate configuration */
#define GATT_SVC_ENVS_UUID                          0x181A
#define GATT_SVC_AUTOIO_UUID                        0x1815 
#define GATT_UNIT_CELSIUS_UUID                      0x272F
#define GATT_UNIT_PERC_UUID                         0x27AD
#define GATT_CHR_TEMP_UUID                          0x2A6E
#define GATT_CHR_RH_UUID                            0x2A6F
#define GATT_CHR_PERC_UUID                          0x2B04
#define GATT_DESC_ENVS_MEAS_UUID                    0x290C
#define GATT_DEVICE_INFO_UUID                       0x180A
#define GATT_MANUFACTURER_NAME_UUID                 0x2A29
#define GATT_MODEL_NUMBER_UUID                      0x2A24

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;
extern uint8_t blehr_addr_type;
extern uint16_t target_temp_val_handle;
extern uint16_t temp_val_handle;
extern uint16_t rh_val_handle;
extern uint16_t conn_handle;
extern bool notify_state;

int ble_gap_event_func(struct ble_gap_event *event, void *arg);
void ble_advertise(void);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);
int gatt_svr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len);

#endif