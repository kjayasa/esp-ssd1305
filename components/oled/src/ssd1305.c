#include <stdio.h>
#include <string.h>

#include "ssd1305.h"
#include "esp_log.h"

#include "bitmap_image.h"

#include "font.h"

#undef ESP_ERROR_CHECK
#define ESP_ERROR_CHECK(x)                         \
    do                                             \
    {                                              \
        esp_err_t rc = (x);                        \
        if (rc != ESP_OK)                          \
        {                                          \
            ESP_LOGE("err", "esp_err_t = %d", rc); \
            assert(0 && #x);                       \
        }                                          \
    } while (0);

#define ASCII_START_VAL ' '

typedef struct
{
    ssd_1305_device_handle_t *h_dev;
    uint8_t isData;

} spi_pre_transfer_callback_data_t;

void delay_ms(const TickType_t xms)
{
    vTaskDelay(xms / portTICK_RATE_MS);
}

void set_cs_low(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.cs, 0); }
void set_cs_heigh(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.cs, 1); }

void set_rst_low(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.reset, 0); }
void set_rst_heigh(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.reset, 1); }

void set_dc_low(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.dc, 0); }
void set_dc_high(ssd_1305_device_handle_t *h_dev) { gpio_set_level(h_dev->config.dc, 1); }

void spi_write_data_n(ssd_1305_device_handle_t *h_dev, uint8_t *pData, uint32_t Len, uint8_t is_data)
{

    spi_transaction_t trans_desc;
    trans_desc.addr = 0;
    trans_desc.cmd = 0;
    trans_desc.flags = 0;
    trans_desc.length = 8 * Len; // Number of bits NOT number of bytes.
    trans_desc.rxlength = 0;
    trans_desc.tx_buffer = pData;
    trans_desc.rx_buffer = NULL;

    spi_pre_transfer_callback_data_t *callbackData = calloc(1, sizeof(spi_pre_transfer_callback_data_t));

    if (callbackData != NULL)
    {
        callbackData->h_dev = h_dev;
        callbackData->isData = is_data;

        trans_desc.user = callbackData;
    }
    else
    {
        ESP_LOGE("SSD1305_SPI_WRITE","could not allocate memoryfor call back");
    }

    printf("spi_device_transmit \n");

    ESP_ERROR_CHECK(spi_device_transmit(h_dev->handle_spi, &trans_desc));
}

void spi_write_4_bytes(ssd_1305_device_handle_t *h_dev, uint8_t byte1, int8_t byte2, int8_t byte3, int8_t byte4, int8_t len, uint8_t is_data) /// used///
{

    spi_transaction_t trans_desc;
    trans_desc.addr = 0;
    trans_desc.cmd = 0;
    trans_desc.flags = SPI_TRANS_USE_TXDATA;
    trans_desc.length = 8 * len; // Number of bits NOT number of bytes.
    trans_desc.rxlength = 0;
    trans_desc.tx_buffer = NULL;
    trans_desc.rx_buffer = NULL;
    trans_desc.tx_data[0] = byte1;
    trans_desc.tx_data[1] = byte2;
    trans_desc.tx_data[2] = byte3;
    trans_desc.tx_data[3] = byte4;

    spi_pre_transfer_callback_data_t *callbackData = calloc(1, sizeof(spi_pre_transfer_callback_data_t));

    if (callbackData != NULL)
    {
        callbackData->h_dev = h_dev;
        callbackData->isData = is_data;

        trans_desc.user = callbackData;
    }
    else
    {
        ESP_LOGE("SSD1305_SPI_WRITE","could not allocate memoryfor call back");
    }
    ESP_ERROR_CHECK(spi_device_transmit(h_dev->handle_spi, &trans_desc));
}

void spi_write_byte(ssd_1305_device_handle_t *h_dev, uint8_t Value, uint8_t is_data) /// used///
{
    spi_write_4_bytes(h_dev, Value, 0, 0, 0, 1, is_data);
}

