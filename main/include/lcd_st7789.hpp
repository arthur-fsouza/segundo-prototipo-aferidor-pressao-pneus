/*
 * Este projeto utiliza o LCD st7789
 */

#pragma once

#include "esp_lcd_types.h"

#include "misc/lv_area.h"
#include "misc/lv_color.h"
#include "misc/lv_types.h"

#include <cstddef>
#include <cstdint>

class LCD_ST7789 {
public:
    enum class ModoRegional {
        LATAM,
        EUA,
        EURO
    };

    LCD_ST7789();

    void montar_interface();
    void alternar_modo();
    void definir_modo(ModoRegional modo);
    void definir_leituras(float pressao_psi, float temperatura_interna_c, float temperatura_externa_c);
    void atualizar_painel();

private:
    // Callbacks
    static bool notificar_lvgl_pronto(esp_lcd_panel_io_handle_t io, esp_lcd_panel_io_event_data_t *edata,
                                      void *user_ctx);
    static void enviar_pixels_lvgl(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
    static uint32_t tick_lvgl_ms();

    lv_obj_t *criar_card(lv_obj_t *pai, int x, int y, int w, int h);
    void criar_marcador_ideal(lv_obj_t *pai, int offset_x);

    lv_color_t cor_por_temperatura(float temp_c);

    static constexpr int WIDTH  = 320;
    static constexpr int HEIGHT = 170;
    static constexpr size_t BUF_BYTES = WIDTH * 40 * sizeof(uint16_t);

    esp_lcd_panel_handle_t panel_handle_ = nullptr;

    lv_obj_t *lbl_titulo_ = nullptr;
    lv_obj_t *lbl_unidade_ = nullptr;
    lv_obj_t *lbl_modo_ = nullptr;
    lv_obj_t *badge_modo_ = nullptr;
    lv_obj_t *lbl_pressao_ = nullptr;
    lv_obj_t *barra_pressao_ = nullptr;
    lv_obj_t *lbl_escala_max_ = nullptr;
    lv_obj_t *card_status_ = nullptr;
    lv_obj_t *lbl_status_ = nullptr;
    lv_obj_t *lbl_temp_int_ = nullptr;
   // lv_obj_t *lbl_temp_ext_ = nullptr;

    ModoRegional modo_atual_ = ModoRegional::LATAM;
    float pressao_ = 32.0F;
    float temp_int_ = 30.0F;
    float temp_ext_ = 25.0F;
    bool interface_montada_ = false;
};
