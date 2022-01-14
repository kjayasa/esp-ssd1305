#ifndef __SSD1305_H
#define __SSD1305_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "font.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        gpio_num_t clk;
        gpio_num_t mosi;
        gpio_num_t cs;
        gpio_num_t reset;
        gpio_num_t dc;
        uint8_t height;
        uint8_t widht;
    } ssd_1305_device_config_t;

    typedef struct
    {
        ssd_1305_device_config_t config;
        uint8_t *p_frameBuffer;
        size_t frameBufferPagesCount;
        size_t frameBufferLength;
        spi_device_handle_t handle_spi;

    } ssd_1305_device_handle_t;

    typedef enum
    {
        orintation_normal = 1,
        orintation_ninenty = 2,
        orintation_oneeighty = 3,
        orintation_twoseventy = 4

    } draw_orintation;

    typedef enum
    {
        transfer_mode_normal = 1,
        transfer_mode_transperent = 2,
        transfer_mode_invert = 3
    } transfer_mode;

    void ssd_1305_create(ssd_1305_device_config_t *config, ssd_1305_device_handle_t *h_dev);

    void ssd_1305_setPixel(ssd_1305_device_handle_t *h_dev, uint8_t x, uint8_t y, uint8_t val);
    void ssd_1305_clear_framebuffer(ssd_1305_device_handle_t *h_dev, uint8_t fillColor);
    void ssd_1305_draw_bitmap(ssd_1305_device_handle_t *h_dev, uint8_t x, uint8_t y, const bitmap_image *pImage, draw_orintation orintation, transfer_mode transferMode);
    void ssd_1305_draw_text(ssd_1305_device_handle_t *h_dev, const char *string, uint8_t x, uint8_t y, const font *pFont, draw_orintation orintation, transfer_mode transferMode);
    void ssd_1305_send_buffer(ssd_1305_device_handle_t *h_dev);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
class Ssd1305
{

private:
    
    ssd_1305_device_handle_t m_hDev;

public:
    Ssd1305(ssd_1305_device_config_t *config);

    void setPixel( uint8_t x, uint8_t y, uint8_t val);
    void clear_framebuffer( uint8_t fillColor);
    void draw_bitmap( uint8_t x, uint8_t y, const bitmap_image *pImage, draw_orintation orintation, transfer_mode transferMode);
    void draw_text( const char *string, uint8_t x, uint8_t y, const font *pFont, draw_orintation orintation, transfer_mode transferMode);
    void send_buffer();
};

#endif

#endif
