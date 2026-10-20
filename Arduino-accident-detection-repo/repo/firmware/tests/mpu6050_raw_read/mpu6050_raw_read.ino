#include <Wire.h>
#define MPU_ADDR 0x68
void setup() {
  Serial.begin(9600); Wire.begin(); delay(1000);
  Wire.beginTransmission(MPU_ADDR); Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission(true);
  delay(200); Serial.println("MPU6050 raw read");
}
void loop() {
  Wire.beginTransmission(MPU_ADDR); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  if (Wire.available() >= 6) {
    int16_t x = (Wire.read() << 8) | Wire.read();
    int16_t y = (Wire.read() << 8) | Wire.read();
    int16_t z = (Wire.read() << 8) | Wire.read();
    Serial.print("X:"); Serial.print(x); Serial.print(" Y:"); Serial.print(y); Serial.print(" Z:"); Serial.println(z);
  } else Serial.println("No data from sensor");
  delay(300);
}
