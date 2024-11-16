#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "core/lv_obj.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_scroll.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_tree.h"
#include "esp_task_wdt.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "iot_button.h"
#include "esp_sleep.h"
#include "esp_idf_version.h"
#include "core/lv_disp.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "lv_api_map.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "SSD1306.h"
#include "demos/lv_demos.h"
#include "misc/lv_anim.h"
#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "qrcode.h"
#include "lv_examples.h"
#include "widgets/lv_label.h"
#include <inttypes.h>
#include<button.h>
#include<textbox.h>


static const char *TAG = "example";

#define I2C_HOST  0
#define LCD_SPI_HOST SPI3_HOST

#define GPIO_MOSI  23  // OLED Data Out
#define GPIO_RESET 32  // OLED Resolution
#define GPIO_DC    27  // Data/command identification
#define GPIO_MISO  19
#define GPIO_SCLK  18  // OLED Clock
#define GPIO_CS     5

#define EXAMPLE_LCD_H_RES  128
#define EXAMPLE_LCD_V_RES  64

static SSD1306_t dev;
static lv_obj_t *menu = NULL;  // Declare menu as a global object to access it in the button handler
static lv_obj_t *focused_btn = NULL; // Global pointer to track the focused button
//static lv_obj_t *currentButton = NULL;
static lv_obj_t *button_list[15]; // Array to keep track of buttons in the list
static int focused_btn_idx = 0;
static int button_count = 0; // Number of buttons in the list
bool button_text_bool = false;


void lv_port_disp_init(void);
void create_menu(void);
void button_text(void);
void display_task(void *pvParameters);

static lv_obj_t *current_page = NULL;

static lv_obj_t *page_history[10]; // Stack to keep track of page history
static int page_history_index = -1;
int last_selected_level = 0;

typedef struct lvl_name
{
    char *str1;
    char *str2;
    char *str3;
    char *str4;
    char *str5;
    char *str6;
    char *str7;
    char *str8;
    char *str9;
    char *str10;

}levl_one;

typedef struct brgt_lvl_name
{
    int lvl_1;
    int lvl_2;
    int lvl_3;
    int lvl_4;
    int lvl_5;
    int lvl_6;
    int lvl_7;
    int lvl_8;
    int lvl_9;
    int lvl_10;
}brightness;

levl_one l1;
brightness brgt_lvl;

void level_name_set()
      {
l1.str1 = (brgt_lvl.lvl_1 == 0) ? "10": "10<-";
l1.str2 = (brgt_lvl.lvl_2 == 0) ? "20": "20<-";
l1.str3 = (brgt_lvl.lvl_3 == 0) ? "30": "30<-";
l1.str4 = (brgt_lvl.lvl_4 == 0) ? "40": "40<-";
l1.str5 = (brgt_lvl.lvl_5 == 0) ? "50": "50<-";
l1.str6 = (brgt_lvl.lvl_6 == 0) ? "60": "60<-";
l1.str7 = (brgt_lvl.lvl_7 == 0) ? "70": "70<-";
l1.str8 = (brgt_lvl.lvl_8 == 0) ? "80": "80<-";
l1.str9 = (brgt_lvl.lvl_9 == 0) ? "90": "90<-";
l1.str10 = (brgt_lvl.lvl_10 == 0) ? "100": "100<-";
}

