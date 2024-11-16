/*
 * button.c
 *
 *  Created on: 29-Aug-2024
 *      Author: HP
 */
 
 /*
 This code is used to control basic button configurations
 but the one which focuses and selects that function is in spi oled main
 */
 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "iot_button.h"
#include "esp_sleep.h"
#include "esp_idf_version.h"
#include "esp_timer.h"
#include "button.h"

//Earlier used for logging statements in e=button_event_cb
//static const char *BUT = "BTN_TASK";


//
const char *button_event_table[] = {
    "BUTTON_SINGLE_CLICK",
    "BUTTON_LONG_PRESS_HOLD",
};
static void button_event_cb(void *arg, void *data)
{
}
void button_init(uint32_t button_num)
{
    button_config_t btn_cfg = {
        .type = BUTTON_TYPE_GPIO,
        .gpio_button_config = {
            .gpio_num = button_num,
            .active_level = BUTTON_ACTIVE_LEVEL,
        },
    };
    button_handle_t btn = iot_button_create(&btn_cfg);
    assert(btn);
    esp_err_t err =  iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, button_event_cb, (void *)BUTTON_SINGLE_CLICK);
    err |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_HOLD, button_event_cb, (void *)BUTTON_LONG_PRESS_HOLD);
    
    ESP_ERROR_CHECK(err);
}
/**
 * @fn    uint8_t read_button(void)
 * @brief It sets how the button is pressed and if it's not long press then 
          it is single click by noticing timing.
 * @pre
 * @post
 * @param  static uint32_t press_start_time, static bool long_press_detected
           uint8_t button_state
 * @return 
 */
uint8_t read_button(void) {
    static uint32_t press_start_time = 0;
    static bool long_press_detected = false;

    uint8_t button_state = gpio_get_level(BOOT_BUTTON_NUM);

    if (button_state == BUTTON_ACTIVE_LEVEL) {
        if (press_start_time == 0) {
            // Button just pressed, record the time
            press_start_time = esp_timer_get_time() / 1000;  // Convert microseconds to milliseconds
            long_press_detected = false;
        } else {
            uint32_t current_time = esp_timer_get_time() / 1000;
            if (!long_press_detected && (current_time - press_start_time) >= LONG_PRESS_DURATION) {
                // Long press detected
                long_press_detected = true;
                return BUTTON_LONG_PRESS_HOLD;
            }
        }
    } else {
        // Button released, reset timer
        if (press_start_time > 0 && !long_press_detected) {
            uint32_t press_duration = (esp_timer_get_time() / 1000) - press_start_time;
            if (press_duration < LONG_PRESS_DURATION) {
                press_start_time = 0;
                return BUTTON_SINGLE_CLICK;
            }
        }
        press_start_time = 0;
        long_press_detected = false;
    }
    return 0; // No button pressed
}






