// ----------------------------------------------------------------------------
// Adafruit DPS310 Precision Barometric Pressure Sensor
// ----------------------------------------------------------------------------
void configureDps310()
{
  DEBUG_PRINT("Info: Initializing DPS310...");

  if (dps310.begin_I2C())
  {
    online.dps310 = true;
    DEBUG_PRINTLN("success!");
    dps310.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps310.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
  }
  else
  {
    online.dps310 = false;
    DEBUG_PRINTLN("failed!");
  }
}

// Read DPS310
void readDps310()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor(s)
  configureDps310();

  // Check if sensor initialized successfully
  if (online.dps310)
  {
    DEBUG_PRINT("Info: Reading DPS310...");

    sensors_event_t temp_event, pressure_event;

    // Read sensor until value is returned or timeout is exceeded
    while ((!dps310.temperatureAvailable() || !dps310.pressureAvailable()) && millis() - loopStartTime < 5000UL)
    {
      return;
    }

    // Read sensor data
    dps310.getEvents(&temp_event, &pressure_event);
    float temperatureInt = temp_event.temperature;

    // Add to statistics object
    temperatureIntStats.add(temperatureInt);
    //.add();

    // Write data to union
    moSbdMessage.temperatureInt = temperatureInt * 100;

    DEBUG_PRINTLN("done.");
  }
  else
  {
    DEBUG_PRINTLN("Warning: DPS310 offline!");
  }
  // Stop the loop timer
  timer.dps310 = millis() - loopStartTime;

}

// ----------------------------------------------------------------------------
// Adafruit LSM303AGR Accelerometer/Magnetomter
// ----------------------------------------------------------------------------
void configureLsm303()
{
  DEBUG_PRINT("Info: Initializing LSM303...");

  // Initialize LSM303 accelerometer
  if (lsm303.begin())
  {
    online.lsm303 = true;
    DEBUG_PRINTLN("success!");
  }
  else
  {
    online.lsm303 = false;
    DEBUG_PRINTLN("failed!");
  }
}

void readLsm303()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Initialize accelerometer
  configureLsm303();

  // Check if sensor initialized successfully
  if (online.lsm303)
  {
    DEBUG_PRINT("Info: Reading LSM303...");

    myDelay(500);

    // Read accelerometer data
    sensors_event_t accel;
    lsm303.getEvent(&accel);

    // Calculate pitch and roll
    float pitch = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180 / PI;
    float roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180 / PI;

    // Write data to union
    moSbdMessage.pitch = pitch * 100;
    moSbdMessage.roll = roll * 100;

    // Add to statistics object
    //.add();
    //.add();

    DEBUG_PRINTLN("done.");
  }
  else
  {
    DEBUG_PRINTLN("Warning: LSM303 offline!");
  }

  // Stop loop timer
  timer.lsm303 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------

// Vaisala HMP60 Temperature/Relative Humidity
// Brown  5-28 VDC
// White  Channel 1 RH 0-1V
// Blue   GND
// Black  Channel 2 T 0-1V
// ----------------------------------------------------------------------------
void readHmp60()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  DEBUG_PRINT("Info: Reading HMP60...");

  // Note: A startup delay of 4 s is recommended at 13.5 V and 2 s at 5 V
  myDelay(4000);

  // Perform analog readings
  float temperatureExt = analogRead(PIN_TEMP);
  float humidityExt = analogRead(PIN_HUMID);

  // Map voltages to sensor ranges
  temperatureExt = mapFloat(temperatureExt, 0, 1240, -40, 60); // Map temperature from 0-1 V to -40-60°C
  humidityExt = mapFloat(humidityExt, 0, 1240, 0, 100);        // Map humidity 0-1 V to 0-100

  //Serial.print(F("temperatureExt: ")); Serial.println(temperatureExt);
  //Serial.print(F("humidityExt: ")); Serial.println(humidityExt);

  // Add to statistics object
  temperatureExtStats.add(temperatureExt);
  humidityExtStats.add(humidityExt);

  DEBUG_PRINTLN("done.");

  // Stop loop timer
  timer.hmp60 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// R.M. Young Wind Monitor 5103L (4-20 mA)
// 150 Ohm 0.1% resistor
// Voltage range: 0.5995 - 2.9675 V
// WS+  Black
// WS-  Red
// WD+  White
// WD-  Green
// ----------------------------------------------------------------------------
void readAnemometer()
{
  unsigned int loopStartTime = millis();

  DEBUG_PRINT("Info: Reading anemometer...");

  //for (int i = 0; i < 5; i++)
  //{
  // Measure wind speed
  float sensorValue1 = analogRead(PIN_WIND_SPEED); // Raw analog wind speed value

  // Map wind speed to 0-100 m/s
  windSpeed = mapFloat(sensorValue1, 741, 3671, 0, 100);

  // Measure wind direction
  float sensorValue2 = analogRead(PIN_WIND_DIR); // Raw analog wind direction value

  // Map wind direction to 0-360°
  windDirection = mapFloat(sensorValue2, 741, 3671, 0, 360);

  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  DEBUG_PRINTLN("done.");

  Serial.print(F("windSpeed: ")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.println(windSpeed, 2);
  Serial.print(F("windDirection: ")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.println(windDirection, 2);
  //myDelay(500);
  //}


  // Determine wind gust and direction
  if ((windSpeed > 0) && (windSpeed > windGust))
  {
    windGust = windSpeed;
    windGustDirection = windDirection;
  }

  // Calculate wind speed and direction vectors
  float windDirectionRadians = windDirection * DEG_TO_RAD;    // Convert wind direction from degrees to radians
  float vn1 = -1.0 * windSpeed * cos(windDirectionRadians);   // Magnitude of the north-south component (v) of the resultant vector mean wind
  float ve1 = -1.0 * windSpeed * sin(windDirectionRadians);   // Magnitude of the east-west component (u) of the resultant vector mean wind

  // Write data to union
  moSbdMessage.windGust = windGust * 100;
  moSbdMessage.windGustDirection = windGustDirection;

  // Add to wind statistics 1
  windSpeedStats.add(windSpeed);
  vnStats.add(vn1);
  veStats.add(ve1);

  // Stop loop timer
  timer.anemometer = millis() - loopStartTime;
}

// Calculate resultant mean wind speed and direction vectors
// For more information see: http://www.webmet.com/met_monitoring/622.html
void windVectors()
{
  float rvWindDirection = atan2(veStats.average(), vnStats.average()); // Resultant mean wind direction
  rvWindDirection *= RAD_TO_DEG;  // Convert from radians to degrees

  if (rvWindDirection < 0)
  {
    rvWindDirection += 360;
  }

  float rvWindSpeed = sqrt(sq(veStats.average()) + sq(vnStats.average())); // Resultant mean wind speed

  if ((rvWindDirection == 0) && (rvWindSpeed != 0))
  {
    rvWindDirection = 360;
  }

  if (rvWindSpeed == 0)
  {
    rvWindDirection = 0;
  }

  // Wind direction "from" correction
  if (rvWindDirection < 180)
  {
    rvWindDirection += 180;
  }
  else if (rvWindDirection > 180)
  {
    rvWindDirection -= 180;
  }

  // Write data to union
  moSbdMessage.windSpeed = rvWindSpeed * 100;      // Resultant mean wind speed 1 (m/s)
  moSbdMessage.windDirection = rvWindDirection;    // Resultant mean wind direction 1 (°)
}
