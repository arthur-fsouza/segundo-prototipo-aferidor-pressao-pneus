#pragma once

#include "soc/gpio_num.h"

// SMP 3011:
// Amarelo:	SDA
// Azul:	SCL

// I2C
constexpr int I2C_BUS_PORT = 0;
constexpr gpio_num_t PIN_SDA = GPIO_NUM_21;
constexpr gpio_num_t PIN_SCL = GPIO_NUM_22;

constexpr gpio_num_t PIN_LCD_BACKLIGHT = GPIO_NUM_23;
constexpr gpio_num_t PIN_LCD_MOSI = GPIO_NUM_19;
constexpr gpio_num_t PIN_LCD_SCLK = GPIO_NUM_18;

constexpr gpio_num_t PIN_RES = GPIO_NUM_4;
constexpr gpio_num_t PIN_DC = GPIO_NUM_2;
constexpr gpio_num_t PIN_CS = GPIO_NUM_5;

constexpr gpio_num_t PIN_BTN_LOCAL = GPIO_NUM_12;
constexpr gpio_num_t PIN_BTN_ON_OFF = GPIO_NUM_13;
