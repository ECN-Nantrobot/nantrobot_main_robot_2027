#include "esp_log.h"
#include "esp_err.h"

#include "board.h"
#include "motor.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "nantrobot firmware starting (status LED on GPIO %d)", BOARD_STATUS_LED_GPIO);
   

    // Init order (add each module here as it is written):
    // 1. nvs_flash / params
    // 2. motor, encoder
    // 3. drive_control
    // 4. imu, battery
    // 5. comms
    // Then start the FreeRTOS tasks (see the task table in README.md).
}

