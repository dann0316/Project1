#include <Wire.h>
#include "RAK12033-IIM42652.h"

IIM42652 IMU;

const float alpha = 0.98; // Complementary filter constant
const float alpha_lowpass = 0.1; // Low-pass filter constant

float roll = 0.0;   // Roll angle
float pitch = 0.0;  // Pitch angle

float filtered_acc_x = 0.0; // Filtered accelerometer data
float filtered_acc_y = 0.0;
float filtered_acc_z = 0.0;

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
  IIM42652_axis_t gyro_data;
  
  IMU.get_accel_data(&accel_data);
  IMU.get_gyro_data(&gyro_data);
  
  float acc_x = (float)accel_data.x / 2048.0;
  float acc_y = (float)accel_data.y / 2048.0;
  float acc_z = (float)accel_data.z / 2048.0;

  // Apply low-pass filter to accelerometer data
  filtered_acc_x = (alpha_lowpass * filtered_acc_x) + ((1 - alpha_lowpass) * acc_x);
  filtered_acc_y = (alpha_lowpass * filtered_acc_y) + ((1 - alpha_lowpass) * acc_y);
  filtered_acc_z = (alpha_lowpass * filtered_acc_z) + ((1 - alpha_lowpass) * acc_z);

  float gyro_x = (float)gyro_data.x / 16.4;
  float gyro_y = (float)gyro_data.y / 16.4;
  float gyro_z = (float)gyro_data.z / 16.4;
  
  // Complementary Filter for orientation estimation
  float gyro_angle_x = gyro_x * 0.01; // Integration of gyro data
  float gyro_angle_y = gyro_y * 0.01;
  
  float accel_angle_x = atan2(filtered_acc_y, filtered_acc_z) * 180.0 / PI; // Calculate accelerometer angles using filtered data
  float accel_angle_y = atan2(filtered_acc_x, filtered_acc_z) * 180.0 / PI;

  roll = alpha * (gyro_angle_x) + (1 - alpha) * accel_angle_x;   // Complementary filter update
  pitch = alpha * (gyro_angle_y) + (1 - alpha) * accel_angle_y;

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" degrees  Pitch: ");
  Serial.print(pitch);
  Serial.println(" degrees");

  delay(10);
}
