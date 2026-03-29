#include "serial_link.h"

#define FW_VERSION "0.1"

void app_main(void)
{
    const serial_link_cfg_t link_cfg = SERIAL_LINK_DEFAULT_CONFIG();
    const serial_link_stats_t *stats = NULL;
    const char startup_banner[] = "ESP32 Screen Controller FW v" FW_VERSION "\r\n";

    if (serial_link_init(&link_cfg, &stats) != ESP_OK) {
        return;
    }
    (void)stats;

    serial_link_write((const uint8_t *)startup_banner, sizeof(startup_banner) - 1U);
}
