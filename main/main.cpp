#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ssd1305.h"

#include "sdkconfig.h"
#include "ImageData.h"
#include "fonts.h"

#define OLED_2in23_WIDTH 128
#define OLED_2in23_HEIGHT 32
// CLK - GPIO14
#define PIN_CLK GPIO_NUM_14

// MOSI - GPIO 13
#define PIN_MOSI GPIO_NUM_13

// RESET - GPIO 26
#define PIN_RESET GPIO_NUM_26

// DC - GPIO 27
#define PIN_DC GPIO_NUM_27

// CS - GPIO 15
#define PIN_CS GPIO_NUM_15

extern "C"
{
    void app_main();
}

static font *allfonts[] =
    {
        &font_teletactile_8, &font_teletactile_13, &font_teletactile_15, &font_teletactile_18, &font_teletactile_19, &font_teletactile_21, &font_teletactile_22, NULL};

void app_main()
{
    ssd_1305_device_config_t config;

    printf("OELD_test Demo... starting\r\n");

    config.clk = PIN_CLK;
    config.mosi = PIN_MOSI;
    config.cs = PIN_CS;
    config.reset = PIN_RESET;
    config.dc = PIN_DC;
    config.height = 32;
    config.widht = 128;

    printf("OELD Init complete\r\n");

    Ssd1305 oled(&config);

    while (1)
    {

        for (size_t i = 0; i < 4; i++)
        {
            oled.draw_bitmap(64 - 7, 16 - 7, &test_image, orintation_normal, transfer_mode_normal);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);
            oled.clear_framebuffer(0);

            oled.draw_bitmap(64, 16-7, &test_image, orintation_ninenty, transfer_mode_normal);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);
            oled.clear_framebuffer(0);

            oled.draw_bitmap(64, 16, &test_image, orintation_oneeighty, transfer_mode_normal);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);
            oled.clear_framebuffer(0);

            oled.draw_bitmap(64 - 7, 16, &test_image, orintation_twoseventy, transfer_mode_normal);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);
            oled.clear_framebuffer(0);
        }

        uint8_t i = 0;
        do
        {

            oled.clear_framebuffer(0);
            oled.draw_text("Riddha", 0, 0, allfonts[i], orintation_normal, transfer_mode_transperent);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);

            oled.clear_framebuffer(0);
            char *string = new char[5];
            sprintf(string, "%d", i);
            oled.draw_text(string, 0, 0, allfonts[i], orintation_normal, transfer_mode_transperent);
            oled.send_buffer();
            vTaskDelay(1000 / portTICK_RATE_MS);

            delete string;

            i++;
        } while (allfonts[i] != NULL);

        // TODO: bug- Wrapping is happening

        oled.clear_framebuffer(0);
        oled.draw_text("Riddha", 0, 0, allfonts[0], orintation_normal, transfer_mode_normal);
        oled.send_buffer();
        vTaskDelay(1000 / portTICK_RATE_MS);

        oled.clear_framebuffer(0);
        oled.draw_text("Riddha", 126, 0, allfonts[0], orintation_ninenty, transfer_mode_normal);
        oled.send_buffer();
        vTaskDelay(1000 / portTICK_RATE_MS);

        oled.clear_framebuffer(0);
        oled.draw_text("Riddha", 127, 31, allfonts[0], orintation_oneeighty, transfer_mode_normal);
        oled.send_buffer();
        vTaskDelay(1000 / portTICK_RATE_MS);

        oled.clear_framebuffer(0);
        oled.draw_text("Riddha", 1, 31, allfonts[0], orintation_twoseventy, transfer_mode_normal);
        oled.send_buffer();
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
