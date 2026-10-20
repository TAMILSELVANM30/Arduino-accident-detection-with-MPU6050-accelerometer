# Troubleshooting log (what went wrong and the fix)
| Problem | Cause | Fix |
|---|---|---|
| `unable to open port COM3` / `Access is denied` | Serial Monitor, another IDE window or the Python script holding the port | Close Serial Monitor before upload; only one program per COM port; replug USB |
| `not in sync: resp=0xe0` | Wrong bootloader / speed (Nano clone) | Processor = ATmega328P (Old Bootloader); try 57600, then 115200 |
| `MPU6050.h: No such file` / `no member named begin` | Library missing or a different library API | Skip the library; use raw `Wire.h` code (this repo) |
| Serial Monitor shows old output | Upload failed silently | Re-upload, close/reopen monitor, check baud 9600 |
| I2C scanner: no devices | MPU6050 powered from 5V (dim LED), loose wires | Use **3V3** pin, reseat wires; LED should be bright |
| Readings all `-1` or `0` | Sensor not woken from sleep / bad contact | Write 0x00 to register 0x6B; re-test wiring |
| Alarm at rest | Threshold applied to raw axis values (gravity ≈ 16384 / tilt gives 14000) | Use magnitude deviation from 1 g (final firmware) |
| Dividing values gave 0 | Integer division made values tiny | Do not scale; tune the threshold instead |
| `python: can't open file` | File saved as `.txt` | Rename to `.py` (enable file extensions in Explorer) |
| Email not sent | No App Password / 2-Step off | Enable 2-Step Verification, create App Password |