void brightness_level(int selected_level) {
	//current_selected_option = selected_level;
	/*
    brgt_lvl.lvl_1 = (selected_level == 1) ? 1 : 0;
    brgt_lvl.lvl_2 = (selected_level == 2) ? 1 : 0;
    brgt_lvl.lvl_3 = (selected_level == 3) ? 0 : 1;
    brgt_lvl.lvl_4 = (selected_level == 4) ? 1 : 0;
    brgt_lvl.lvl_5 = (selected_level == 5) ? 1 : 0;
    brgt_lvl.lvl_6 = (selected_level == 6) ? 1 : 0;
    brgt_lvl.lvl_7 = (selected_level == 7) ? 1 : 0;
    brgt_lvl.lvl_8 = (selected_level == 8) ? 1 : 0;
    brgt_lvl.lvl_9 = (selected_level == 9) ? 1 : 0;
    brgt_lvl.lvl_10 =(selected_level == 10)? 1 : 0;
   */
    brgt_lvl.lvl_1 = brgt_lvl.lvl_2 = brgt_lvl.lvl_4 =
    brgt_lvl.lvl_5 = brgt_lvl.lvl_6 = brgt_lvl.lvl_7 = 
    brgt_lvl.lvl_8 = brgt_lvl.lvl_9 = brgt_lvl.lvl_10 =
    brgt_lvl.lvl_3 = 0;
    
    switch(selected_level){
		case 1: brgt_lvl.lvl_1 = 1; break;
        case 2: brgt_lvl.lvl_2 = 1; break;
        case 3: brgt_lvl.lvl_3 = 1; break;
        case 4: brgt_lvl.lvl_4 = 1; break;
        case 5: brgt_lvl.lvl_5 = 1; break;
        case 6: brgt_lvl.lvl_6 = 1; break;
        case 7: brgt_lvl.lvl_7 = 1; break;
        case 8: brgt_lvl.lvl_8 = 1; break;
        case 9: brgt_lvl.lvl_9 = 1; break;
        case 10: brgt_lvl.lvl_10 = 1; break;
        default: break;
	}
	last_selected_level = selected_level;
	level_name_set();
}

char *list[10] = {0};

/**
 * @fn static void back_button_handler(lv_event_t *e)
 * @brief when " < Back" button is pressed this function gets called and execute
          it's working.
 * @pre
 * @post
 * @param lv_event_t *e
 * @return last accessed page
 */
static void back_button_handler(lv_event_t *e) {
	   xTaskCreatePinnedToCore(display_task, "display_task", 8096, NULL, 2, NULL, 0);
	   focused_btn = NULL; 
    button_count = 0;
}

/**
 * @fn static void menu_button_handler(lv_event_t *e)
 * @brief when "Menu > " button is pressed this function gets called and execute
          it's working.
 * @pre
 * @post
 * @param lv_event_t *e
 * @return menu page/ page 1 gets popped
 */
 
static void menu_button_handler(lv_event_t *e) {
    //lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
    focused_btn = NULL; 
    button_count = 0;
    create_menu();
}
/**
 * @fn static void create_subpage(const char *title, const char *items[], int item_count)
 * @brief Function for creating 2nd pages for each option in menu page
          (creates each sub page for Display Configuration, Button configuration...etc)
 * @pre
 * @post
 * @param const char *title, const char *items[], int item_count
 * @return sub_page/ 2nd page is created
 */
