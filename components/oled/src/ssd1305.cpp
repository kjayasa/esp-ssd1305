
#include "ssd1305.h"

Ssd1305::Ssd1305(ssd_1305_device_config_t *config)
{
    ssd_1305_create(config, &(this->m_hDev));
}

void Ssd1305::setPixel(uint8_t x, uint8_t y, uint8_t val)
{
    ssd_1305_setPixel(&(this->m_hDev), x, y, val);
}
void Ssd1305::clear_framebuffer(uint8_t fillColor)
{
    ssd_1305_clear_framebuffer(&(this->m_hDev), fillColor);
}
void Ssd1305::draw_bitmap( uint8_t x, uint8_t y, const bitmap_image *pImage, draw_orintation orintation, transfer_mode transferMode)
{
    ssd_1305_draw_bitmap(&(this->m_hDev), x, y, pImage, orintation, transferMode);
}
void Ssd1305::draw_text(const char *string, uint8_t x, uint8_t y, const font *pFont, draw_orintation orintation, transfer_mode transferMode)
{
    ssd_1305_draw_text(&(this->m_hDev), string, x, y, pFont, orintation, transferMode);
}
void Ssd1305::send_buffer()
{
    ssd_1305_send_buffer(&(this->m_hDev));
}