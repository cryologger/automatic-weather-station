/*
  Title:    Adafruit LSM303AGR Test Code
  Date:     June 29, 2023
  Author:   Adam Garbo

  Sensor:
  - Adafruit LSM303AGR Accelerometer Magnetometer
  - https://www.adafruit.com/product/4413

  Notes:
  - I2C sensor
*/

#include <Adafruit_LSM303_Accel.h>  // https://github.com/adafruit/Adafruit_LSM303_Accel (v1.1.4)
#include <Adafruit_Sensor.h>        // https://github.com/adafruit/Adafruit_Sensor (v1.1.4)

Adafruit_LSM303_Accel_Unified   lsm303 = Adafruit_LSM303_Accel_Unified(54321); // I2C address: 0x1E

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  configureLsm303();

  Serial.println("pitch,roll");
  
}

void loop()
{
  readLsm303();
  delay(500);
}

void configureLsm303()
{
 Serial.print("Info: Initializing LSM303...");

  // Initialize LSM303 accelerometer
  if (lsm303.begin())
  {
    Serial.println("success!");
  }
  else
  {
    Serial.println("failed!");
  }
}

void readLsm303()
{
  // Read accelerometer data
  sensors_event_t accel;
  lsm303.getEvent(&accel);

  // Calculate pitch and roll
  float pitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180 / PI;
  float roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180 / PI;

  Serial.print(pitch, 4); Serial.print(","); Serial.println(roll, 4);
}