static void create_subpage(const char *title, const char *items[], int item_count) {
	//push_page_to_history(current_page);
    lv_obj_t *subpage_scr = lv_obj_create(NULL);
    //push_page_to_history(current_page);
    
    //You can say heading of page, the top bar.
    lv_obj_t *top_bar = lv_obj_create(subpage_scr);
    lv_obj_set_size(top_bar, EXAMPLE_LCD_H_RES, 14);
    lv_obj_set_style_bg_color(top_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_COVER,LV_PART_MAIN);
    
    lv_obj_t *top_label = lv_label_create(top_bar);
    lv_label_set_text(top_label, title);
    lv_obj_set_style_text_color(top_label,lv_color_black() , LV_PART_MAIN);
    lv_obj_align(top_label, LV_ALIGN_CENTER, 0, 0);
    
    //list items for sub pages where we can see options like in Display config. Timer settings and brightness
    lv_obj_t *list = lv_list_create(subpage_scr);
    lv_obj_set_size(list, EXAMPLE_LCD_H_RES, 36);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 14);
    lv_obj_set_style_text_color(list,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(list,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER,LV_PART_MAIN);

    button_count = 0;  // Reset button count for the new page
    for (int i = 0; i < item_count; i++) {
        button_list[button_count++] = lv_list_add_btn(list, NULL, items[i]);
    }
    
    //Bottom container of page consisiting of <back button and menu> button followed by buttons declarations
    lv_obj_t *bottom_bar = lv_obj_create(subpage_scr);
    lv_obj_set_size(bottom_bar, EXAMPLE_LCD_H_RES, 14);
    lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(bottom_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(bottom_bar,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_COVER,LV_PART_MAIN);

    lv_obj_t *back_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(back_btn, 52, 14);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(back_btn, back_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, " < Back");
    lv_obj_center(back_label);
    button_list[button_count++] = back_btn;

    lv_obj_t *menu_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(menu_btn, 56, 14);
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(menu_btn, menu_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *menu_label_btn = lv_label_create(menu_btn);
    lv_label_set_text(menu_label_btn, "Menu > ");
    lv_obj_center(menu_label_btn);
    button_list[button_count++] = menu_btn;
 
    //Used for the border over the selected option, below is logic and its style
    focused_btn_idx = 0;
    focused_btn = button_list[focused_btn_idx];
    lv_obj_set_style_border_color(focused_btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_width(focused_btn, 2, LV_PART_MAIN);
    current_page = subpage_scr;
    lv_scr_load_anim(subpage_scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
    //push_page_to_history(current_page);
}

/**
 * @fn static void create_deeper_subpage(const char *title, const char *items[], int item_count)
 * @brief Function for creating 3rd page for each option in subpage of each subpage
          (create each sub page for sub page like in display 
          Configuration sub pages for timer settings consisting of ID & QR timer)
 * @pre
 * @post
 * @param const char *title, const char *items[], int item_count
 * @return deeper_subpage/ 3rd page for each option of subpage is created
 */
static void create_deeper_subpage(char *title, char *items[], int item_count) {
	//push_page_to_history(current_page);
    lv_obj_t *deeper_subpage_scr = lv_obj_create(NULL);
    //push_page_to_history(current_page);

    lv_obj_t *top_bar = lv_obj_create(deeper_subpage_scr);
    lv_obj_set_size(top_bar, EXAMPLE_LCD_H_RES, 14);
    lv_obj_set_style_bg_color(top_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(top_bar, LV_OPA_COVER,LV_PART_MAIN);
    
    lv_obj_t *top_label = lv_label_create(top_bar);
    lv_label_set_text(top_label, title);
    lv_obj_set_style_text_color(top_label,lv_color_black() , LV_PART_MAIN);
    lv_obj_align(top_label, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *list = lv_list_create(deeper_subpage_scr);
    lv_obj_set_size(list, EXAMPLE_LCD_H_RES, 36);
    lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 14);
    lv_obj_set_style_text_color(list,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(list,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER,LV_PART_MAIN);

    button_count = 0;  // Reset button count for this deeper subpage
    for (int i = 0; i < item_count; i++) {
        button_list[button_count++] = lv_list_add_btn(list, NULL, items[i]);
    }

    lv_obj_t *bottom_bar = lv_obj_create(deeper_subpage_scr);
    lv_obj_set_size(bottom_bar, EXAMPLE_LCD_H_RES, 14);
    lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(bottom_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(bottom_bar,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_COVER,LV_PART_MAIN);

    lv_obj_t *back_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(back_btn, 52, 14);
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(back_btn, back_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, " < Back");
    lv_obj_center(back_label);
    button_list[button_count++] = back_btn;

    lv_obj_t *menu_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(menu_btn, 56, 14);
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    //lv_obj_add_event_cb(menu_btn, menu_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *menu_label_btn = lv_label_create(menu_btn);
    lv_label_set_text(menu_label_btn, "Menu > ");
    lv_obj_center(menu_label_btn);
    button_list[button_count++] = menu_btn;

    focused_btn_idx = 0;
    focused_btn = button_list[focused_btn_idx];
    lv_obj_set_style_border_color(focused_btn, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_border_width(focused_btn, 2, LV_PART_MAIN);
    current_page = deeper_subpage_scr;
    lv_scr_load_anim(deeper_subpage_scr, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
    //push_page_to_history(current_page);
}
/**
 * @fn    void create_page_for_menu(lv_obj_t *parent)
 * @brief This function creates a container on menupage where our buttons are displayed just like list
          buttons like Display config, butotn config...
 * @pre
 * @post
 * @param lv_obj_t *parent
 * @return 
 */
void create_page_for_menu(lv_obj_t *parent) {
     // Create a container for the menu items
    lv_obj_t *menu_container = lv_obj_create(parent);
    lv_obj_set_size(menu_container, EXAMPLE_LCD_H_RES, 34); // Adjust the size to match your screen resolution
    lv_obj_align(menu_container, LV_ALIGN_TOP_LEFT, 0, 15); // Positioned below the menu bar 
    lv_obj_set_style_text_color(menu_container,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(menu_container, lv_color_black(),LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_container, LV_OPA_COVER,LV_PART_MAIN);
    // Add buttons to the container instead of using lv_list_add_btn
    lv_obj_t *btn;
    
    btn = lv_btn_create(menu_container); // Display Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "Display Config");
    button_list[button_count++] = btn;
    
    btn = lv_btn_create(menu_container); // Button Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 19);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Button Config");
    button_list[button_count++] = btn;

    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 38);
    label = lv_label_create(btn);
    lv_label_set_text(label, "RGB Config");
    button_list[button_count++] = btn;
    
    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 57);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Address Config");
    button_list[button_count++] = btn;
    
    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 18);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 76);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Health state");
    button_list[button_count++] = btn;
    
    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 95);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Feature Config");
    button_list[button_count++] = btn; 
    
    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 114);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Prox Sensor");
    button_list[button_count++] = btn;    
    
    btn = lv_btn_create(menu_container); // RGB Config Button
    lv_obj_set_size(btn, EXAMPLE_LCD_H_RES, 19);
    lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 0, 133);
    label = lv_label_create(btn);
    lv_label_set_text(label, "Flap door Config");
    button_list[button_count++] = btn;   
}
/**
 * @fn void create_menu(void)
 * @brief Displays 1st basic page/ menu page consisting of items like menu button, back button
          and list with display config, button config, etc.
 * @pre
 * @post
 * @param 
 * @return menu_page/1st screen page
 */
void create_menu(void) {
    // Create a new screen
    lv_obj_t *new_menu_scr = lv_obj_create(NULL); // Create a new screen
    lv_scr_load(new_menu_scr); // Load the new screen to make it active
      
     create_page_for_menu(new_menu_scr);

    // Create the menu bar container
    lv_obj_t *menu_bar = lv_obj_create(new_menu_scr);
    lv_obj_set_size(menu_bar, EXAMPLE_LCD_H_RES, 14); // Height of the menu bar
    lv_obj_align(menu_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_color(menu_bar,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(menu_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(menu_bar, LV_OPA_COVER,LV_PART_MAIN);
       
    // Add a label to the menu bar
    lv_obj_t *menu_label = lv_label_create(menu_bar);
    lv_label_set_text(menu_label, "MENU");
    lv_obj_align(menu_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *bottom_bar = lv_obj_create(new_menu_scr);
    lv_obj_set_size(bottom_bar, EXAMPLE_LCD_H_RES, 15); // Height of the bottom bar
    lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_color(bottom_bar,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(bottom_bar,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bottom_bar, LV_OPA_COVER,LV_PART_MAIN);
    
    lv_obj_t *back_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(back_btn, 52, 14); // Width and height of the button
    lv_obj_align(back_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    //lv_obj_add_event_cb(back_btn, back_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, " < Back");
    lv_obj_center(back_label);
    button_list[button_count++] = back_btn; // Add back button to button_list

    lv_obj_t *menu_btn = lv_btn_create(bottom_bar);
    lv_obj_set_size(menu_btn, 56, 14); // Width and height of the button
    lv_obj_align(menu_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    //lv_obj_add_event_cb(menu_btn, menu_button_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_t *menu_label_btn = lv_label_create(menu_btn);
    lv_label_set_text(menu_label_btn, "Menu > ");
    lv_obj_center(menu_label_btn);
    button_list[button_count++] = menu_btn; // Add menu button to button_list
    
    focused_btn_idx = 0;  // Index of the focused button
    focused_btn = button_list[focused_btn_idx];   
    
    // Set focused button style: white background, black text, black border
    lv_obj_set_style_border_color(focused_btn, lv_color_white(), LV_PART_MAIN); // Set border color to black
    lv_obj_set_style_border_width(focused_btn, 2, LV_PART_MAIN); // Set border width
}
/**
 * @fn    static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
 * @brief This function acts as per single click/ long hold press as per button logic
          for single click moves to next option and long hold press, opens up new page
          or pops message as per given instructions.
 * @pre
 * @post
 * @param lv_indev_drv_t *indev_drv, lv_indev_data_t *data,
          static uint8_t last_btn, uint8_t btn
 * @return 
 */

static void button_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
    static uint8_t last_btn = 0;
    uint8_t btn = read_button(); // Your function to read the button state

    if (btn && btn != last_btn) {
        last_btn = btn;
        
        if(btn == BUTTON_SINGLE_CLICK){
			printf("Single press detected \r\n");
		  
        // Deselect the current focused button
        lv_obj_set_style_border_width(focused_btn, 0, LV_PART_MAIN); // Removes border
        
        //focused_btn_idx = get_selected_button_id(menu);
        //scroll_to_next_button(menu);
        
        // Move focus to the next button
        focused_btn_idx = (focused_btn_idx + 1) % button_count;
        focused_btn = button_list[focused_btn_idx];

        // Apply the focused style to the new focused button
        lv_obj_set_style_border_color(focused_btn, lv_color_white(), LV_PART_MAIN);
        lv_obj_set_style_border_width(focused_btn, 2, LV_PART_MAIN); // Applies border
        
        lv_obj_scroll_to_view(focused_btn, LV_ANIM_ON);
        //lv_list_down(menu);
        
        } else if (btn == BUTTON_LONG_PRESS_HOLD) { 
			printf("long press detected \r\n");    
             if (focused_btn == button_list[focused_btn_idx]) {
                const char *focused_text = lv_list_get_btn_text(menu, focused_btn);
                if (strcmp(focused_text, " < Back") == 0) {
            back_button_handler(NULL);  // Navigate back to the previous screen
        } else if (strcmp(focused_text, "Menu > ") == 0) {
			//lv_obj_t* screen= lv_scr_act();
			  //lv_obj_clean(lv_scr_act());
			 //lv_obj_add_flag(lv_scr_act(), LV_OBJ_FLAG_HIDDEN);
			 //create_menu();
            menu_button_handler(NULL);  // Navigate to the main menu
            
                  // Display Config starts from here
        } else if (strcmp(focused_text, "Display Config") == 0) {
                    const char *display_config[] = {"Timer settings", "Brightness"};
                      create_subpage("Display Config", display_config, 2); // Reuse your create_subpage function
                  // deeper
                     } else if (strcmp(focused_text, "Timer settings") == 0) {
                           char *timer_settings[] = {"ID Timer", "QR Timer"};
                           create_deeper_subpage("Timer settings", timer_settings, 2);
                     }else if (strcmp(focused_text, "ID Timer") == 0) {
                            char *ID_timer[] = {"5 seconds","10 seconds","15 seconds","20 seconds","25 seconds","30 seconds"};
                           create_deeper_subpage("ID Timer", ID_timer, 6);
                     } else if (strcmp(focused_text, "QR Timer") == 0) {
                           char *ID_timer[] = {"5 seconds","10 seconds","15 seconds","20 seconds","25 seconds","30 seconds"};
                           create_deeper_subpage("QR Timer", ID_timer, 6);
                     } else if (strcmp(focused_text, "Brightness") == 0) {
                                 level_name_set();
                                 
                                 brightness_level(last_selected_level);
                                 //level_name_set(); 
                                    
                                 list[0] = (char*) l1.str1;
                                 list[1] = (char*) l1.str2;
                                 list[2] = (char*) l1.str3;
                                 list[3] = (char*) l1.str4;
                                 list[4] = (char*) l1.str5;
                                 list[5] = (char*) l1.str6;
                                 list[6] = (char*) l1.str7;
                                 list[7] = (char*) l1.str8;
                                 list[8] = (char*) l1.str9;
                                 list[9] = (char*) l1.str10;
                                 
                                 //last_selected_level;
                        create_deeper_subpage("Brightness", list, 10);

                                 for (int i = 0; i < 10; i++)
                                     {
                                     	 printf("%s\n", list[i]);
                                     }
                           }else if (strcmp(focused_text, "10") == 0) { 
							  last_selected_level = 1;
							  printf("%d\n", last_selected_level );
							//brightness_level(1);  
                           show_message("10% Selected!");  // Call the function to show the message box                        
                           } else if (strcmp(focused_text, "20") == 0) {
							    last_selected_level = 2;
						   //brightness_level(2);
                           show_message("20% Selected!");
                           }else if (strcmp(focused_text, "30") == 0) {
							    last_selected_level = 3;
						   brightness_level(3);
                           //show_message("30% Selected!");
                           }else if (strcmp(focused_text, "40") == 0) {
                           show_message("40% Selected!");
                           brightness_level(4);
                           }else if (strcmp(focused_text, "50") == 0) {
                           show_message("50% Selected!");
                           }else if (strcmp(focused_text, "60") == 0) {
                           show_message("60% Selected!");
                           }else if (strcmp(focused_text, "70") == 0) {
                           show_message("70% Selected!");
                           }else if (strcmp(focused_text, "80") == 0) {
                           show_message("80% Selected!");
                           }else if (strcmp(focused_text, "90") == 0) {
                           show_message("90% Selected!");
                           }else if (strcmp(focused_text, "100") == 0) {
                           show_message("100% Selected!");
                           
                  //Button Config starts below         
                }else if (strcmp(focused_text, "Button Config") == 0) {
                    const char *button_config[] = {"ACK"};
                    create_subpage("Button Config", button_config, 1);
                    // deeper
                     } else if (strcmp(focused_text, "ACK") == 0) {
                             char *ack[] = {"Single click", "Double click"};
                           create_deeper_subpage("ACK", ack, 2);
                           }else if (strcmp(focused_text, "Single click") == 0) {
                           show_message("Single click Selected!");
                           }else if (strcmp(focused_text, "Double click") == 0) {
                           show_message("Double click Selected!");
                           
                   //RGB COnfig starts below        
                } else if (strcmp(focused_text, "RGB Config") == 0) {
                    const char *rgb[] = {"ACK_STATE", "JOB_STATE", "RESET_STATE", "Address success", "Address fail"};
                    create_subpage("RGB Config", rgb, 5);
                    // deeper
                     }  else if (strcmp(focused_text, "ACK_STATE") == 0) {
                             char *ack_state[] = {"RED", "GREEN","BLUE","NONE"};
                           create_deeper_subpage("ACK_STATE", ack_state, 4);
                           }else if (strcmp(focused_text, "RED") == 0) {
                           show_message("Set to RED !");
                           }else if (strcmp(focused_text, "GREEN") == 0) {
                           show_message("Set to GREEN !"); 
                           }else if (strcmp(focused_text, "BLUE") == 0) {
                           show_message("Set to BLUE !"); 
                           }else if (strcmp(focused_text, "NONE") == 0) {
                           show_message("Set to NONE !");  
                     }  else if (strcmp(focused_text, "JOB_STATE") == 0) {
                             char *job[] = {"RED", "GREEN","BLUE","NONE"};
                           create_deeper_subpage("JOB_STATE", job, 4);
                     }  else if (strcmp(focused_text, "RESET_STATE") == 0) {
                            char *reset[] = {"RED", "GREEN","BLUE","NONE"};
                           create_deeper_subpage("RESET_STATE", reset, 4); 
                     } else if (strcmp(focused_text, "Address success") == 0) {
                             char *add[] = {NULL};
                           create_deeper_subpage("Address success", add, 1); 
                           
                      // Address Config from here       
                } else if (strcmp(focused_text, "Address Config") == 0) {
                    const char *add_config[] = {"Curr Address: 1", "Address reset ?"};
                    create_subpage("Address Config", add_config, 2);
                     } else if (strcmp(focused_text, "Address reset ?") == 0) {
                            const char *addreset[] = {"YES","NO"};
                            create_subpage("Address reset ?", addreset, 2); 
                         }else if (strcmp(focused_text,"YES") == 0) {
                           show_message("Address loading..."); 
                           
                      //Health state from here       
                } else if (strcmp(focused_text, "Health state") == 0) {
                    const char *health_state[] = {"Timer to send"};
                    create_subpage("Health state", health_state, 1);
                     } else if (strcmp(focused_text, "Timer to send") == 0) {
                            char *tts[] = {"5 seconds","10 seconds","15 seconds","20 seconds","25 seconds","30 seconds"};
                           create_deeper_subpage("Timer to send", tts, 6); 
                          }else if (strcmp(focused_text, "5 seconds") == 0) {
                           show_message("Timer set to 5 seconds !"); 
                          }else if (strcmp(focused_text, "10 seconds") == 0) {
                           show_message("Timer set to 10 seconds !"); 
                          }else if (strcmp(focused_text, "15 seconds") == 0) {
                           show_message("Timer set to 15 seconds !"); 
                          }else if (strcmp(focused_text, "20 seconds") == 0) {
                           show_message("Timer set to 20 seconds !"); 
                          }else if (strcmp(focused_text, "25 seconds") == 0) {
                           show_message("Timer set to 25 seconds !"); 
                          }else if (strcmp(focused_text, "30 seconds") == 0) {
                           show_message("Timer set to 30 seconds !"); 
                           
                     // Feature Config from here       
                } else if (strcmp(focused_text, "Feature Config") == 0) {
                    const char *feature_config[] = {"Button only", "Prox + Button", "Prox sensor only", "Flap + Button", "Flap + Prox", "Flap+Button+Prox"};
                    create_subpage("Feature Config", feature_config, 6);
                           }else if (strcmp(focused_text, "Button only") == 0) {
                           show_message("Button only selected !"); 
                           }else if (strcmp(focused_text, "Prox + Button") == 0) {
                           show_message("Prox + Button selected !"); 
                           }else if (strcmp(focused_text, "Prox sensor only") == 0) {
                           show_message("Prox sensor only selected !"); 
                           }else if (strcmp(focused_text, "Flap + Button") == 0) {
                           show_message("Flap + Button selected !"); 
                           }else if (strcmp(focused_text, "Flap + Prox") == 0) {
                           show_message("Flap + Prox selected !"); 
                           }else if (strcmp(focused_text, "Flap+Button+Prox") == 0) {
                           show_message("Flap + Button + Prox selected !"); 
                    
                    //Prox sensor Config starts from here
                }else if (strcmp(focused_text, "Prox Sensor") == 0) {
                    const char *proxsensor[] = {"Threshold length","Timer to detect"};
                    create_subpage("Prox Sensor", proxsensor, 2);
                     } else if (strcmp(focused_text, "Threshold length") == 0) {
                            char *TL[] = {"5 cm","10 cm","15 cm"};
                           create_deeper_subpage("Threshold length", TL, 3);
                                }else if (strcmp(focused_text, "5 cm") == 0) {
                                 show_message("Set as 5 cm !");  
                                }else if (strcmp(focused_text, "10 cm") == 0) {
                                 show_message("Set as 10 cm !");  
                                }else if (strcmp(focused_text, "15 cm") == 0) {
                                 show_message("Set as 15 cm !");  
                     } else if (strcmp(focused_text, "Timer to detect") == 0) {
                            char *ttd[] = {"3 seconds","4 seconds","5 seconds","6 seconds"};
                           create_deeper_subpage("Timer to detect", ttd, 4); 
                           }else if (strcmp(focused_text, "3 seconds") == 0) {
                           show_message("Timer set to 3 seconds !"); 
                           }else if (strcmp(focused_text, "4 seconds") == 0) {
                           show_message("Timer set to 4 seconds !"); 
                           }else if (strcmp(focused_text, "6 seconds") == 0) {
                           show_message("Timer set to 6 seconds !"); 
                          
                    //Flap door starts from here     
            }else if (strcmp(focused_text, "Flap door Config") == 0) {
                    const char *flap_door[] = {"Door open speed","Door close speed"};
                    create_subpage("Flap door Config", flap_door, 2);
                 }
             }
        }
    } else if (!btn) {
        last_btn = 0; 
    }
    data->state = btn ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL; 
    data->btn_id = focused_btn_idx;
    //printf("end of button read \r\n");
}
/**
 * @fn    void button_text(void)
 * @brief This function creates the initial display showing text button
          this can be replaced by QR & text alternate display
 * @pre
 * @post
 * @param 
 * @return 
 */
void button_text(void){
	  button_text_bool = true;  
	    
    lv_obj_t *Button_label = lv_obj_create(NULL);
    lv_scr_load(Button_label);
    lv_obj_set_style_text_color(Button_label,lv_color_white() , LV_PART_MAIN);
    lv_obj_set_style_bg_color(Button_label,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(Button_label, LV_OPA_COVER,LV_PART_MAIN);
    lv_obj_t *text = lv_label_create(Button_label);
    lv_label_set_text(text, "BUTTON");
    lv_obj_align(text, LV_ALIGN_CENTER, 0, 0);

}

/**
 * @fn    void display_task(void *pvParameters)
 * @brief This function sets basic building block/ display of screen.
 * @pre
 * @post
 * @param void *pvParameters
 * @return 
 */

void display_task(void *pvParameters)
{ 
	//lv_obj_t *scr = lv_scr_act();
    //lv_obj_clean(scr);
    
    esp_task_wdt_add(NULL);
    // Initialize the input device driver
    lv_indev_drv_t indev_drv; 
    lv_indev_drv_init(&indev_drv); 
    indev_drv.type = LV_INDEV_TYPE_BUTTON; 
    indev_drv.read_cb = button_read; // Your button read function 
    lv_indev_drv_register(&indev_drv);
    /*
    lv_obj_t *Button_label = lv_label_create(lv_scr_act());
    lv_label_set_text(Button_label, "BUTTON");
    lv_obj_align(Button_label, LV_ALIGN_CENTER, 0, 0);
    */
    button_text();
    vTaskDelay(pdMS_TO_TICKS(100));

   while (1) {
	   
    if(read_button()== BUTTON_LONG_PRESS_HOLD ){
			printf("button pressed for menu page\r\n");
            create_menu();
            //esp_task_wdt_reset();  
            printf("menu page created \r\n");
            //esp_task_wdt_reset();
            vTaskDelete(NULL); 
            }
               lv_task_handler();
               
               esp_task_wdt_reset();
               //vTaskDelay(pdMS_TO_TICKS(100));  // after removing this no problem in coming from 
                                                  // button to menu page
    }
    //esp_task_wdt_reset();
    //vTaskDelay(pdMS_TO_TICKS(100));
}


/**
 * @fn    void app_main(void)
 * @brief This is main function of program
 * @pre
 * @post
 * @param 
 * @return 
 */
void app_main(void)
{ 
    last_selected_level = 3;
    ESP_LOGI(TAG, "Initialize SPI bus");

    spi_master_init(&dev, GPIO_MOSI, GPIO_SCLK, GPIO_CS, GPIO_DC, GPIO_RESET);
    ESP_LOGI(TAG, "Panel is 128x64");
    ssd1306_init(&dev, EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    ssd1306_clear_screen(&dev, false);

    ESP_LOGI(TAG, "Initialize LVGL");

    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);
    lv_port_disp_init();

    button_init(BOOT_BUTTON_NUM);

    ssd1306_contrast(&dev, LV_OPA_COVER);
   	//create_menu(); 
    /*while (1) {
        if(read_button() == BUTTON_LONG_PRESS_HOLD) {
            printf("button pressed for menu page\r\n");
            create_menu();  // Create your menu
            printf("menu page created\r\n");
        }*/
        //vTaskDelay(pdMS_TO_TICKS(100));  // Delay to prevent task hogging
        //vTaskDelete(NULL); // Task deletion
        //display_task(NULL);
     xTaskCreatePinnedToCore(display_task, "display_task", 8096, NULL, 1, NULL, 0);
}

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    int32_t x, y;
    lv_color_t *colorPtr = color_p;

    for (y = area->y1; y <= area->y2; y++) {
        for (x = area->x1; x <= area->x2; x++) {
            bool pixel = colorPtr->full;
            _ssd1306_pixel(&dev, x, y, !pixel);
            colorPtr++;
        }
    }

    ssd1306_show_buffer(&dev);
    lv_disp_flush_ready(disp_drv);
}

void lv_port_disp_init(void) {
    static lv_disp_draw_buf_t draw_buf_dsc_1;
    static lv_color_t buf_1[EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES]; // A buffer for the entire screen
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES); // Initialize the display buffer

    static lv_disp_drv_t disp_drv; // Descriptor of a display driver
    lv_disp_drv_init(&disp_drv); // Basic initialization

    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.flush_cb = disp_flush; // Set the function to flush the buffer to the display

    // Set a display buffer
    disp_drv.draw_buf = &draw_buf_dsc_1;

    lv_disp_drv_register(&disp_drv); // Register the display driver
}
