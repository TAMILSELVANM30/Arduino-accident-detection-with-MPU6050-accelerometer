#include <Wire.h>
void setup() { Serial.begin(9600); Wire.begin(); delay(1000); Serial.println("I2C Scanner"); }
void loop() {
  int found = 0;
  for (int a = 1; a < 127; a++) {
    Wire.beginTransmission(a);
    if (Wire.endTransmission() == 0) { Serial.print("Device found at 0x"); Serial.println(a, HEX); found++; }
  }
  if (!found) Serial.println("NO I2C devices - check wiring/power (use 3V3)");
  Serial.println("---"); delay(2000);
}
