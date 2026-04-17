# ESP32 Radar System

## Overview

This project builds a sweeping radar using an ESP32, a servo motor, an HC-SR04 ultrasonic sensor, and a passive buzzer. The servo rotates the sensor from 0 to 180 degrees continuously. At each angle the sensor measures the distance to any object ahead. If an object is detected within the set range, the buzzer sounds. Objects that are closer produce a louder, higher-pitched, faster beeping tone. Objects that are farther away produce a quieter, lower-pitched, slower beeping tone. All angle and distance data is also printed to the Serial Monitor for logging or use with a visual radar display.

---

## Hardware Required

- ESP32 development board
- HC-SR04 ultrasonic sensor
- SG90 or MG90S servo motor
- Passive buzzer (not active buzzer)
- Jumper wires
- External 5V power supply or powered breadboard for servo (recommended)
- USB cable for programming

Note: Use a passive buzzer, not an active one. An active buzzer produces only a fixed tone and cannot vary in frequency or volume the way this project requires.

---

## Pin Connections

| Component        | Pin      | ESP32 Pin | Description            |
|------------------|----------|-----------|------------------------|
| HC-SR04          | VCC      | 5V        | Sensor power           |
| HC-SR04          | GND      | GND       | Ground                 |
| HC-SR04          | TRIG     | GPIO 5    | Trigger pulse output   |
| HC-SR04          | ECHO     | GPIO 18   | Echo pulse input       |
| Servo Motor      | VCC      | 5V        | Servo power            |
| Servo Motor      | GND      | GND       | Ground                 |
| Servo Motor      | Signal   | GPIO 13   | PWM control signal     |
| Passive Buzzer   | Positive | GPIO 14   | PWM tone output        |
| Passive Buzzer   | Negative | GND       | Ground                 |

Important: Power the servo from an external 5V supply or a powered rail rather than directly from the ESP32 3.3V pin. Servo motors draw more current than the ESP32 can safely provide and may cause resets or erratic behavior if underpowered.

---

## How It Works

The servo sweeps the ultrasonic sensor back and forth between 0 and 180 degrees, stepping one degree at a time. At each position the HC-SR04 fires a 10-microsecond ultrasonic pulse and listens for the echo. The time taken for the echo to return is converted to a distance in centimeters using the formula: distance = duration x 0.034 / 2.

If the measured distance falls within the detection range (default 100 cm), the buzzer is triggered. The ESP32 LEDC peripheral generates a PWM signal on the buzzer pin. Both the frequency and the duty cycle (volume) are mapped inversely to the distance:

- At close range: high frequency (4000 Hz), high duty cycle (loud), short gap between beeps (fast).
- At far range: low frequency (500 Hz), low duty cycle (quiet), long gap between beeps (slow).

The current angle and distance are also sent over Serial in the format ANGLE:90,DIST:45 so they can be read by a serial plotter, a Processing sketch, or a web-based radar visualizer.

---

## Buzzer Behavior by Distance

| Distance      | Frequency | Volume  | Beep Speed |
|---------------|-----------|---------|------------|
| 0 to 20 cm    | ~4000 Hz  | Loud    | Very fast  |
| 20 to 50 cm   | ~2500 Hz  | Medium  | Fast       |
| 50 to 80 cm   | ~1200 Hz  | Quiet   | Slow       |
| 80 to 100 cm  | ~500 Hz   | Faint   | Very slow  |
| Above 100 cm  | Off       | Silent  | No beep    |

---

## Live Radar Display

This project includes a file called `radar_display.html` — a visual radar monitor you can open directly in your browser. It works in two modes:

**Simulation mode (no hardware needed)**
Just open `radar_display.html` in Google Chrome or Microsoft Edge. The radar will run automatically with simulated objects so you can preview what the display looks like before building anything.

**Live mode (with real ESP32)**
Once your ESP32 is wired and the sketch is uploaded, connect it via USB and click the Connect ESP32 button inside the page. Select the correct COM port and the radar will switch to live data from your real hardware in real time.

> Web Serial only works in Google Chrome or Microsoft Edge. It does not work in Firefox or Safari.

---

## How a Visitor Uses This Project

