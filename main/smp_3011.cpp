#include "smp_3011.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "driver/i2c_master.h"

SMP_3011::SMP_3011(const i2c_master_bus_handle_t& i2c_bus) {
	i2c_device_config_t dev_config = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = addr,
		.scl_speed_hz = 400000,
		.scl_wait_us = 0,
		.flags = {
			.disable_ack_check = 0
		}
	};         

	ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &dev_config, &handle));
	ESP_LOGI(TAG, "Sensor SMP 3011 inicializado com sucesso!");
}

// Pego do datasheet:
// https://pt.scribd.com/document/627622073/SMP3011%E5%9E%8BI2C-%E9%80%9A%E8%AE%AF%E5%8D%8F%E8%AE%AE%E6%8C%87%E4%BB%A4%E8%AF%B4%E6%98%8E-V2-English
SMP_3011::RawReading SMP_3011::read() const {
	// Passo 1: Mandar bytes específicos ao sensor
	constexpr uint8_t cmd = 0xAC;
	ESP_ERROR_CHECK(
		i2c_master_transmit(handle, &cmd, 1, -1)
	);

	// Passo 2: Fazer polling e receber dados
	uint8_t dados[6];
	do {
		ESP_ERROR_CHECK(
			i2c_master_receive(handle, dados, sizeof(dados), -1)
		);

		vTaskDelay(pdMS_TO_TICKS(2));
	} while (dados[0] & (1 << 5));	// O 5° bit nos diz se a leitura está pronta

	// ESP_LOGI(TAG, "Dados crus: %02X %02X %02X %02X %02X %02X",
	// 		 dados[0], dados[1], dados[2],
	// 		 dados[3], dados[4], dados[5]);

    return {
        ((uint32_t)dados[1] << 16) | ((uint32_t)dados[2] << 8) | dados[3],
        (uint16_t)(((uint16_t)dados[4] << 8) | dados[5])
    };
}
