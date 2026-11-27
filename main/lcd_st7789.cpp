#include "lcd_st7789.hpp"

#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_dev.h"
#include "esp_timer.h"

#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"

#include "hal/lcd_types.h"

#include "lvgl.h"

#include "pinos.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>

LCD_ST7789::LCD_ST7789() {
    constexpr gpio_config_t backlight_config{
        .pin_bit_mask = 1ULL << PIN_LCD_BACKLIGHT,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&backlight_config));
    ESP_ERROR_CHECK(gpio_set_level(PIN_LCD_BACKLIGHT, 0));

    constexpr spi_bus_config_t buscfg{
        .mosi_io_num = PIN_LCD_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_LCD_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = -1,
        .data5_io_num = -1,
        .data6_io_num = -1,
        .data7_io_num = -1,
        .data_io_default_level = false,
        .max_transfer_sz = WIDTH * 40 * sizeof(uint16_t),
        .dma_burst_size = 0,
        .flags = SPICOMMON_BUSFLAG_MASTER |
                 SPICOMMON_BUSFLAG_MOSI |
                 SPICOMMON_BUSFLAG_SCLK,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .intr_flags = 0,
        
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = nullptr;
    esp_lcd_panel_io_spi_config_t io_config{
        .cs_gpio_num = PIN_CS,
        .dc_gpio_num = PIN_DC,
        .spi_mode = 0,
        .pclk_hz = 20 * 1000 * 1000,
        .trans_queue_depth = 10,
        .on_color_trans_done = nullptr,
        .user_ctx = nullptr,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .flags{}
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &io_handle));

    constexpr esp_lcd_panel_dev_config_t panel_config{
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .data_endian = LCD_RGB_DATA_ENDIAN_BIG,
        .bits_per_pixel = 16,
        .reset_gpio_num = PIN_RES,
        .vendor_config = nullptr,
        .flags{}
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle_));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle_));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle_));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle_, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle_, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle_, true, false));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle_, 0, 35));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle_, true));
    ESP_ERROR_CHECK(gpio_set_level(PIN_LCD_BACKLIGHT, 1));

    //LVGL configuracoes
    lv_init();
    lv_tick_set_cb(tick_lvgl_ms);

    lv_display_t *disp = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_flush_cb(disp, enviar_pixels_lvgl);
    lv_display_set_user_data(disp, this);

    esp_lcd_panel_io_callbacks_t cbs = {};
    cbs.on_color_trans_done = notificar_lvgl_pronto;
    esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, disp);

    void *buf1 = heap_caps_malloc(BUF_BYTES, MALLOC_CAP_DMA);
    ESP_ERROR_CHECK(buf1 == nullptr ? ESP_ERR_NO_MEM : ESP_OK);
    lv_display_set_buffers(disp, buf1, nullptr, BUF_BYTES, LV_DISPLAY_RENDER_MODE_PARTIAL);
}

