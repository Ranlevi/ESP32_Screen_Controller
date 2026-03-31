cmake_minimum_required(VERSION 3.16)
project(esp32_tests C)

set(CMAKE_C_STANDARD 11)

# ---------------------------------------------------------------------------
# test_on_rx — tests main.c line-buffer logic
# ---------------------------------------------------------------------------
add_executable(test_on_rx
    unity/unity.c
    stubs/esp_stubs.c
    stubs/freertos_stubs.c
    stubs/oled_stub.c
    stubs/serial_link_stub.c
    stubs/profiler_stub.c
    ../main/main.c
    test_on_rx.c
)
target_include_directories(test_on_rx PRIVATE
    unity/
    stubs/
    ../components/oled/include/
    ../components/serial_link/include/
)

# ---------------------------------------------------------------------------
# test_serial_link — tests serial_link component (real component, stub hardware)
# ---------------------------------------------------------------------------
add_executable(test_serial_link
    unity/unity.c
    stubs/esp_stubs.c
    stubs/freertos_stubs.c
    stubs/uart_stubs.c
    ../components/serial_link/serial_link.c
    test_serial_link.c
)
target_include_directories(test_serial_link PRIVATE
    unity/
    stubs/
    ../components/serial_link/include/
)

# ---------------------------------------------------------------------------
# test_oled — tests oled component (real component, stub hardware)
# ---------------------------------------------------------------------------
add_executable(test_oled
    unity/unity.c
    stubs/esp_stubs.c
    stubs/esp_lcd_stubs.c
    ../components/oled/oled.c
    test_oled.c
)
target_include_directories(test_oled PRIVATE
    unity/
    stubs/
    ../components/oled/include/
)

# ---------------------------------------------------------------------------
# test_profiler — tests profiler component (real component, stub hardware)
# ---------------------------------------------------------------------------
add_executable(test_profiler
    unity/unity.c
    stubs/esp_stubs.c
    stubs/freertos_stubs.c
    stubs/profiler_hw_stubs.c
    ../components/profiler/profiler.c
    test_profiler.c
)
target_include_directories(test_profiler PRIVATE
    unity/
    stubs/
    ../components/profiler/include/
    ../components/serial_link/include/
)
target_compile_definitions(test_profiler PRIVATE PROFILER_TEST_ACCESS)

# ---------------------------------------------------------------------------
# 'make test' runs all four executables in sequence
# ---------------------------------------------------------------------------
add_custom_target(run_all_tests
    COMMAND test_on_rx
    COMMAND test_serial_link
    COMMAND test_oled
    COMMAND test_profiler
    DEPENDS test_on_rx test_serial_link test_oled test_profiler
    COMMENT "Running all test suites"
)
