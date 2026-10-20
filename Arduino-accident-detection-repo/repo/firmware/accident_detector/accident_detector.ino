// Accident Detection - Arduino Nano + MPU6050 (raw I2C, no library needed)
// Wiring: MPU6050 3V3->3V3, GND->GND, SDA->A4, SCL->A5 | Buzzer + -> D9, - -> GND
#include <Wire.h>

#define MPU_ADDR     0x68
#define BUZZER_PIN   9
#define LED_PIN      13

const long ONE_G = 16384;                 // LSB per g at +/-2g range
const long THRESHOLD = 12000;             // deviation from 1g (~0.75 g). Tune this!
const unsigned long ALERT_MS = 7000;      // alarm length (7 s)

bool alertActive = false;
unsigned long alertStart = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  Wire.beginTransmission(MPU_ADDR);       // wake up MPU6050
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  delay(200);
  Serial.println("START");
}

void stopAlarm() {
  alertActive = false;
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);                       // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  if (Wire.available() >= 6) {
    int16_t x = (Wire.read() << 8) | Wire.read();
    int16_t y = (Wire.read() << 8) | Wire.read();
    int16_t z = (Wire.read() << 8) | Wire.read();

    Serial.print("X:"); Serial.print(x);
    Serial.print(" Y:"); Serial.print(y);
    Serial.print(" Z:"); Serial.println(z);

    // total acceleration magnitude vs 1 g (ignores board tilt)
    float mag = sqrt((float)x * x + (float)y * y + (float)z * z);
    long deviation = labs((long)mag - ONE_G);

    if (!alertActive && deviation > THRESHOLD) {
      alertActive = true;
      alertStart = millis();
      Serial.println("ACCIDENT_DETECTED");
    }
  }

  if (alertActive) {
    if (millis() - alertStart >= ALERT_MS) {
      stopAlarm();
      Serial.println("ALERT_RESET");
    } else {
      digitalWrite(BUZZER_PIN, HIGH);
      digitalWrite(LED_PIN, ((millis() - alertStart) / 250) % 2 == 0);  // blink LED
    }
  }

  // Commands from laptop: N = false alarm -> stop now, Y = confirmed -> let alarm finish
  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (alertActive && c == 'N') { stopAlarm(); Serial.println("FALSE_ALERT"); }
    if (alertActive && c == 'Y') { Serial.println("REAL_ACCIDENT"); }
  }
  delay(100);
}
