#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/i2c_master.h"

#include "hal/gpio_types.h"

#include "misc/lv_timer.h"

#include "lcd_st7789.hpp"
#include "smp_3011.hpp"
#include "pinos.hpp"

static const char* TAG = "projeto";

// Botões
static void inicializar_botoes() {
    constexpr gpio_config_t btn_cfg{
        .pin_bit_mask = (1ULL << PIN_BTN_LATAM) | (1ULL << PIN_BTN_EUA) | (1ULL << PIN_BTN_EURO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    ESP_ERROR_CHECK(gpio_config(&btn_cfg));
}

extern "C" void app_main() {
	//=== Config I2C ===//
	ESP_LOGI(TAG, "Inicializar bus I2C");
	i2c_master_bus_handle_t i2c_bus = NULL;
	i2c_master_bus_config_t bus_config{
        .i2c_port = I2C_BUS_PORT,
        .sda_io_num = PIN_SDA,
        .scl_io_num = PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags{
            .enable_internal_pullup = true,
            .allow_pd = false
        }
    };

	ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    ESP_LOGI(TAG, "Escaneando por dispositivos I2C...");
	for (int i = 1; i < 127; i++) {
		esp_err_t err = i2c_master_probe(i2c_bus, i, -1);
		if (err == ESP_OK)
			ESP_LOGI(TAG, "Dispositivo encontrado em 0x%02X", i);
	}

    //=== Configuração de Dispositivos/Sensores ===//
    LCD_ST7789 display;
    display.montar_interface();

    SMP_3011 smp_3011(i2c_bus);

    inicializar_botoes();

    //=== Loop ===//
    while (true) {
        SMP_3011::RawReading smp_reading = smp_3011.read();

        float smp_temp = SMP_3011::convert_temp(smp_reading.temp);
        float smp_pressure = SMP_3011::convert_pressure(smp_reading.pressure);
        display.definir_leituras(smp_pressure, smp_temp, 25.0F);

        if (gpio_get_level(PIN_BTN_LATAM) == 0)
            display.definir_modo(LCD_ST7789::ModoRegional::LATAM);
        else if (gpio_get_level(PIN_BTN_EUA) == 0)
            display.definir_modo(LCD_ST7789::ModoRegional::EUA);
        else if (gpio_get_level(PIN_BTN_EURO) == 0)
            display.definir_modo(LCD_ST7789::ModoRegional::EURO);

        display.atualizar_painel();

        uint32_t espera_ms = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(espera_ms < 10 ? 10 : espera_ms));
    }
}