// This function is called (in irq context!) just before a transmission starts. It will
// set the D/C line to the value indicated in the user field.
void lcd_spi_pre_transfer_callback(spi_transaction_t *t)
{
    spi_pre_transfer_callback_data_t *callbackData = (spi_pre_transfer_callback_data_t *)t->user;
    if (callbackData)
    {
        gpio_set_level(callbackData->h_dev->config.dc, callbackData->isData);
        free(callbackData);
    }
}

void setup_spi(ssd_1305_device_handle_t *h_dev)
{

    // GPIO Config
    gpio_set_direction(h_dev->config.dc, GPIO_MODE_OUTPUT);
    gpio_set_direction(h_dev->config.reset, GPIO_MODE_OUTPUT);

    spi_bus_config_t buscfg = {
        .miso_io_num = -1,
        .mosi_io_num = h_dev->config.mosi,
        .sclk_io_num = h_dev->config.clk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 16 * 320 * 2 + 8};
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 10 * 1000 * 1000, // Clock out at 10 MHz

        .mode = 0,                               // SPI mode 0
        .spics_io_num = h_dev->config.cs,        // CS pin
        .queue_size = 7,                         // We want to be able to queue 7 transactions at a time
        .pre_cb = lcd_spi_pre_transfer_callback, // Specify pre-transfer callback to handle D/C line
    };

    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &(h_dev->handle_spi)));

    printf("setup_spi OK \r\n");
}

static void reset(ssd_1305_device_handle_t *h_dev)
{
    set_rst_heigh(h_dev);
    delay_ms(100);
    set_rst_low(h_dev);
    delay_ms(100);
    set_cs_heigh(h_dev);
    set_dc_low(h_dev);
    set_rst_heigh(h_dev);
    delay_ms(100);
}

static void write_command(ssd_1305_device_handle_t *h_dev, uint8_t Reg)
{
    spi_write_byte(h_dev, Reg, 0);
}

static void write_data(ssd_1305_device_handle_t *h_dev, uint8_t Data)
{
    spi_write_byte(h_dev, Data, 1);
}
static void write_command_n(ssd_1305_device_handle_t *h_dev, uint8_t *Reg, uint32_t len)
{
    spi_write_data_n(h_dev, Reg, len, 0);
}

static void write_data_n(ssd_1305_device_handle_t *h_dev, uint8_t *Data, uint32_t len)
{
    spi_write_data_n(h_dev, Data, len, 01);
}

static void setup_ssd1305_regs(ssd_1305_device_handle_t *h_dev)
{

    write_command(h_dev, 0x00);
    write_command(h_dev, 0x10);
    //     write_command(0xAE);	/*turn off OLED display*/

    //     write_command(0x04);	/*turn off OLED display*/

    //     write_command(0x10);	/*turn off OLED display*/

    //     write_command(0x40);	/*set lower column address*/
    //     write_command(0x81);	/*set higher column address*/
    //     write_command(0x80);  	//--set s   write_command(0x40);

    //     write_command(0xDB); 	 /*set vcomh*/
    //     write_command(0x08);  	//Set VCOM Deselect Level

    //     write_command(0xAF)setPixelrn off OLED display
    write_command(h_dev, 0xAF); // turn on OLED display

    // write_command(0x22); 	//Set page addressing mode
    write_command(h_dev, 0x20); // Set page Horizontal mode

    // set_dc_low();
    // spi_write_4_bytes(0x21,0,131,0,3);// Setup column start and end address to 0..131

    // set_dc_low();
    // spi_write_4_bytes(0x22,0,7,0,3);// Setup page start and end address to 0..7

    ssd_1305_clear_framebuffer(h_dev, 0x00);
}

void ssd_1305_setPixel(ssd_1305_device_handle_t *h_dev, uint8_t x, uint8_t y, uint8_t val)
{

    uint8_t page = y / 8;
    size_t index = (page * h_dev->config.widht) + x;

    if (val)
    {
        h_dev->p_frameBuffer[index] |= 1 << (y % 8);
    }
    else
    {
        h_dev->p_frameBuffer[index] &= ~(1 << (y % 8));
    }
}

