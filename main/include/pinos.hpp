#pragma once

#include "soc/gpio_num.h"

// SMP 3011:
// Amarelo:	SDA
// Azul:	SCL

constexpr int I2C_BUS_PORT = 0;
constexpr gpio_num_t PIN_SDA = GPIO_NUM_23;
constexpr gpio_num_t PIN_SCL = GPIO_NUM_18;
constexpr gpio_num_t PIN_RES = GPIO_NUM_4;
constexpr gpio_num_t PIN_DC = GPIO_NUM_2;
constexpr gpio_num_t PIN_CS = GPIO_NUM_5;

constexpr gpio_num_t PIN_BTN_LATAM = GPIO_NUM_12; // PSI + °C (Português)
constexpr gpio_num_t PIN_BTN_EUA = GPIO_NUM_13; // BAR + °F (Inglês)
constexpr gpio_num_t PIN_BTN_EURO = GPIO_NUM_14; // BAR + °C (Inglês)
