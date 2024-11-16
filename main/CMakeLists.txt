idf_component_register(SRCS "spi_oled_app.c" "spi_oled_drv/spi_oled_drv.c" "spi_oled_drv/ssd1306.c" "spi_oled_drv/button.c"
                          "spi_oled_drv/textbox.c"
                       INCLUDE_DIRS "include" "spi_oled_drv")