void ssd_1305_send_buffer(ssd_1305_device_handle_t *h_dev)
{

    for (size_t i = 0; i < h_dev->frameBufferPagesCount; i++)
    {
        write_command(h_dev, 0xb0 + i);

        write_command(h_dev, 0x00);
        write_command(h_dev, 0x10);

        // write_data_n((p_frameBuffer+(i*h_dev->config.widht)),h_dev->config.widht);
        for (uint8_t j = 0; j < h_dev->config.widht; j++)
        {
            write_data(h_dev, h_dev->p_frameBuffer[(i * h_dev->config.widht) + j]);

            // write_data(getSegment(i,j));
        }
    }
}

void ssd_1305_clear_framebuffer(ssd_1305_device_handle_t *h_dev, uint8_t fillColor)
{
    memset(h_dev->p_frameBuffer, 0, h_dev->frameBufferLength);
}

void ssd_1305_draw_bitmap(ssd_1305_device_handle_t *h_dev, uint8_t x, uint8_t y, const bitmap_image *pImage, draw_orintation orintation, transfer_mode transferMode)
{

    uint16_t px = 0, py = 0, dx = 0, dy = 0, byteWidth = (pImage->width + 7) / 8;
    uint8_t pixelVal;
    for (py = 0; py < pImage->height; py++)
    {
        for (px = 0; px < pImage->width; px++)
        {

            switch (orintation)
            {
            case orintation_normal:
                dx = x + px;
                dy = y + py;
                break;
            case orintation_ninenty:
                dx = x - py;
                dy = y + px;
                break;
            case orintation_oneeighty:
                dx = x - px;
                dy = y - py;
                break;
            case orintation_twoseventy:
                dx = x + py;
                dy = y - px;
                break;
            }

            pixelVal = (*(pImage->buffer + py * byteWidth + px / 8) & (128 >> (px & 7)));

            switch (transferMode)
            {
            case transfer_mode_normal:
                ssd_1305_setPixel(h_dev, dx, dy, pixelVal ? 1 : 0);
                break;
            case transfer_mode_transperent:
                if (pixelVal)
                {
                    ssd_1305_setPixel(h_dev, dx, dy, 1);
                }
                break;
            case transfer_mode_invert:
                ssd_1305_setPixel(h_dev, dx, dy, pixelVal ? 0 : 1);
                break;
            }
        }
    }
}

void ssd_1305_draw_text(ssd_1305_device_handle_t *h_dev, const char *string, uint8_t x, uint8_t y, const font *pFont, draw_orintation orintation, transfer_mode transferMode)
{

    size_t line_height = pFont->line_height;

    size_t len = strlen(string);

    uint8_t cx = x;
    uint8_t cy = y;

    for (size_t i = 0; i < len; i++)
    {
        const bitmap_image *glyph = &pFont->glyphs[string[i] - ASCII_START_VAL];

        switch (orintation)
        {
        case orintation_normal:
            cy = y + (line_height - glyph->height);
            break;
        case orintation_oneeighty:
            cy = y - (line_height - glyph->height);
            break;
        case orintation_ninenty:
            cx = x + (line_height - glyph->height);
            break;
        case orintation_twoseventy:
            cx = x - (line_height - glyph->height);
            break;
        }

        ssd_1305_draw_bitmap(h_dev, cx, cy, glyph, orintation, transferMode);

        switch (orintation)
        {
        case orintation_normal:
            cx += glyph->width;
            break;
        case orintation_oneeighty:
            cx -= glyph->width;
            break;
        case orintation_ninenty:
            cy += glyph->width;
            break;
        case orintation_twoseventy:
            cy -= glyph->width;
            break;
        }
    }
}
void ssd_1305_create(ssd_1305_device_config_t *config, ssd_1305_device_handle_t *h_dev)
{
    // TODO: error check, width ,height ets

    h_dev->config = *config;

    h_dev->frameBufferPagesCount = h_dev->config.height / 8; // each page is 8 pix tall

    h_dev->frameBufferLength = h_dev->config.widht * h_dev->frameBufferPagesCount;

    // allocate framebuffer memory
    h_dev->p_frameBuffer = calloc(1, h_dev->frameBufferLength);

    delay_ms(20);
    // setup spi
    setup_spi(h_dev);

    // Hardware reset
    reset(h_dev);

    // Set the initialization register
    setup_ssd1305_regs(h_dev);
}
