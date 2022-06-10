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
// Davis Instruments Temperature Humidity Sensor
// Sensiron SHT31-LSS
// ----------------------------------------------------------------------------
void readSht31()
{
  // Disable I2C bus
  Wire.end();

  // Add delay
  myDelay(100);

  // Read sensor
  float temperature = sht.readTemperatureC();
  float humidity = sht.readHumidity();

  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");

  // Re-enable I2C bus
  Wire.begin();
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
  float sensorValue1 = analogRead(PIN_TEMP); // External temperature
  float sensorValue2 = analogRead(PIN_HUMID); // External humidity

  DEBUG_PRINTLN("done.");

  // Map voltages to sensor ranges
  float temperatureExt = mapFloat(sensorValue1, 0, 3102, -60, 40); // Map temperature from 0-2.5 V to -60 to 40°C
  float humidityExt = mapFloat(sensorValue2, 0, 3102, 0, 100);      // Map humidity 0-2.5 V to 0 to 100%

  // Calculate measured voltages
  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  Serial.print(F("temperatureExt: ")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.println(temperatureExt, 2);
  Serial.print(F("humidityExt: ")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.println(humidityExt, 2);

  // Check and limit values if maximums are exceeded
  if (temperatureExt < -60)
    temperatureExt = -60;
  if (temperatureExt > 40)
    temperatureExt = 40;
  if (humidityExt < 0)
    humidityExt = 0;
  if (humidityExt > 100)
    humidityExt = 100;

  // Add to statistics object
  temperatureExtStats.add(temperatureExt);
  humidityExtStats.add(humidityExt);

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

  //for (int i = 0; i < 20; i++)
  //{
  // Measure wind speed and direction
  float sensorValue1 = analogRead(PIN_WIND_SPEED); // Raw analog wind speed value
  float sensorValue2 = analogRead(PIN_WIND_DIR); // Raw analog wind direction value

  // Map wind speed and direction
  windSpeed = mapFloat(sensorValue1, 745, 3684, 0, 100); // 0-100 m/s range
  windDirection = mapFloat(sensorValue2, 745, 3684, 0, 360); // 0-360 range

  // Calculate measured voltages
  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  // Check if readings are erroneous
  if (windSpeed < 0)
    windSpeed = 0;
  if (windDirection < 0)
    windDirection = 0;
  if (windDirection > 360)
    windDirection = 360;

  DEBUG_PRINTLN("done.");

  Serial.print(F("windSpeed: ")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.println(windSpeed, 2);
  Serial.print(F("windDirection: ")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.println(windDirection, 2);
  //myDelay(500);
  //}

  // Check and update wind gust and direction
  if ((windSpeed > 0) && (windSpeed > windGustSpeed))
  {
    windGustSpeed = windSpeed;
    windGustDirection = windDirection;
  }

  // Calculate wind speed and direction vectors
  // http://tornado.sfsu.edu/geosciences/classes/m430/Wind/WindDirection.html
  float windDirectionRadians = windDirection * DEG_TO_RAD;  // Convert wind direction from degrees to radians
  float u = -1.0 * windSpeed * sin(windDirectionRadians);   // Magnitude of east-west component (u) of vector winds
  float v = -1.0 * windSpeed * cos(windDirectionRadians);   // Magnitude of north-south component (v) of vector winds

  // Write data to union
  moSbdMessage.windGustSpeed = windGustSpeed * 100;
  moSbdMessage.windGustDirection = windGustDirection * 10;

  // Add to wind statistics
  windSpeedStats.add(windSpeed);
  uStats.add(u);
  vStats.add(v);

  // Stop loop timer
  timer.anemometer = millis() - loopStartTime;
}


// ----------------------------------------------------------------------------
// Measure wind speed and direction from Davis Instruments 7911 anemometer
// Davis Instruments 7911 Anemometer
// ------------------------------
// Colour    Description     Pin
// ------------------------------
// Black     Wind speed      A0
// Red       Ground          GND
// Green     Wind direction  A1
// Yellow    Power           A5
// ----------------------------------------------------------------------------
void readDavis7911()
{
  uint32_t loopStartTime = millis();

  // Enable power
  //digitalWrite(PIN_SENSOR_PWR, HIGH);

  // Enable pull-ups
  pinMode(PIN_WIND_SPEED, INPUT_PULLUP);

  // Attach interrupt to wind speed input pin
  attachInterrupt(PIN_WIND_SPEED, windSpeedIsr, FALLING);
  revolutions = 0;

  // Measure wind speed for 5 seconds
  while (millis() < loopStartTime + (samplePeriod * 1000));
  {
    // Do nothing
  }

  // Detach interrupt from wind speed input pin
  detachInterrupt(PIN_WIND_SPEED);

  // Disable pull-ups
  pinMode(PIN_WIND_SPEED, INPUT);

  // Disable power
  //digitalWrite(PIN_SENSOR_PWR, LOW);

  // Calculate wind speed according to Davis Instruments formula: V = P(2.25/T)
  // V = speed in miles per hour
  // P = no. of pulses in sample period
  // T = duration of sample period in seconds
  windSpeed = revolutions * (2.25 / samplePeriod);  // Calculate wind speed in miles per hour
  windSpeed *= 0.44704;                             // Convert wind speed to metres per second

  // Measure wind direction
  (void)analogRead(PIN_WIND_DIRECTION);
  windDirection = analogRead(PIN_WIND_DIRECTION); // Raw analog wind direction value
  windDirection = map(windDirection, 0, 4095, 0, 359); // Map wind direction to degrees (0-360°)

  // Correct for negative wind direction values
  if (windDirection > 360)
    windDirection -= 360;
  if (windDirection < 0)
    windDirection += 360;

  DEBUG_PRINT(F("Wind Speed: ")); DEBUG_PRINTLN(windSpeed);
  DEBUG_PRINT(F("Wind Direction: ")); DEBUG_PRINTLN(windDirection);
}

// Wind speed interrupt service routine (ISR)
// for Davis Instruments 7911 anemometer
void windSpeedIsr()
{
  revolutions++;
}

// Calculate mean wind speed and direction from vector components
// For more information see:
// http://tornado.sfsu.edu/geosciences/classes/m430/Wind/WindDirection.html
void windVectors()
{
  // Calculate resultant mean wind speed
  float rvWindSpeed = sqrt(sq(uStats.average()) + sq(vStats.average()));

  Serial.print("uStats.average(): "); Serial.println(uStats.average());
  Serial.print("vStats.average(): "); Serial.println(vStats.average());

  // Calculate resultant mean wind direction
  float rvWindDirection = atan2(-1.0 * uStats.average(), -1.0 * vStats.average());
  Serial.print("rvWindDirection: "); Serial.println(rvWindDirection);
  rvWindDirection *= RAD_TO_DEG;  // Convert from radians to degrees
  Serial.print("rvWindDirection: "); Serial.println(rvWindDirection);

  // Why?
  if (rvWindDirection < 0)
    rvWindDirection += 360;

  // Zero wind direction if wind speed is zero.
  // Note: atan2 can be undefined if u and v vectors are zero
  if (rvWindSpeed == 0)
    rvWindDirection = 0;

  // Write data to union
  moSbdMessage.windSpeed = rvWindSpeed * 100;         // Resultant mean wind speed (m/s)
  moSbdMessage.windDirection = rvWindDirection * 10;  // Resultant mean wind direction (°)
}
