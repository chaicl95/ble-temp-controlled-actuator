#include <proto.h>
#include <servo.h>
#include <htu21d.h>
#include <bleprph.h>
#include <pid_control.h>

#define I2C_MASTER_SCL_IO           22      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           21      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */

float target_temp = 18.0;
float curr_temp = NAN;
float curr_rh = NAN;
float percent_angle = 0.0;

esp_err_t i2c_init(void) {
    int i2c_master_port = 0;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, DISABLE, DISABLE, 0);
}

static void ble_on_sync(void) {
    int rc;

    rc = ble_hs_id_infer_auto(0, &blehr_addr_type);
    assert(rc == 0);

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(blehr_addr_type, addr_val, NULL);
    /* Begin advertising */
    ble_advertise();
}

static void ble_on_reset(int reason) {
    ESP_LOGE("BLE_EVT", "Resetting state; reason=%d\n", reason);
}

static void servo_cntrl_loop(void *pvParameter) {
    float output;
    int angle;
    pid_control_t pid = {
        // Ku = 10, Tu = 0.5
        .proportional   = 6.0,
        .integrator     = 24.0,
        .differentiator = 0.375,
        .lim_max = 100.0,
        .lim_min = 0.0,
        .sampling_time_ms = 500
    };
    while (true) {
        vTaskDelay(pid.sampling_time_ms / portTICK_PERIOD_MS);
        if (target_temp == NAN || curr_temp == NAN) {
            continue;
        } 
        output = pid_compute(&pid, &curr_temp, &target_temp);
        output = pid_process_output(&pid, output);
        percent_angle = output;
        angle = (int) output * SERVO_MAX_DEGREE / 100;
        ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(mcpwm_cmpr, angle_to_compare(angle)));
    }
}

static void env_sensing_loop(void *pvParameter) {
    float rd_temp; // Temporary variable for temperature
    float rd_rh;
    while (true) {
        rd_temp = htu21d_read_temp();
        if (rd_temp != NAN) {
            curr_temp = rd_temp;
        }
        rd_rh = htu21d_read_rh();
        if (rd_rh != NAN) {
            curr_rh = rd_rh;
        }
    }
}

static void ble_notify_loop(void *pvParameter) {
    struct os_mbuf *om;
    while(true) {
        int16_t conv_temp = (int16_t)(curr_temp / (1 * pow(10, -2) * pow(2, 0))); // R = C * M * 10 ^ d * 2 ^ b
        om = ble_hs_mbuf_from_flat(&conv_temp, sizeof(conv_temp));
        ble_gatts_notify_custom(conn_handle, temp_val_handle, om);
        conv_temp = (int16_t)(target_temp / (1 * pow(10, -2) * pow(2, 0))); // R = C * M * 10 ^ d * 2 ^ b
        om = ble_hs_mbuf_from_flat(&conv_temp, sizeof(conv_temp));
        ble_gatts_notify_custom(conn_handle, target_temp_val_handle, om);
        uint16_t conv_rh = (uint16_t)(curr_rh / (1 * pow(10, -2) * pow(2, 0)));
        om = ble_hs_mbuf_from_flat(&conv_rh, sizeof(conv_rh));
        ble_gatts_notify_custom(conn_handle, rh_val_handle, om);
        ESP_LOGI("SERVO", "Current Angle : %.2f", percent_angle);
        ESP_LOGI("SERVO", "Target Temperature : %.2f", target_temp);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void ble_host_task(void *param)
{
    ESP_LOGI("BLE_EVT", "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void app_main(void)
{
    int rc;
    // Set up I2C Driver
    ESP_ERROR_CHECK(i2c_init());
    
    // Set up NVS Partition
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    nimble_port_init();

    /* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = ble_on_sync;
    ble_hs_cfg.reset_cb = ble_on_reset;

    pwm_init();


    xTaskCreate(&env_sensing_loop, "env_sensing_loop", 2048, NULL, 5, NULL);
    xTaskCreate(&ble_notify_loop, "ble_notify_loop", 4096, NULL, 5, NULL);
    xTaskCreate(&servo_cntrl_loop, "servo_cntrl_loop", 2048, NULL, 5, NULL);

    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name */
    rc = ble_svc_gap_device_name_set(device_name);
    assert(rc == 0);
    
    /* Start the task */
    nimble_port_freertos_init(ble_host_task);

}