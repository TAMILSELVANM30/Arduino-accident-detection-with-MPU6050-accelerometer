# 🚗 Arduino Accident Detection with MPU6050

A low-cost accident detection prototype. An **Arduino Nano** reads an **MPU6050 accelerometer**. On a violent shock it sounds a **buzzer**, blinks an **LED** and notifies a **laptop dashboard**. The dashboard asks the user to confirm. If the accident is real, it sends an **email alert with the location**.

## Abstract
Road accidents are often made worse by delayed emergency response. This project is a minimum viable prototype (MVP) of an automatic accident alert system. An MPU6050 3-axis accelerometer continuously measures acceleration, and an Arduino Nano compares the total acceleration with a crash threshold. When a crash-like shock is detected, the Nano starts a 7-second local alarm (buzzer and LED) and sends an `ACCIDENT_DETECTED` message over USB serial to a Python dashboard on a laptop. The dashboard shows the X/Y/Z values and asks the user for **Y/N** confirmation within 6 seconds. **N** (false alarm) stops the alarm immediately. **Y**, or no response, treats the event as a real accident and sends an email containing the time, sensor values and a static GPS location. A GSM or GPS module was left out because of time and budget, so the location is a fixed coordinate. The design is modular, so the SIM800L, GPS and SD-card parts can be added later.

## Hardware
| Component | Purpose |
|---|---|
| Arduino Nano (ATmega328P, old bootloader) | Main controller |
| MPU6050 / GY-521 | Accelerometer + gyroscope |
| Active buzzer | Audible alarm |
| Onboard LED (D13) | Visual alarm |
| USB cable (data) | Power and serial link to laptop |
| *(optional, future)* SD card module | Log accidents |
| *(optional, future)* SIM800L / NEO-6M | SMS and real GPS |

## Wiring
| MPU6050 | Nano |
|---|---|
| 3V3 | 3V3 |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

| Buzzer | Nano |
|---|---|
| + | D9 |
| − | GND |

See `docs/wiring.svg`. **Use the 3V3 pin.** On the board used here the 5V pin gave a dim LED and no I²C response.

## How it works
```
MPU6050 --I2C--> Arduino Nano --USB serial--> Python dashboard --SMTP--> Email (+location)
                    |  buzzer + LED (7 s)           ^
                    +-------------------------------+  Y / N reply
```
1. The Nano reads raw X/Y/Z (±2 g, 16384 LSB = 1 g) every 100 ms.
2. The detector computes the total acceleration magnitude. At rest it is about 1 g, so a crash is a large **deviation from 1 g**. This is why tilting the board no longer causes false alarms.
3. When the deviation exceeds `THRESHOLD`, the Nano prints `ACCIDENT_DETECTED` and runs the alarm for 7 s.
4. The dashboard asks **"Real accident? Y/N"** with a 6 s timeout.
   - **N** sends `N` to the Nano, which stops the alarm immediately (false alert).
   - **Y** or timeout confirms the accident, sends the email and lets the alarm run out.

## Setup
**Arduino**
1. Arduino IDE → Board: *Arduino Nano*, Processor: *ATmega328P (Old Bootloader)*, Upload speed 57600 (try 115200 if it fails).
2. Open `firmware/accident_detector/accident_detector.ino` and upload.
3. **Close the Serial Monitor before running the dashboard**, because only one program can use COM3.

**Laptop**
```bash
cd dashboard
pip install -r requirements.txt
copy config.example.py config.py     # Windows (use cp on Linux/Mac)
# edit config.py: COM port, email, Gmail App Password, coordinates
python dashboard.py
```
Gmail App Password: Google Account → Security → 2-Step Verification → App passwords.

## Testing
1. `firmware/tests/i2c_scanner`: should print `Device found at 0x68`.
2. `firmware/tests/mpu6050_raw_read`: values should change when you move the board (Z ≈ ±16384 when flat).
3. Run the main firmware. Shake it hard to trigger the alarm and the dashboard prompt.

## Results
- I²C detection at 0x68 confirmed after fixing the power pin (3V3).
- Resting values with the board tilted reached ±14000, so a raw per-axis threshold caused false alarms. The final design uses the deviation of total acceleration from 1 g.
- The end-to-end flow (detect → buzzer/LED → Y/N → email with location) was demonstrated.

## Troubleshooting (lessons learned)
See `docs/troubleshooting.md`. Main issues were COM3 "Access is denied", "not in sync" bootloader errors, library mismatches, all `-1` or `0` readings, and wrong power pin.

## Future work
SD-card logging, SIM800L SMS, NEO-6M GPS, ESP32 with WiFi and a phone app, gyroscope-based rollover detection, and a standalone enclosure with its own battery.

## Repo layout
```
firmware/accident_detector/   main Arduino sketch
firmware/tests/               I2C scanner + raw MPU6050 read
dashboard/                    Python dashboard + config template
docs/                         wiring diagram, troubleshooting, timeline
```
## Author
TAMILSELVAN M, DR.NGPIT