void LCD_ST7789::montar_interface() {
    lv_obj_t *tela = lv_screen_active();
    lv_obj_set_style_bg_color(tela, lv_color_hex(0x0F141C), 0);


    // Card Esquerdo
    lv_obj_t *card_esq = criar_card(tela, 8, 8, 188, 154);
    lbl_titulo_ = lv_label_create(card_esq);
    lv_label_set_text(lbl_titulo_, "PNEU");
    lv_obj_set_style_text_color(lbl_titulo_, lv_color_hex(0x38D7EB), 0);
    lv_obj_align(lbl_titulo_, LV_ALIGN_TOP_LEFT, 2, 0);
    lbl_unidade_ = lv_label_create(card_esq);
    lv_obj_set_style_text_color(lbl_unidade_, lv_color_hex(0x96A5BE), 0);
    lv_obj_align(lbl_unidade_, LV_ALIGN_TOP_LEFT, 52, 0);
    badge_modo_ = lv_obj_create(card_esq);
    lv_obj_set_size(badge_modo_, 56, 20);
    lv_obj_set_style_bg_color(badge_modo_, lv_color_hex(0x0F141C), 0);
    lv_obj_set_style_border_width(badge_modo_, 1, 0);
    lv_obj_set_style_radius(badge_modo_, 4, 0);
    lv_obj_set_style_pad_all(badge_modo_, 0, 0);
    lv_obj_clear_flag(badge_modo_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(badge_modo_, LV_ALIGN_TOP_RIGHT, 0, -2);

    // 
    lbl_modo_ = lv_label_create(badge_modo_);
    lv_obj_center(lbl_modo_);

    lbl_pressao_ = lv_label_create(card_esq);
    lv_obj_set_style_text_color(lbl_pressao_, lv_color_hex(0xFAFCFF), 0);
    lv_obj_set_style_text_font(lbl_pressao_, &lv_font_montserrat_38, 0);
    lv_obj_align(lbl_pressao_, LV_ALIGN_CENTER, 0, -4);

    // Barra de pressão (largura = 168 px, representando 0..50 PSI / 0..3.45 BAR)
    static constexpr int LARGURA_BARRA = 168;
    barra_pressao_ = lv_bar_create(card_esq);
    lv_obj_set_size(barra_pressao_, LARGURA_BARRA, 16);
    lv_obj_align(barra_pressao_, LV_ALIGN_BOTTOM_MID, 0, -14);
    lv_bar_set_range(barra_pressao_, 0, 500);
    lv_obj_set_style_bg_color(barra_pressao_, lv_color_hex(0x222C40), LV_PART_MAIN);
    lv_obj_set_style_radius(barra_pressao_, 4, LV_PART_MAIN);
    lv_obj_set_style_radius(barra_pressao_, 4, LV_PART_INDICATOR);

    // Barrinhas brancas indicando a faixa ideal (28 PSI e 36 PSI)
    // A barra começa 2 px após a borda interna esquerda do card
    int x_ideal_min = 2 + (LARGURA_BARRA * 28) / 50; // ~96 px
    int x_ideal_max = 2 + (LARGURA_BARRA * 36) / 50; // ~123 px
    criar_marcador_ideal(card_esq, x_ideal_min);
    criar_marcador_ideal(card_esq, x_ideal_max);

    lv_obj_t *lbl_min = lv_label_create(card_esq);
    lv_label_set_text(lbl_min, "0");
    lv_obj_set_style_text_color(lbl_min, lv_color_hex(0x697891), 0);
    lv_obj_align(lbl_min, LV_ALIGN_BOTTOM_LEFT, 2, 2);

    lbl_escala_max_ = lv_label_create(card_esq);
    lv_obj_set_style_text_color(lbl_escala_max_, lv_color_hex(0x697891), 0);
    lv_obj_align(lbl_escala_max_, LV_ALIGN_BOTTOM_RIGHT, -2, 2);


//#######################################################################
    // Coluna Direita: Status, Int, Ext
//########################################################################
    //caixa de status
    card_status_ = criar_card(tela, 204, 8, 108, 70);
    lbl_status_ = lv_label_create(card_status_);
    lv_obj_set_style_text_font(lbl_status_, &lv_font_montserrat_30, 0);
    lv_obj_center(lbl_status_);

//texto dentro do card de temperatura
    lv_obj_t *card_int = criar_card(tela, 204, 94, 108, 68);
    lv_obj_t *tit_int = lv_label_create(card_int);
    lv_label_set_text(tit_int, "TEMP");
    lv_obj_set_style_text_color(tit_int, lv_color_hex(0x96A5BE), 0);
    lv_obj_align(tit_int, LV_ALIGN_TOP_LEFT, 2, -2);

//temperatura numero
    lbl_temp_int_ = lv_label_create(card_int);
    lv_obj_set_style_text_font(lbl_temp_int_, &lv_font_montserrat_30, 0);
    lv_obj_align(lbl_temp_int_, LV_ALIGN_BOTTOM_MID, 0, 2);

    

    interface_montada_ = true;
    atualizar_painel();
}

void LCD_ST7789::alternar_modo() {
    modo_atual_ = static_cast<ModoRegional>((static_cast<int>(modo_atual_) + 1) % 3);
}

void LCD_ST7789::desligar() {
    ESP_ERROR_CHECK(gpio_set_level(PIN_LCD_BACKLIGHT, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle_, false));
}

void LCD_ST7789::definir_modo(ModoRegional modo) {
    modo_atual_ = modo;
}

void LCD_ST7789::definir_leituras(float pressao_psi, float temperatura_interna_c) {
    pressao_ = pressao_psi;
    temp_int_ = temperatura_interna_c;
}

void LCD_ST7789::atualizar_painel() {
    if (!interface_montada_)
        return;

    const bool em_ingles = modo_atual_ == ModoRegional::EUA || modo_atual_ == ModoRegional::EURO;

    // 1. Avalia faixa ideal da pressão (28..36 PSI) e escolhe texto no idioma do modo
    const char *texto_status = em_ingles ? "Bad" : "Ruim";
    lv_color_t cor_status = lv_color_hex(0xEF4444); // Vermelho

    if (pressao_ >= 28.0F && pressao_ <= 36.0F) {
        texto_status = em_ingles ? "Good" : "Boa";
        cor_status = lv_color_hex(0x2ED573); // Verde
    } else if ((pressao_ >= 22.0F && pressao_ < 28.0F) ||
               (pressao_ > 36.0F && pressao_ <= 42.0F)) {
        texto_status = em_ingles ? "Warning" : "Alerta";
        cor_status = lv_color_hex(0xFACC34); // Amarelo
    }

    lv_label_set_text(lbl_titulo_, em_ingles ? "TIRE" : "PNEU");
    lv_label_set_text(lbl_status_, texto_status);
    lv_obj_set_style_text_color(lbl_status_, cor_status, 0);
    lv_obj_set_style_border_color(card_status_, cor_status, 0);
    lv_obj_set_style_bg_color(barra_pressao_, cor_status, LV_PART_INDICATOR);

    // 2. Atualiza pressão e unidades conforme o modo regional
    const float bar_val = pressao_ / 14.5038F;
    const int psi_int = static_cast<int>(std::lround(pressao_));
    const int t_int_c = static_cast<int>(std::lround(temp_int_));
    const int t_int_f = static_cast<int>(std::lround((temp_int_ * 9.0F / 5.0F) + 32.0F));
    char buf_pressao[16];
    const int valor_barra = std::clamp(static_cast<int>(std::lround(pressao_ * 10.0F)), 0, 500);

    if (modo_atual_ == ModoRegional::LATAM) {
        lv_label_set_text(lbl_modo_, "LATAM");
        lv_obj_set_style_text_color(lbl_modo_, lv_color_hex(0x2ED573), 0);
        lv_obj_set_style_border_color(badge_modo_, lv_color_hex(0x2ED573), 0);

        lv_label_set_text(lbl_unidade_, "PSI");
        std::snprintf(buf_pressao, sizeof(buf_pressao), "%02d", psi_int);
        lv_label_set_text(lbl_pressao_, buf_pressao);
        lv_label_set_text(lbl_escala_max_, "50");
        lv_bar_set_value(barra_pressao_, valor_barra, LV_ANIM_ON);

        lv_label_set_text_fmt(lbl_temp_int_, "%d°C", t_int_c);

    } else if (modo_atual_ == ModoRegional::EUA) {
        lv_label_set_text(lbl_modo_, "USA");
        lv_obj_set_style_text_color(lbl_modo_, lv_color_hex(0x9682F0), 0);
        lv_obj_set_style_border_color(badge_modo_, lv_color_hex(0x9682F0), 0);

        lv_label_set_text(lbl_unidade_, "BAR");
        std::snprintf(buf_pressao, sizeof(buf_pressao), "%.2f", bar_val);
        lv_label_set_text(lbl_pressao_, buf_pressao);
        lv_label_set_text(lbl_escala_max_, "3.5");
        lv_bar_set_value(barra_pressao_, valor_barra, LV_ANIM_ON);

        lv_label_set_text_fmt(lbl_temp_int_, "%d°F", t_int_f);

    } else { // MODO_EURO
        lv_label_set_text(lbl_modo_, "EURO");
        lv_obj_set_style_text_color(lbl_modo_, lv_color_hex(0x38D7EB), 0);
        lv_obj_set_style_border_color(badge_modo_, lv_color_hex(0x38D7EB), 0);

        lv_label_set_text(lbl_unidade_, "BAR");
        std::snprintf(buf_pressao, sizeof(buf_pressao), "%.2f", bar_val);
        lv_label_set_text(lbl_pressao_, buf_pressao);
        lv_label_set_text(lbl_escala_max_, "3.5");
        lv_bar_set_value(barra_pressao_, valor_barra, LV_ANIM_ON);

        lv_label_set_text_fmt(lbl_temp_int_, "%d°C", t_int_c);
    }

    // 3. Aplica cores das temperaturas
    lv_obj_set_style_text_color(lbl_temp_int_, cor_por_temperatura(temp_int_), 0);
}

bool LCD_ST7789::notificar_lvgl_pronto(esp_lcd_panel_io_handle_t io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx) {
    lv_display_flush_ready(static_cast<lv_display_t *>(user_ctx));
    return false;
}

void LCD_ST7789::enviar_pixels_lvgl(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;
    lv_draw_sw_rgb565_swap(px_map, w * h);
    auto *lcd = static_cast<LCD_ST7789 *>(lv_display_get_user_data(disp));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(lcd->panel_handle_, area->x1, area->y1,
                                              area->x2 + 1, area->y2 + 1, px_map));
}

