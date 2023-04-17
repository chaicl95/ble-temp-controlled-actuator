#ifndef HTU21D_H
#define HTU21D_H

#include <stdint.h>
#include <stdbool.h>

#define HTU21D_I2C_ADDRESS                      0x40

#define HTU21D_TEMP_MEASURE_HOLD                0xe3
#define HTU21D_RH_MEASURE_HOLD                  0xe5
#define HTU21D_TEMP_MEASURE_NO_HOLD             0xf3
#define HTU21D_RH_MEASURE_NO_HOLD               0xf5
#define HTU21D_WR_REG                           0xe6
#define HTU21D_RD_REG                           0xe7
#define HTU21D_SOFT_RESET                       0xfe

#define HTU21D_RESET_TIMEOUT    50
#define HTU21D_MEASURE_TIMEOUT  100
#define HTU21D_CONFIG_TIMEOUT   200

/* User Register
Bit 0,7   : Measurement Resolution
    |Bit 7 | Bit 0 |RH      |Temp   |
    ================================
    |0     | 0     |12 bits |14 bits|
    |0     | 1     |8 bits  |12 bits|
    |1     | 0     |10 bits |13 bits|
    |1     | 1     |11 bits |11 bits|
Bit 6     : End of Battery
    0 : VDD>2.25V
    1 : VDD<2.25V
Bit 3,4,5 : Reserved
Bit 2     : On-chip heater - for Testing
Bit 1     : Disable OTP reload (not recommended, use soft reset instead)
*/

typedef enum {
    RH_12_TEMP_14 = 0x00,
    RH_8_TEMP_12  = 0x01,
    RH_10_TEMP_13 = 0x80,
    RH_11_TEMP_11 = 0x81
} htu21d_measure_res_t;

typedef enum {
    VDD_GT_2_25V = 0x0,
    VDD_LT_2_25V = 0x1
} htu21d_battery_stat_t;

typedef enum {
    DISABLE = 0x0,
    ENABLE =  0x1,
} htu21d_onchip_heat_t;


void htu21d_reset();
float htu21d_read_rh();
float htu21d_read_temp();
uint8_t htu21d_read_resolution();
uint8_t htu21d_read_heater();
uint8_t htu21d_set_resolution(htu21d_measure_res_t res);
uint8_t htu21d_set_heater(bool enable);



#endif