If someone downloads or clones this repository, here is the exact order of steps they should follow:

**Step 1 — Preview the radar (no hardware required)**
Open `radar_display.html` in Chrome or Edge. The radar runs in simulation mode immediately with no setup needed. Use the range and speed sliders to explore how the display behaves.

**Step 2 — Gather the hardware**
Collect all parts listed in the Hardware Required section. Pay attention to the note about using a passive buzzer and powering the servo from an external 5V supply.

**Step 3 — Wire the circuit**
Follow the Pin Connections table. Double check that the servo VCC goes to an external 5V source, not the ESP32 pin. Share GND between all components and the ESP32.

**Step 4 — Install the Arduino library**
Open Arduino IDE, go to Tools > Manage Libraries, search for ESP32Servo and install it.

**Step 5 — Upload the code**
Open `radar_esp32.ino` in Arduino IDE. Select the correct ESP32 board under Tools > Board and the correct port under Tools > Port. Click Upload.

**Step 6 — Verify in Serial Monitor**
Open Serial Monitor at 115200 baud. You should see lines like `ANGLE:45,DIST:62` scrolling as the servo sweeps. If you see this, everything is working.

**Step 7 — Connect the live radar display**
Go back to `radar_display.html` in Chrome or Edge. Click Connect ESP32, select the port your ESP32 is on, and the radar will switch from simulation to your live hardware data.

---

## Setup Steps

1. Wire all components as shown in the pin connection table.
2. Power the servo from a dedicated 5V rail, not from the ESP32 pin directly.
3. Install the ESP32Servo library in Arduino IDE via Library Manager.
4. Open the radar_esp32.ino file in Arduino IDE.
5. Select your ESP32 board from Tools > Board.
6. Select the correct COM port from Tools > Port.
7. Upload the sketch.
8. Open Serial Monitor at 115200 baud to view angle and distance data.
9. Mount the servo and sensor on a flat base with the sensor facing forward.

---

## Expected Serial Output

```
ANGLE:0,DIST:200
ANGLE:1,DIST:200
ANGLE:45,DIST:62
ANGLE:46,DIST:60
ANGLE:47,DIST:58
ANGLE:90,DIST:23
ANGLE:91,DIST:22
```

---

## Troubleshooting

- If the servo jitters or the ESP32 resets, the servo is drawing too much current. Use a separate 5V power supply for the servo and share only GND with the ESP32.
- If the buzzer makes no sound, confirm you are using a passive buzzer. Active buzzers will not respond to LEDC PWM frequency changes.
- If distance readings are always 200 (the fallback maximum), check that TRIG and ECHO pins are wired correctly and that the sensor VCC is connected to 5V not 3.3V.
- If the sweep is jerky, increase the delay after radarServo.write() to give the servo more time to settle at each angle.

---

## Possible Projects

**Intruder Alarm System**
Mount the radar at a fixed height near a door or window. When any object enters the detection zone, trigger a buzzer alarm and optionally send an alert over Wi-Fi using the ESP32 to a phone or MQTT broker.

**Autonomous Obstacle-Avoiding Robot**
Replace the fixed mounting with a mobile robot chassis. Use the radar sweep to scan ahead before moving. The robot turns away from the direction where the closest object is detected.

**Parking Sensor Assistant**
Mount the sensor at the rear of a vehicle or garage wall. As a car reverses toward the wall, the buzzer beeps faster and louder, mimicking commercial parking sensors.

**3D Room Mapper**
Add a second servo for vertical tilt alongside the horizontal sweep. Log angle-distance pairs over serial and reconstruct a point cloud of the room on a computer using Python or Processing.

**Security Perimeter Monitor**
Use multiple ESP32 radar units placed around a perimeter, all reporting to a central MQTT server. A dashboard shows which unit detected motion and at what distance.

**Wildlife or Pet Motion Tracker**
Set detection sensitivity to a wider range and log timestamped detections to an SD card module. Review movement patterns over time to study animal activity in a yard or farm area.

**Interactive Art Installation**
Drive LED strips in addition to the buzzer. Map distance to LED color and brightness so the display reacts in real time as people walk past or interact with the installation.

---

## License

This project is released for open use. No restrictions apply.
