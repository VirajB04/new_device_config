/*
 * textbox.c
 *
 *  Created on: 04-Sep-2024
 *      Author: HP
 */

#include "misc/lv_color.h"
#include<stdio.h>
#include<textbox.h>


/**
 * @fn void timer_callback(lv_timer_t *timer)
 * @brief Timer callback function to switch to the desired page after msg is popped on the screen
 * @pre
 * @post
 * @param lv_timer_t *timer
 * @return 
 */
 
void timer_callback(lv_timer_t *timer) {
     lv_obj_t *msg_screen = (lv_obj_t *)timer->user_data; // Access user data directly from the timer structure
     lv_obj_del(msg_screen); // Delete the message screen
}

/**
 * @fn void show_message(const char *msg)
 * @brief function to display/pop message when last choice is selected/ filter is applied.
 * @pre
 * @post
 * @param const char *msg
 * @return input message
 */
void show_message(const char *msg) {
    // Create a black background screen to hold the message
    lv_obj_t *msg_screen = lv_obj_create(NULL); // Create a new screen (page)
    lv_obj_set_style_bg_color(msg_screen, lv_color_black(), 0); // Set background color to black
    lv_obj_set_style_bg_opa(msg_screen, LV_OPA_COVER,LV_PART_MAIN);
    lv_scr_load(msg_screen); // Load the message screen to show the message

    // Create a text box for the message
    lv_obj_t *textbox = lv_obj_create(msg_screen); // Attach the textbox to the message screen
    lv_obj_set_size(textbox, 124, 50); // Size the textbox to fit the content
    lv_obj_align(textbox, LV_ALIGN_CENTER, 0, 0); // Center the textbox on the screen
    lv_obj_set_style_bg_color(textbox,lv_color_black() , LV_PART_MAIN);
    lv_obj_set_style_bg_opa(textbox, LV_OPA_COVER,LV_PART_MAIN);
    lv_obj_set_style_border_width(textbox, 1, 0); // Set the border width (thicker for visibility)
    lv_obj_set_style_border_color(textbox, lv_color_white(), 0); // Set the border color to white
    lv_obj_set_style_radius(textbox, 10, 0); // Set the radius for curved corners
    lv_obj_set_style_pad_all(textbox, 6, 0); // Add padding to ensure text isn't too close to the border

    // Create a label for the text inside the textbox
    lv_obj_t *label = lv_label_create(textbox);
    lv_label_set_text(label, msg); // Set the message text
    lv_obj_set_style_text_color(label,lv_color_white() , LV_PART_MAIN);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP); // Enable text wrapping
    lv_obj_set_width(label, 110); // Set the width of the label to fit within the textbox
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Center the label inside the textbox

    // Create a timer to switch to the desired page after 2 seconds
    //lv_timer_t *timer = lv_timer_create(timer_callback, 2000, NULL); // 1000 ms = 1 second
    //timer->user_data = msg_screen; // Pass the msg_screen as user data to the timer
}


