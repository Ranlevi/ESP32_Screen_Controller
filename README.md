# ESP32 Screen Controller

An ESP32 firmware that displays received serial text on an SSD1306 OLED, and streams live system stats to a browser-based dashboard over the same serial connection.

## Features

- **Serial display** — text received over UART0 is shown on the OLED in real time.
- **System profiler** — a background task periodically sends JSON frames with uptime, heap usage, task count, CPU frequency, IDF/FW version, reset reason, and serial byte counters.
- **OLED stat selector** — the browser can send `OLED:<key>` commands to pin any profiler stat to the display.
- **Task runtime table** — if `CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS` is enabled the dashboard also shows a per-task CPU usage table.
- **Web dashboard** — a single `web_app/index.html` file, opened locally in the browser, connects via the Web Serial API (Chrome / Edge).

## Hardware

Tested on:

| Part | Details |
|------|---------|
| MCU | ESP32 (revision 0) |
| Display | SSD1306 128×64 OLED, I2C address `0x3C` |
| I2C wiring | SDA → GPIO 21, SCL → GPIO 22 |
| Serial | UART0 (USB-to-serial bridge on the dev board), 115200 baud |

## Project structure

```
.
├── main/
│   └── main.c               # App entry point, on_rx() line buffer, OLED mutex
├── components/
│   ├── oled/                # SSD1306 driver — framebuffer, blit, flush
│   ├── profiler/            # Periodic JSON stats task, OLED stat selection
│   └── serial_link/         # UART RX task, write with mutex, stats counters
├── web_app/
│   └── index.html           # Self-contained dashboard (Web Serial API)
└── tests/                   # Host-side Unity test suites (no device needed)
    ├── test_on_rx.c
    ├── test_oled.c
    ├── test_profiler.c
    ├── test_serial_link.c
    └── stubs/               # Lightweight ESP-IDF / FreeRTOS stubs
```

## Building the firmware

Requires [ESP-IDF v6.0](https://github.com/espressif/esp-idf).

```bash
idf.py build
idf.py -p <PORT> flash monitor
```

## Running the tests

The test suite runs on the host (no device required). Requires CMake and a C11 compiler (MinGW on Windows, GCC/Clang on Linux/macOS).

```bash
cd tests
cmake -B build -G "MinGW Makefiles"   # or "Unix Makefiles" on Linux/macOS
cmake --build build
cmake --build build --target run_all_tests
```

Individual suites:

```bash
./build/test_on_rx.exe
./build/test_serial_link.exe
./build/test_oled.exe
./build/test_profiler.exe
```

## Using the dashboard

1. Open `web_app/index.html` in Chrome or Edge (Web Serial requires a Chromium-based browser).
2. Click **Connect** and select the ESP32 serial port.
3. Stat cards update automatically as JSON frames arrive.
4. Select a stat from the **OLED Display** panel and click **Set** to pin it to the physical screen.
5. The collapsible terminal at the bottom shows raw TX/RX traffic; JSON lines are dimmed.
