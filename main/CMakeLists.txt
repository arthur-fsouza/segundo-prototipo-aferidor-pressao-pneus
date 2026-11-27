idf_component_register(
    SRCS
        "main.cpp"
        
        "oled.cpp"
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