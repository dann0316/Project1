#include <Wire.h>
#include "RAK12033-IIM42652.h"

IIM42652 IMU;

const float filter_alpha = 0.98; // Complementary filter constant
const float alpha_lowpass = 0.1; // Low-pass filter constant

float roll = 0.0;   // Roll angle
float pitch = 0.0;  // Pitch angle

float filtered_acc_x = 0.0; // Filtered accelerometer data
float filtered_acc_y = 0.0;
float filtered_acc_z = 0.0;

float Cal_GyX = 0.0, Cal_GyY = 0.0, Cal_GyZ = 0.0; // Pitch, Roll & Yaw of Gyroscope applied time factor
float acc_pitch, acc_roll, acc_yaw; // Pitch, Roll & Yaw from Accelerometer
float angle_pitch, angle_roll, angle_yaw; // Angle of Pitch, Roll, & Yaw
float complementary_alpha = 0.96; // Complementary constant

struct LorawanSettings {
  bool install_direction;
  // Other properties of LorawanSettings
};

void sendAbnormalSignal() {
  // Code to send the "abnormal" signal, such as triggering an alarm or sending a notification
  // You can customize this function based on your specific requirements
  Serial.println("이상 감지");
}

LorawanSettings g_lorawan_settings; // Declaration and definition of the variable


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
  float temp;

  IMU.get_accel_data(&accel_data);
  IMU.get_gyro_data(&gyro_data);
  IMU.get_temperature(&temp);

  float acc_x = (float)accel_data.x / 2048.0;
  float acc_y = (float)accel_data.y / 2048.0;
  float acc_z = (float)accel_data.z / 2048.0;

  // Apply low-pass filter to accelerometer data
  filtered_acc_x = (alpha_lowpass * filtered_acc_x) + ((1 - alpha_lowpass) * acc_x);
  filtered_acc_y = (alpha_lowpass * filtered_acc_y) + ((1 - alpha_lowpass) * acc_y);
  filtered_acc_z = (alpha_lowpass * filtered_acc_z) + ((1 - alpha_lowpass) * acc_z);

  float gyro_x = (float)gyro_data.x / 2097.2;
  float gyro_y = (float)gyro_data.y / 2097.2;
  float gyro_z = (float)gyro_data.z / 2097.2;

  acc_pitch = atan(acc_y / sqrt(acc_x * acc_x + acc_z * acc_z)) * 57.29577951; // 180 / PI = 57.29577951
  acc_roll = -atan(acc_x / sqrt(acc_y * acc_y + acc_z * acc_z)) * 57.29577951;
  acc_yaw = atan(sqrt(acc_x * acc_x + acc_z * acc_z) / acc_z) * 57.29577951;

  Cal_GyX += gyro_x * 0.000244140625;
  Cal_GyY += gyro_y * 0.000244140625;
  Cal_GyZ += gyro_z * 0.000244140625;

  angle_pitch = complementary_alpha * (gyro_x * 0.000244140625 + angle_pitch) + (1 - complementary_alpha) * acc_pitch;
  angle_roll = complementary_alpha * (gyro_y * 0.000244140625 + angle_roll) + (1 - complementary_alpha) * acc_roll;
  angle_yaw += gyro_z * 0.000244140625;

  float angleX = (int16_t)angle_pitch;
  float angleY = (int16_t)angle_roll;

  if (acc_yaw > 0) {
    if (angleX > 0)
      angleX = 180 - angleX;
    else
      angleX = -(angleX + 180);

    if (angleY > 0)
      angleY = 180 - angleY;
    else
      angleY = -(angleY + 180);
  }

  if (g_lorawan_settings.install_direction == false) {
    if (angleX > 0)
      angleX = 180 - angleX;
    else
      angleX = -(180 + angleX);

    if (angleY > 0)
      angleY = 180 - angleY;
    else
      angleY = -(180 + angleY);
  }

  roll = angle_roll;
  pitch = angle_pitch;

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" degrees  Pitch: ");
  Serial.print(pitch);
  Serial.println(" degrees");

  // Check if Roll or Pitch values exceed the thresholds
  if (roll < -60 || roll > 60 || pitch < -40 || pitch > 40) {
    sendAbnormalSignal(); // Call the function to output the "abnormal" signal
  }



  // Print other sensor data if needed
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" degrees Celsius");

  Serial.print(roll);
  Serial.print("\t");
  Serial.print(pitch);
  Serial.print("\t");
  Serial.println(temp);

  delay(10);
}
