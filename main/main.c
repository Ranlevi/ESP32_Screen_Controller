#include "oled.h"
#include "serial_link.h"

#define FW_VERSION "0.2"

//  Configure the UART component, send a startup banner to the
//  browser, and from now on the serial link task will run in 
//  a loop.
void app_main(void)
{
    //Defined in serial_link.h
    const serial_link_cfg_t link_cfg = SERIAL_LINK_DEFAULT_CONFIG();

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
