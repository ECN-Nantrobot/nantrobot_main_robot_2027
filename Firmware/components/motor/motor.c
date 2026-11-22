#include "motor.h"
#include "driver/mcpwm_prelude.h"
#include "driver/pulse_cnt.h"
#include <stdio.h>

typedef struct {
    gpio_num_t step_gpio;
    gpio_num_t dir_gpio;
    uint16_t steps_per_rev;          
    motor_microstep_t microstep;
    bool invert_direction;   
    
    //pwm
    mcpwm_timer_handle_t current_step_hz ; 
    mcpwm_oper_handle_t operation ; // links timer to generator
    mcpwm_cmpr_handle_t comparator;
    mcpwm_gen_handle_t generator;


} motor_t;

esp_err_t motor_init(motor_config_t *motor, motor_handle_t *ret_motor)
{
   
   return ESP_OK;
}