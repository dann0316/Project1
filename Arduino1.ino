#include <Wire.h>
#include "RAK12033-IIM42652.h"

IIM42652 IMU;

void setup() {
  Wire.begin();
  Serial.begin(115200);
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize the RAK12033 module.");
    while (1);
  }

  IMU.accelerometer_enable();
  IMU.gyroscope_enable();
  IMU.temperature_enable();
}

void loop() {
  IIM42652_axis_t accel_data;
  
  IMU.get_accel_data(&accel_data);
  
  float acc_x = (float)accel_data.x / 2048.0;
  float acc_y = (float)accel_data.y / 2048.0;
  float acc_z = (float)accel_data.z / 2048.0;
  
  Serial.print("Accel X: ");
  Serial.print(acc_x);
  Serial.print(" g  Y: ");
  Serial.print(acc_y);
  Serial.print(" g  Z: ");
  Serial.print(acc_z);
  Serial.println(" g");

  delay(1000);
}
