idf_component_register(
    SRCS "serial_link.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_driver_uart freertos
)
