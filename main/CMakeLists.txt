idf_component_register(
    SRCS
        "main.cpp"
        
        "lcd_st7789.cpp"
        "smp_3011.cpp"
    
    INCLUDE_DIRS
        "."
        "include"

    REQUIRES
        driver
        esp_lcd
        esp_timer
        esp_adc
        lvgl
)