#include "oled.h"
#include "oled_stub.h"

#include <string.h>

static char s_last_text[64];
static int  s_clear_count;

esp_err_t oled_init(void)
{
    return ESP_OK;
}

esp_err_t oled_clear(void)
{
    s_clear_count++;
    return ESP_OK;
}

esp_err_t oled_show_text(int col, int row, const char *text)
{
    (void)col;
    (void)row;
    strncpy(s_last_text, text, sizeof(s_last_text) - 1);
    s_last_text[sizeof(s_last_text) - 1] = '\0';
    return ESP_OK;
}

esp_err_t oled_blit_text(int col, int row, const char *text)
{
    //  In tests, blit behaves the same as show_text — captures the last text.
    return oled_show_text(col, row, text);
}

esp_err_t oled_flush(void)
{
    return ESP_OK;
}

const char *stub_last_text(void)   { return s_last_text; }
int         stub_clear_count(void) { return s_clear_count; }
void        stub_reset(void)       { s_last_text[0] = '\0'; s_clear_count = 0; }