uint32_t LCD_ST7789::tick_lvgl_ms() {
    return static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);
}

lv_obj_t *LCD_ST7789::criar_card(lv_obj_t *pai, int x, int y, int w, int h) {
    lv_obj_t *card = lv_obj_create(pai);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, w, h);
    lv_obj_set_style_bg_color(card, lv_color_hex(0x161D2A), 0);
    lv_obj_set_style_border_color(card, lv_color_hex(0x344460), 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_radius(card, 8, 0);
    lv_obj_set_style_pad_all(card, 6, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    return card;
}

void LCD_ST7789::criar_marcador_ideal(lv_obj_t *pai, int offset_x) {
    lv_obj_t *marca = lv_obj_create(pai);
    lv_obj_set_size(marca, 2, 20);
    lv_obj_set_style_bg_color(marca, lv_color_hex(0xFAFCFF), 0);
    lv_obj_set_style_border_width(marca, 0, 0);
    lv_obj_set_style_radius(marca, 0, 0);
    lv_obj_set_style_pad_all(marca, 0, 0);
    lv_obj_clear_flag(marca, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align(marca, LV_ALIGN_BOTTOM_LEFT, offset_x, -12);
}

lv_color_t LCD_ST7789::cor_por_temperatura(float temp_c) {
    if (temp_c <= 35.0F) return lv_color_hex(0x38D7EB); // Ciano
    if (temp_c <= 46.0F) return lv_color_hex(0xFACC34); // Amarelo
    return lv_color_hex(0xF97316);                   // Laranja
}
