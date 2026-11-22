#pragma once

// Pin map of the main robot ESP32-S3 board.
// Every GPIO number used by the firmware must be defined here, nowhere else.
//
// Pins to avoid on the ESP32-S3:
//   0, 3, 45, 46  strapping pins
//   19, 20        native USB (D-, D+)
//   26-37         SPI flash / PSRAM (depending on the module)

#include "driver/gpio.h"

// Status LED
#define BOARD_STATUS_LED_GPIO   GPIO_NUM_2

// Motors (TODO: fill in once the motor driver is chosen)
#define BOARD_MOTOR_RIGHT_GPIO GPIO_NUM_16
#define BOARD_MOTOR_RIGHT_DIR GPIO_NUM_17

#define BOARD_MOTOR_LEFT_GPIO GPIO_NUM_18
#define BOARD_MOTOR_LEFT_DIR GPIO_NUM_19



// IMU (TODO)

// Battery voltage divider, must be an ADC1 pin: GPIO 1-10, not 3 (TODO)
