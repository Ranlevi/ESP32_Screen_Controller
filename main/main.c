#include <string.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "oled.h"
#include "profiler.h"
#include "serial_link.h"

#define OLED_CMD_PREFIX     "OLED:"
#define OLED_CMD_PREFIX_LEN 5   // strlen("OLED:")

#define FW_VERSION "0.5"

// 128px wide / 8px per char = 16 characters per row
#define DISPLAY_COLS 16

//  Derived: large enough for a display line OR a full OLED: command.
//  If PROFILER_OLED_KEY_MAX_LEN grows, this grows with it automatically.
#define LINE_BUF_LEN (OLED_CMD_PREFIX_LEN + PROFILER_OLED_KEY_MAX_LEN)

static const char *TAG = "main";

/*
    Note: Mutex use in clear+show sequences.
    Both on_rx() and oled_dispaly_stat() can call the screen API
    to write to it. oled_display_stat() has higher priority than
    on_rx(), so it can take over and write to the frame buffer while
    on_rx() does - causing dispaly errors. 
    Solution: we use a mutex to prevent such interleaving. If on_rx()
    locks the mutex, the RTOS schedular will put the profiler task (which
    calls oled_display_stat()) into the blocked state, and will let
    on_rx() complete the actions.
*/
static SemaphoreHandle_t s_oled_lock;

static char   s_line_buf[LINE_BUF_LEN + 1];
static size_t s_line_len = 0;

//  Accumulates received bytes into a line buffer.
//  Flushes to the OLED on newline or when the line is full.
void on_rx(const uint8_t *data, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        char c = (char)data[i];

        if (c == '\r') {
            continue;
        }

        if (c == '\n') {
            s_line_buf[s_line_len] = '\0';

            if (strncmp(s_line_buf, OLED_CMD_PREFIX, OLED_CMD_PREFIX_LEN) == 0) {
                //This data should not be printed to screen - it's a message
                //to the profiler.
                profiler_set_oled_stat(s_line_buf + OLED_CMD_PREFIX_LEN);
            } else {
                //This data should be printed to the scree.
                xSemaphoreTake(s_oled_lock, portMAX_DELAY);
                esp_err_t err = oled_clear();
                if (err != ESP_OK) {
                    ESP_LOGW(TAG, "oled_clear failed: %s", esp_err_to_name(err));
                }
                err = oled_show_text(0, 0, s_line_buf);
                if (err != ESP_OK) {
                    ESP_LOGW(TAG, "oled_show_text failed: %s", esp_err_to_name(err));
                }
                xSemaphoreGive(s_oled_lock);
            }

            s_line_len = 0;
            continue;
        }

        if (s_line_len < LINE_BUF_LEN) {
            s_line_buf[s_line_len++] = c;
        }

        //  Flush display text when the row is full — but not if the buffer is
        //  accumulating an OLED: command (those may be longer than DISPLAY_COLS).
        if (s_line_len >= DISPLAY_COLS &&
            strncmp(s_line_buf, OLED_CMD_PREFIX, OLED_CMD_PREFIX_LEN) != 0) {
            s_line_buf[s_line_len] = '\0';
            xSemaphoreTake(s_oled_lock, portMAX_DELAY);
            esp_err_t err = oled_clear();
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "oled_clear failed: %s", esp_err_to_name(err));
            }
            err = oled_show_text(0, 0, s_line_buf);
            if (err != ESP_OK) {
                ESP_LOGW(TAG, "oled_show_text failed: %s", esp_err_to_name(err));
            }
            xSemaphoreGive(s_oled_lock);
            s_line_len = 0;
        }
    }
}

//  Resets the line buffer state. Called by tests between test cases.
void on_rx_reset(void)
{
    s_line_len = 0;
    s_line_buf[0] = '\0';
}

//  Displays a stat on the OLED: label on the top row, value on the bottom row.
//  Called from the profiler task every interval when a stat is selected.
static void oled_display_stat(const char *label, const char *value)
{
    xSemaphoreTake(s_oled_lock, portMAX_DELAY);
    oled_clear();
    oled_show_text(0,  0, label);
    oled_show_text(0, 32, value);
    xSemaphoreGive(s_oled_lock);
}

//  Configure the UART component, send a startup banner to the
//  browser, and from now on the serial link task will run in
//  a loop.
void app_main(void)
{
    s_oled_lock = xSemaphoreCreateMutex();
    if (s_oled_lock == NULL) {
        return;
    }

    serial_link_cfg_t link_cfg = SERIAL_LINK_DEFAULT_CONFIG();
    link_cfg.rx_callback = on_rx;

    const serial_link_stats_t *stats = NULL;
    const char startup_banner[]      = "ESP32 Screen Controller FW v" FW_VERSION "\r\n";

    if (serial_link_init(&link_cfg, &stats) != ESP_OK) {
        return;
    }


    serial_link_write((const uint8_t *)startup_banner, sizeof(startup_banner) - 1U);

    if (oled_init() != ESP_OK) {
        return;
    }

    oled_show_text(0, 0, "Hello World");

    profiler_cfg_t profiler_cfg = PROFILER_DEFAULT_CONFIG();
    profiler_cfg.write_fn     = serial_link_write;
    profiler_cfg.serial_stats = stats;
    profiler_cfg.oled_fn      = oled_display_stat;
    profiler_cfg.fw_version   = FW_VERSION;
    profiler_init(&profiler_cfg);
}
