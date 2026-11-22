#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"

// Microstepping set on the A4988 MS1-MS3 pins (1/16 maximum).
typedef enum {
    MOTOR_MICROSTEP_FULL      = 1,
    MOTOR_MICROSTEP_HALF      = 2,
    MOTOR_MICROSTEP_QUARTER   = 4,
    MOTOR_MICROSTEP_EIGHTH    = 8,
    MOTOR_MICROSTEP_SIXTEENTH = 16,
} motor_microstep_t;

// Filled in by the caller, read once by motor_new().

typedef struct {
    gpio_num_t step_gpio;
    gpio_num_t dir_gpio;
    uint16_t steps_per_rev;          
    motor_microstep_t microstep;
    bool invert_direction;           // true for the mirrored motor
} motor_config_t;


typedef struct motor_t *motor_handle_t;

// Create one motor. Returns ESP_OK and writes the new handle to *ret_motor.
esp_err_t motor_new(const motor_config_t *config, motor_handle_t *ret_motor);


esp_err_t motor_set_speed(motor_handle_t motor, int32_t steps_per_s);


esp_err_t motor_get_position(motor_handle_t motor, int32_t *ret_position);
