idf_component_register(
    SRCS "profiler.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_system esp_timer freertos serial_link
)
