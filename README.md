# ESP32 Ultrasonic Radar

[![Platform](https://img.shields.io/badge/Platform-ESP32-00979D?logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32)
[![Firmware](https://img.shields.io/badge/Firmware-Arduino%20C%2B%2B-00599C?logo=arduino&logoColor=white)](https://docs.arduino.cc/)
[![Display](https://img.shields.io/badge/Display-HTML5%20%2B%20Canvas-E34F26?logo=html5&logoColor=white)](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)

A small ESP32 project that sweeps an HC-SR04 ultrasonic sensor across a servo's horizontal arc, reports angle/distance readings over serial, and changes a passive buzzer's tone and duty cycle when an object is within the configured range. A standalone browser page provides a simulated radar view and can read the ESP32 serial stream through the Web Serial API.

This is a learning prototype, not a safety-rated detection system or a true radio-frequency radar. Ultrasonic measurements depend on target shape, angle, surface and environment.

## Features

- Servo sweep between approximately 0° and 180°.
- HC-SR04 distance measurement with a 30 ms echo timeout and a 200 cm fallback value.
- Serial records in the format `ANGLE:<degrees>,DIST:<cm>`.
- Distance-dependent passive-buzzer frequency, duty cycle and beep interval.
- Browser radar visualization with simulation mode, pause/resume, range/speed controls and optional live serial input.
- No external JavaScript framework or remote asset is required by the HTML page.

## Repository files

The filenames below are the actual names currently stored in the repository, including spaces.

| File | Purpose |
|---|---|
| [`radar esp32 .ino`](./radar%20esp32%20.ino) | ESP32 firmware for the servo, ultrasonic sensor, buzzer and serial output |
| [`radar display monitor.html`](./radar%20display%20monitor.html) | Browser radar display; supports simulated data and live ESP32 serial input |
| [`README.md`](./README.md) | Wiring, setup, limitations and maintenance notes |

## Hardware and bill of materials

| Item | Status | Notes |
|---|---|---|
| ESP32 development board | Required | Arduino-compatible board with GPIO 5, 13, 14 and 18 available |
| HC-SR04 ultrasonic module | Required | Powered from 5 V in the documented wiring |
| SG90/MG90S-compatible servo | Required | Confirm the servo's voltage/current requirements |
| Passive piezo buzzer | Required for the documented variable-tone behavior | An active buzzer generally cannot reproduce the firmware's changing pitch as intended |
| Regulated 5 V supply for the servo | Strongly recommended | Size it for the servo's stall/startup current; do not power the servo from the ESP32 3.3 V pin |
| 1 kΩ and 2 kΩ resistors, or a suitable logic-level shifter | Required for direct HC-SR04-to-ESP32 ECHO wiring | Forms a divider to reduce the nominal 5 V ECHO signal to about 3.3 V |
| Jumper wires and common ground | Required | Join ESP32, sensor and servo supply grounds |
| USB data cable | Required for programming and live serial display | A charge-only cable will not provide serial data |

The resistor values above are one practical divider option, not a precision component requirement. A dedicated 5 V-to-3.3 V logic-level shifter is also suitable.

## Wiring

| Component pin | Connect to | Notes |
|---|---|---|
| HC-SR04 VCC | Regulated 5 V | Sensor supply |
| HC-SR04 GND | Common GND | Must connect to ESP32 GND |
| HC-SR04 TRIG | ESP32 GPIO 5 | Trigger output |
| HC-SR04 ECHO | ESP32 GPIO 18 **through a divider/level shifter** | Do not connect the nominal 5 V ECHO output directly to the ESP32 |
| Servo signal | ESP32 GPIO 13 | Control signal |
| Servo VCC | Appropriate regulated servo supply, typically 5 V | Separate supply recommended |
| Servo GND | Common GND | Grounds must be shared |
| Passive buzzer + | ESP32 GPIO 14, if the buzzer is a low-current piezo type | Use a transistor driver if the buzzer's current exceeds safe GPIO limits |
| Passive buzzer − | GND | Common ground |

**Important electrical safety:** ESP32 GPIOs are not 5 V tolerant. The HC-SR04 ECHO line is typically 5 V logic when the sensor is powered from 5 V; level-shift it before GPIO 18. Do not connect the servo's power rail to the ESP32 3.3 V output. If using a separate servo supply, connect its ground to the ESP32 ground so the signal has a reference.

## Software requirements

- Arduino IDE and ESP32 board support.
- The **ESP32Servo** library.
- A compatible Arduino-ESP32 core version.

The firmware uses the legacy LEDC functions `ledcSetup()`, `ledcAttachPin()` and `ledcChangeFrequency()`, which are from the Arduino-ESP32 2.x API. Arduino-ESP32 3.x changed the LEDC API, so this sketch may need a code migration before it will compile with core 3.x. The repository does not pin a toolchain or include a build workflow.

For live browser input, use a Chromium-based browser such as Google Chrome or Microsoft Edge with Web Serial support. Browser support and secure-context rules can vary; if opening the local file does not expose Web Serial, serve the folder from `localhost` and reopen the page.

## Setup

### 1. Preview the browser display without hardware

1. Open [`radar display monitor.html`](./radar%20display%20monitor.html) in Chrome or Edge.
2. The page starts in simulation mode and animates simulated objects.
3. Try the range and speed controls and the Pause/Resume button. These controls affect the browser simulation; they do not reconfigure the ESP32 firmware.

### 2. Assemble and upload the hardware firmware

1. Build the wiring from the table above, including the ECHO level shifter and common ground.
2. Install ESP32 board support and the ESP32Servo library in Arduino IDE.
3. The repository sketch is named `radar esp32 .ino`, which does not follow the usual Arduino sketch-folder naming convention. Create a folder named `radar_esp32`, copy the source into it, and name the copy `radar_esp32.ino`.
4. Open `radar_esp32/radar_esp32.ino`, select the correct ESP32 board and serial port, then upload.
5. Open Serial Monitor at **115200 baud** and confirm that angle/distance records are emitted as the servo sweeps.

### 3. Connect the live browser view

1. Close Serial Monitor or any other application that currently owns the ESP32 serial port.
2. Open the browser display page and click **Connect ESP32**.
3. Select the ESP32 serial port and approve the browser's access prompt.
4. The page expects lines in the format `ANGLE:90,DIST:45` at 115200 baud. Disconnect/reload the page to recover if the serial reader exits or the device is unplugged.

## Firmware behavior

- The servo is commanded from 0° to 180° in 1° steps, with a short settling delay before each distance reading.
- The distance formula in the source is approximately `duration × 0.034 / 2` centimeters.
- If no echo is received within 30 ms, the firmware reports the configured fallback of 200 cm.
- The buzzer is activated when measured distance is below `DETECT_RANGE` (100 cm in the current source). Frequency and duty cycle are mapped from distance, with an 80 ms tone and a distance-dependent silence interval.
- The browser page independently draws a simulated or live sweep and generates its own low-volume Web Audio tone when it detects an object. The browser sound is not the same as the physical buzzer output.

The example records below are illustrative only, not evidence of a successful hardware test:

```text
ANGLE:0,DIST:200
ANGLE:45,DIST:62
ANGLE:90,DIST:23
```

## Controls and limitations

- **Hardware detection range:** set by `DETECT_RANGE` in the firmware. The browser's Range slider changes the display/simulation range and filters live display values; it does not change the firmware constant.
- **Simulation sweep speed:** the browser Speed slider affects simulation only. The physical servo's sweep rate is determined by the firmware loop and delay calls.
- **Measurement quality:** HC-SR04 readings can be unreliable for soft, angled, narrow or acoustically absorbent targets, and at very short distances. The 200 cm timeout fallback is not a real measurement.
- **Sweep timing:** the buzzer routine contains blocking delays, so the servo sweep pauses while tones play.
- **Mechanical setup:** secure the servo and sensor, keep the sensor's view clear, and avoid forcing the servo against its mechanical stops.
- **Not a safety device:** do not use this prototype for collision avoidance, security guarantees, medical applications or other safety-critical decisions.

## Troubleshooting

| Symptom | Checks |
|---|---|
| ESP32 resets when the servo moves | Use a suitable separate regulated servo supply and join grounds; check current capacity and wiring. |
| Risk of GPIO damage / erratic ECHO readings | Verify the HC-SR04 ECHO line passes through a divider or level shifter before GPIO 18. |
| Distance stays at 200 cm | Check sensor 5 V/GND, TRIG GPIO 5, ECHO divider and GPIO 18, and the target position. |
| Servo does not move | Check servo signal on GPIO 13, power, shared ground and library/core compatibility. |
| Sketch fails to compile around LEDC functions | The source uses the Arduino-ESP32 2.x LEDC API; use a compatible 2.x core or port the sketch to the 3.x API. |
| Buzzer is silent or only gives one pitch | Confirm GPIO 14 wiring, common ground and that the device is a suitable passive piezo buzzer. |
| Browser cannot connect | Use Chrome/Edge with Web Serial support, grant port permission, close Serial Monitor, and check that the firmware sends the expected line format. |
| Live display shows no target beyond the slider range | The page filters live readings against its own Range value; this does not change the firmware's 100 cm buzzer threshold. |

## License and maintenance

- **License:** no `LICENSE` file is present, so reuse and redistribution terms are not formally declared. Add a license file if you want to grant explicit permissions.
- **Verification:** source and documentation review only for this pass. Firmware compilation, browser-to-device serial connection and physical hardware behavior have not been verified.
- **Last updated:** 11 November 2026 — corrected actual filenames, setup instructions, ESP32 ECHO voltage safety, power guidance and firmware/browser limitations.
- **Maintenance:** update the pin table, voltage protection, BOM and core/library compatibility notes whenever wiring or firmware changes.
