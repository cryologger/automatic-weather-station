// Configure attached sensors
void configureSensors()
{
  // Enable power to sensor
  enableSensorPower();

  DEBUG_PRINT("Info: Initializing DPS310...");

  if (dps310.begin_I2C())
  {
    dps310.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps310.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
    online.dps310 = true;
    DEBUG_PRINTLN("success!");
  }
  else
  {
    DEBUG_PRINTLN("failed!");
    online.dps310 = false;
  }
}

// Read attached sensors
void readSensors()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor(s)
  configureSensors();

  // Check if sensor(s) online
  if (online.dps310)
  {
    DEBUG_PRINT("Info: Reading DPS310...");

    sensors_event_t temp_event, pressure_event;

    while ((!dps310.temperatureAvailable() || !dps310.pressureAvailable()) && millis() - loopStartTime < 5000UL)
    {
      return; // Wait until there's something to read
    }

    dps310.getEvents(&temp_event, &pressure_event);

    // Record measurements
    float temperature = temp_event.temperature;
    float pressure = pressure_event.pressure;

    //DEBUG_PRINT(temperature); DEBUG_PRINT(","); DEBUG_PRINTLN(pressure);

    // Write data to union
    moSbdMessage.intTemperature = temperature * 100;
    moSbdMessage.intPressure = (pressure - 850) * 100;
    moSbdMessage.intHumidity = humidity * 10;

    DEBUG_PRINTLN("done.");
  }
  else
  {
    DEBUG_PRINTLN("Warning: DPS310 offline!");
  }
  // Stop the loop timer
  timer.sensors = millis() - loopStartTime;

  // Disable power to sensor
  disableSensorPower();
}


// Configure the accelerometer
void configureAccel()
{
  // Enable power
  enableImuPower();

  DEBUG_PRINT("Info: Initializing LSM303...");

  // Initialize LSM303 accelerometer
  if (lsm303.begin())
  {
    online.lsm303 = true;
    DEBUG_PRINTLN("success!");
  }
  else
  {
    DEBUG_PRINTLN(F("Warning: Failed to initialize LSM303!"));
    online.lsm303 = false;
  }
}

// ------------------------------------------------------------------------------------------------
// LSM303AGR
// ------------------------------------------------------------------------------------------------
void readAccel()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Initialize IMU
  configureAccel();

  // Centered and scaled accelerometer/magnetomer data initialized to zero
  float Axyz[3] = {};

  // Check if IMU initialized successfully
  if (online.lsm303)
  {
    DEBUG_PRINT("Info: Reading accelerometer...");

    // Read normalized accelerometer data
    sensors_event_t accel;
    lsm303.getEvent(&accel);

    Axyz[0] += accel.acceleration.x;
    Axyz[1] += accel.acceleration.y;
    Axyz[2] += accel.acceleration.z;

    // Calculate pitch, roll and tilt-compensated heading
    float pitch = atan2(-Axyz[0], sqrt(Axyz[1] * Axyz[1] + Axyz[2] * Axyz[2])) * 180 / PI;
    float roll = atan2(Axyz[1], Axyz[2]) * 180 / PI;

    // Write orientation data to union
    moSbdMessage.pitch = pitch * 100;
    moSbdMessage.roll = roll * 100;

    DEBUG_PRINTLN("done.");

  }
  else
  {
    DEBUG_PRINTLN("Warning: LSM303 offline!");
  }

  // Disable power to IMU
  disableImuPower();

  // Stop loop timer
  timer.imu = millis() - loopStartTime;
}

// ------------------------------------------------------------------------------------------------
// Vaisala HMP60 Temperature/Relative Humidity
// ------------------------------------------------------------------------------------------------
void readTrh()
{
  // Enable power
  enableSensorPower();

  // Perform analog readings
  float extTemperature = analogRead(A3);
  float extHumidity = analogRead(A4);

  // Convert analog reading to voltage
  extTemperature *= (3.3 / 4096.0);
  extHumidity *= (3.3 / 4096.0);

  // Add to statistics object
  extTemperatureStats.add(extTemperature);
  extHumidityStats.add(extHumidity);

  // Disable power
  //disableSensorPower();
}



// ------------------------------------------------------------------------------------------------
// R.M. Young Wind Monitor 5103L (4-20 mA)
// ------------------------------------------------------------------------------------------------
void readAnemometer()
{
  unsigned int startTime = millis();

  // Enable power
  enableSensorPower();

  // Measure wind speed
  windDirection = analogRead(PIN_WIND_SPEED); // Raw analog wind speed value

  // Measure wind direction 
  windDirection = analogRead(PIN_WIND_DIR); // Raw analog wind direction value

  // Disable power
  disableSensorPower();

  // Correct for negative wind direction values
  if (windDirection > 360)
    windDirection -= 360;
  if (windDirection < 0)
    windDirection += 360;

  Serial.print(F("windSpeed: ")); Serial.println(windSpeed);
  Serial.print(F("windDirection: ")); Serial.println(windDirection);

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
