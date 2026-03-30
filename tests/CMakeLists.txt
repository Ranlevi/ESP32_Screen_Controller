cmake_minimum_required(VERSION 3.16)
project(esp32_tests C)

set(CMAKE_C_STANDARD 11)

add_executable(test_runner
    # Unity framework
    unity/unity.c

    # Stubs (fake ESP-IDF implementations)
    stubs/esp_stubs.c
    stubs/oled_stub.c
    stubs/serial_link_stub.c

    # Code under test
    ../main/main.c

    # Tests
    test_on_rx.c
)

target_include_directories(test_runner PRIVATE
    unity/                              # unity.h
    stubs/                              # esp_err.h, esp_log.h, oled_stub.h
    ../components/oled/include/         # oled.h
    ../components/serial_link/include/  # serial_link.h
)
