#include <string.h>

#include "esp_log.h"
#include "oled.h"
#include "serial_link.h"

#define FW_VERSION "0.2"

// 128px wide / 8px per char = 16 characters per row
#define DISPLAY_COLS 16

static const char *TAG = "main";

static char   s_line_buf[DISPLAY_COLS + 1];
static size_t s_line_len = 0;

//  Accumulates received bytes into a line buffer.
//  Flushes to the OLED on newline or when the line is full.
static void on_rx(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        char c = (char)data[i];

        if (c == '\r') {
            continue;
        }

        if (c == '\n' || s_line_len >= DISPLAY_COLS) {
            s_line_buf[s_line_len] = '\0';
            esp_err_t err = oled_clear();
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "oled_clear failed: %s", esp_err_to_name(err));
            }
            err = oled_show_text(0, 0, s_line_buf);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "oled_show_text failed: %s", esp_err_to_name(err));
            }
            s_line_len = 0;
            if (c == '\n') {
                continue;
            }
        }

        s_line_buf[s_line_len++] = c;
    }
}

//  Configure the UART component, send a startup banner to the
//  browser, and from now on the serial link task will run in
//  a loop.
void app_main(void)
{
    serial_link_cfg_t link_cfg = SERIAL_LINK_DEFAULT_CONFIG();
    link_cfg.rx_callback = on_rx;

    const serial_link_stats_t *stats = NULL;
    const char startup_banner[]      = "ESP32 Screen Controller FW v" FW_VERSION "\r\n";

    if (serial_link_init(&link_cfg, &stats) != ESP_OK) {
        return;
    }

    (void)stats; //temporary cast to void, to prevent warnings.

    serial_link_write((const uint8_t *)startup_banner, sizeof(startup_banner) - 1U);

    if (oled_init() != ESP_OK) {
        return;
    }

    oled_show_text(0, 0, "Hello World");
}
