# ESP32 Screen Controller

## Project Purpose

This repository is for a public MCU showcase project intended to demonstrate professional embedded software engineering skills.

## High Level Project Description

The project will deliver firmware for an ESP WROOM 32 module. The module will be connected to an OLED screen (exact model still TBD) via I2C, and also connected to a browser-based web app running in Chrome which will communicate with the device via Web Serial. The UI will allow the user to control what appears on the screen and to observe system behavior in real time.

## Folder Structure and important files

- main/main.c - firmware entry point.

## Hardware

- Module: ESP WROOM 32.

- Chip: ESP32-D0WD-V3

- USB-To-Serial Chip: CP2102

- Board Pinout count: 38

- Embedded Flash size: 4MB

- ROM Size: 448KB

- SRAM Size: 520KB

- Crystal frequency:  40MHz

- MAC: fc:b4:67:f1:4e:e0

- SSD1306 OLED display.

## Software

- ESP-IDF Version 6.0

- Host: Windows 11

## OLED Display
The OLED display is connected to the board via I2C, with the following pinout:
- 3.3V
- GND
- SCL: GPIO22
- SDA: GPIO21

## Planned Features

The system is expected to support the following user-visible capabilities:

- Configurable hourglass visualization.
- Scrolling text messages.
- Selectable animations such as bouncing ball, rain, and similar effects, with adjustable parameters.
- Live RTOS telemetry display, such as CPU load, task/runtime statistics, memory usage, and related health indicators.
- Free-draw mode for direct real-time LED control from the browser.

## General Instructions

This is a showcase project, not a quick prototype. All work should favor clarity, robustness, and demonstrable engineering quality over shortcuts.

When making implementation decisions, prefer approaches that visibly communicate:

- Strong embedded systems fundamentals.
- Clean modular architecture.
- Good hardware/software interface design.
- Careful resource management.
- Testability and debuggability.
- Reliable host-device communication.
- Thoughtful tradeoff analysis.
- Good documentation.

# 
