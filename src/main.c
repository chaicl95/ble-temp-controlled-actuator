#include <stdio.h>
#include <htu21d.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"

void app_main(void)
{
    float temp = htu21d_read_temp();
    printf("Temperature : %2.1f\n", temp);

    /* Print chip information */

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}