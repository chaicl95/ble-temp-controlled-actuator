#include <math.h>
#include <htu21d.h>
#include <driver/i2c.h>

void htu21d_reset() {
    uint8_t wr_buf = HTU21D_SOFT_RESET;
    i2c_master_write_to_device(
        I2C_NUM_0, // Assume PORT 0 first
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_RESET_TIMEOUT / portTICK_PERIOD_MS
    );
    return;
}

float htu21d_read_rh() {
    uint8_t rd_buf[3];
    uint8_t wr_buf = HTU21D_RH_MEASURE_HOLD; 
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_MEASURE_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return NAN;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        rd_buf,
        sizeof(rd_buf),
        HTU21D_MEASURE_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return NAN;

    uint16_t raw_rh = rd_buf[0] << 8;
    raw_rh |= rd_buf[1];
    raw_rh ^= 0x02;
    float rh = raw_rh * 0.001907 - 6;
    return rh;
}

float htu21d_read_temp() {
    uint8_t rd_buf[3];
    uint8_t wr_buf = HTU21D_TEMP_MEASURE_HOLD; 
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_MEASURE_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return NAN;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        rd_buf,
        sizeof(rd_buf),
        HTU21D_MEASURE_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return NAN;

    uint16_t raw_temp = rd_buf[0] << 8;
    raw_temp |= rd_buf[1];
    raw_temp ^= 0x02;
    float temp = raw_temp * 0.00268 - 46.85;
    return temp;
}

uint8_t htu21d_read_resolution() {
    uint8_t rd_buf;
    uint8_t wr_buf = HTU21D_RD_REG;
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &rd_buf,
        sizeof(rd_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    uint8_t res = rd_buf & 0x81;
    return res;
}

uint8_t htu21d_read_heater() {
    uint8_t rd_buf;
    uint8_t wr_buf = HTU21D_RD_REG;
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &rd_buf,
        sizeof(rd_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    uint8_t res = (rd_buf & 0x02) >> 1;
    return res;
}

uint8_t htu21d_set_resolution(htu21d_measure_res_t res) {
    uint8_t rd_buf;
    uint8_t wr_buf = HTU21D_RD_REG;
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &rd_buf,
        sizeof(rd_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    rd_buf = (rd_buf & 0x7e) | res;
    uint8_t wr_buf2[2] = {HTU21D_WR_REG, rd_buf};

    ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        wr_buf2,
        sizeof(wr_buf2),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    return res;
}
uint8_t htu21d_set_heater(bool enable) {
    uint8_t rd_buf;
    uint8_t wr_buf = HTU21D_RD_REG;
    esp_err_t ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &wr_buf,
        sizeof(wr_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;
    
    ret = i2c_master_read_from_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        &rd_buf,
        sizeof(rd_buf),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    rd_buf = (rd_buf & 0xfd) | ((uint8_t)enable << 1);
    uint8_t wr_buf2[2] = {HTU21D_WR_REG, rd_buf};

    ret = i2c_master_write_to_device(
        I2C_NUM_0,
        HTU21D_I2C_ADDRESS,
        wr_buf2,
        sizeof(wr_buf2),
        HTU21D_CONFIG_TIMEOUT / portTICK_PERIOD_MS
    );
    if (ret != ESP_OK) return 0xff;

    return (uint8_t)enable;
}