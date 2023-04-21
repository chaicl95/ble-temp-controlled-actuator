#ifndef BLEPRPH_H
#define BLEPRPH_H

#include <proto.h>

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

int ble_gap_event_func(struct ble_gap_event *event, void *arg);
void ble_advertise(void);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
int gatt_svr_init(void);

#endif