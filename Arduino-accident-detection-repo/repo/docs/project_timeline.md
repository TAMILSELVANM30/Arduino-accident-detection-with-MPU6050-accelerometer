# Development timeline
1. MVP plan: Nano + MPU6050, threshold detection, LED alert, Y/N on laptop.
2. Fixed upload issues (board, bootloader, COM port).
3. Library problems -> switched to raw I2C register reads.
4. Debugged sensor: wrong power pin, wake-up register, replaced module.
5. Simulation mode (typed X,Y,Z values) to prove the logic without a sensor.
6. Buzzer + LED alarm; 7 s duration.
7. Python dashboard: live values, Y/N confirmation, email with static coordinates.
8. Threshold tuning, then switched to magnitude-deviation detection.
9. Future: SD logging, GSM/GPS, mobile app.
