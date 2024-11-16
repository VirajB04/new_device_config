/*
 * button.h
 *
 *  Created on: 29-Aug-2024
 *      Author: HP
 */

#ifndef MAIN_INCLUDE_BUTTON_H_
#define MAIN_INCLUDE_BUTTON_H_
#include "driver/spi_master.h"
#include "lvgl.h"

#define BOOT_BUTTON_NUM         13
#define BUTTON_ACTIVE_LEVEL     0
#define LONG_PRESS_DURATION 1000

void button_init(uint32_t button_num);
uint8_t read_button(void);




#endif /* MAIN_INCLUDE_BUTTON_H_ */
