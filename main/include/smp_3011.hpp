#pragma once

#include "driver/i2c_master.h"

#include <cstdint>

class SMP_3011 {
public:
    explicit SMP_3011(const i2c_master_bus_handle_t& i2c_bus);

    struct RawReading {
        uint32_t pressure;
    	uint16_t temp;
    };
    RawReading read() const;

    static float convert_pressure(uint32_t raw) {
        /*
        * O datasheet especifica que o sensor é calibrado de 15% a 85%,
        * porém o nosso está dando valores por volta de 14%.
        * Por conta disso, será usado um pequeno offset.
        */

        float pct = (float)raw / 16777216.0f;

        float psi = ((pct - .15f) / .7f) * 500.0f * 0.29007548f;
        return psi < 0 ? 0 : psi;
    }
    static float convert_temp(uint16_t raw) {
        float pct = (float)raw / 65536.0f;
        return 190.0f * pct - 40.0f;			// Calibrado de -40°C - 150°C
    }

private:
    const char* TAG = "SMP_3011";

    const uint8_t addr = 0x78;

    i2c_master_dev_handle_t handle;

    uint32_t leitura_pressao = 0;
	uint16_t leitura_temp = 0;
};
