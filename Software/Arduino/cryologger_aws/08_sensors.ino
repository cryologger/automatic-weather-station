// ----------------------------------------------------------------------------
// Adafruit BME280 Temperature Humidity Pressure Sensor
// https://www.adafruit.com/product/2652
// ----------------------------------------------------------------------------
void configureBme280()
{
  DEBUG_PRINT("Info: Initializing BME280...");

  if (bme280.begin())
  {
    online.bme280 = true;
    DEBUG_PRINTLN("success!");
  }
  else
  {
    online.bme280 = false;
    DEBUG_PRINTLN("failed!");
  }
}

// Read BME280
void readBme280()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  // Initialize sensor
  configureBme280();

  // Check if sensor initialized successfully
  if (online.bme280)
  {
    DEBUG_PRINT("Info: Reading BME280...");

    myDelay(250);

    // Read sensor data
    temperatureInt = bme280.readTemperature();
    humidityInt = bme280.readHumidity();
    pressureInt = bme280.readPressure() / 100.0F;

    // Add to statistics object
    temperatureIntStats.add(temperatureInt);
    humidityIntStats.add(humidityInt);
    pressureIntStats.add(pressureInt);

    DEBUG_PRINTLN("done.");
  }
  else
  {
    DEBUG_PRINTLN("Warning: BME280 offline!");
  }
  // Stop the loop timer
  timer.readBme280 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Adafruit DPS310 Precision Barometric Pressure Sensor
// https://www.adafruit.com/product/4494
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
  timer.readDps310 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Davis Instruments Temperature Humidity Sensor
// Sensiron SHT31-LSS
// Yellow:  Power
// Green:   Ground
// White:   SCK
// Blue:    SDA
// ----------------------------------------------------------------------------
void readSht31()
{
  // Start the loop timer
  unsigned long loopStartTime = millis();

  DEBUG_PRINT("Info: Reading SHT31...");

  // Disable I2C bus
  Wire.end();

  // Add delay
  myDelay(100);

  // Read sensor
  temperatureExt = sht.readTemperatureC();
  humidityExt = sht.readHumidity();

  // Add to statistics object
  temperatureExtStats.add(temperatureExt);
  humidityExtStats.add(humidityExt);

  DEBUG_PRINT("Temperature: "); DEBUG_PRINT(temperatureExt); DEBUG_PRINTLN(" C");
  DEBUG_PRINT("Humidity: "); DEBUG_PRINT(humidityExt); DEBUG_PRINTLN("%");

  // Re-enable I2C bus
  Wire.begin();

  // Stop the loop timer
  timer.readSht31 = millis() - loopStartTime;
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
    //pitchStats.add();
    //rollStats.add();

    DEBUG_PRINTLN("done.");

    // Print debug info
    //DEBUG_PRINT(F("pitch: ")); DEBUG_PRINT_DEC(pitch, 2);
    //DEBUG_PRINT(F(" roll: ")); DEBUG_PRINTLN_DEC(roll, 2);

  }
  else
  {
    DEBUG_PRINTLN("Warning: LSM303 offline!");
  }

  // Stop loop timer
  timer.readLsm303 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Vaisala HMP60 Temperature/Relative Humidity
// Brown:  5-28 VDC
// White:  Channel 1 RH 0-1V
// Blue:   GND
// Black:  Channel 2 T 0-1V
// ----------------------------------------------------------------------------
void readHmp60()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  DEBUG_PRINT("Info: Reading HMP60...");

  // Note: A startup delay of 4 s is recommended at 13.5 V and 2 s at 5 V
  myDelay(4000);

  // Perform analog readings
  (void)analogRead(PIN_TEMP);
  float sensorValue1 = analogRead(PIN_TEMP); // External temperature
  (void)analogRead(PIN_HUMID);
  float sensorValue2 = analogRead(PIN_HUMID); // External humidity

  DEBUG_PRINTLN("done.");

  // Map voltages to sensor ranges
  temperatureExt = mapFloat(sensorValue1, 0, 1240, -40, 60); // Map temperature from 0-1.0 V to -40 to 60°C
  humidityExt = mapFloat(sensorValue2, 0, 1240, 0, 100);      // Map humidity 0-1 V to 0-100%
  //float temperatureExt = mapFloat(sensorValue1, 0, 3102, -60, 40); // Map temperature from 0-2.5 V to -60 to 40°C
  //float humidityExt = mapFloat(sensorValue2, 0, 3102, 0, 100);      // Map humidity 0-2.5 V to 0 to 100%

  // Calculate measured voltages
  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  // Print debug info

#if CALIBRATE
  DEBUG_PRINT(F("temperatureExt: ")); DEBUG_PRINT_DEC(voltage1, 4); DEBUG_PRINT(F(",")); DEBUG_PRINT(sensorValue1); DEBUG_PRINT(F(",")); DEBUG_PRINTLN_DEC(temperatureExt, 2);
  DEBUG_PRINT(F("humidityExt: ")); DEBUG_PRINT_DEC(voltage2, 4); DEBUG_PRINT(F(",")); DEBUG_PRINT(sensorValue2); DEBUG_PRINT(F(",")); DEBUG_PRINTLN_DEC(humidityExt, 2);
#endif

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
  timer.readHmp60 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Apogee SP-212 Pyranometer
// White: Positive (signal from sensor)
// Red:   Input Power  5-24 V DC
// Black: Ground (from sensor signal and output power)
// Clear: Shield/Ground
// ----------------------------------------------------------------------------
void readSp212()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  DEBUG_PRINT("Info: Reading SP212...");

  // Perform analog readings
  (void)analogRead(PIN_SOLAR);
  float sensorValue = analogRead(PIN_SOLAR); // External temperature

  DEBUG_PRINTLN("done.");

  // Map voltages to sensor ranges
  solar = mapFloat(sensorValue, 0, 3102, 0, 2000); // Map solar from 0-2.5 V to 0 to 2000

  // Calculate measured voltages
  float voltage = sensorValue * (3.3 / 4095.0);

  // Print debug info
  //DEBUG_PRINT(F("solar: ")); DEBUG_PRINT_DEC(voltage, 4); DEBUG_PRINT(F(",")); DEBUG_PRINT(sensorValue); DEBUG_PRINT(F(",")); DEBUG_PRINTLN_DEC(solar, 2);

  // Add to statistics object
  solarStats.add(solar);

  // Stop loop timer
  timer.readSp212 = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// R.M. Young Wind Monitor 5103L (4-20 mA)
// 150 Ohm 0.1% resistor
// Voltage range: 0.5995 - 2.9675 V
// Pin  Colour  Pin
// -----------------
// WS+  Black   12 V
// WS-  Red     A1
// WD+  White   12 V
// WD-  Green   A2
// ----------------------------------------------------------------------------
void read5103L()
{
  unsigned int loopStartTime = millis();

  DEBUG_PRINT("Info: Reading anemometer...");

  // Measure wind speed and direction
  (void)analogRead(PIN_WIND_SPEED);
  float sensorValue1 = analogRead(PIN_WIND_SPEED); // Raw analog wind speed value
  (void)analogRead(PIN_WIND_DIR);
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

#if CALIBRATE
  // Print debug info
  DEBUG_PRINT(F("windSpeed: ")); DEBUG_PRINT_DEC(voltage1, 4); DEBUG_PRINT(F(",")); DEBUG_PRINT(sensorValue1); DEBUG_PRINT(F(",")); DEBUG_PRINTLN_DEC(windSpeed, 2);
  DEBUG_PRINT(F("windDirection: ")); DEBUG_PRINT_DEC(voltage2, 4); DEBUG_PRINT(F(",")); DEBUG_PRINT(sensorValue2); DEBUG_PRINT(F(",")); DEBUG_PRINTLN_DEC(windDirection, 2);
#endif
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
  timer.read5103L = millis() - loopStartTime;
}

// ----------------------------------------------------------------------------
// Davis Instruments 7911 Anemometer
// ------------------------------
// Colour    Description     Pin
// ------------------------------
// Black     Wind speed      A1
// Green     Wind direction  A2
// Yellow    Power           5V
// Red       Ground          GND
// ----------------------------------------------------------------------------
void read7911()
{
  uint32_t loopStartTime = millis();

  DEBUG_PRINTLN("Info: Reading 7911...");

  // Enable pull-ups
  pinMode(PIN_WIND_SPEED, INPUT_PULLUP);

  // Attach interrupt to wind speed input pin
  attachInterrupt(PIN_WIND_SPEED, windSpeedIsr, FALLING);
  revolutions = 0;

  // Measure wind speed for 5 seconds
  while (millis() < loopStartTime + 3000);
  {
    // Do nothing
  }

  // Detach interrupt from wind speed input pin
  detachInterrupt(PIN_WIND_SPEED);

  // Disable pull-ups
  pinMode(PIN_WIND_SPEED, INPUT);

  // Calculate wind speed according to Davis Instruments formula: V = P(2.25/T)
  // V = speed in miles per hour
  // P = no. of pulses in sample period
  // T = duration of sample period in seconds
  windSpeed = revolutions * (2.25 / 3);   // Calculate wind speed in miles per hour
  windSpeed *= 0.44704;                   // Convert wind speed to metres per second

  // Enable power
  digitalWrite(PIN_SENSOR_PWR, HIGH);

  // Measure wind direction
  (void)analogRead(PIN_WIND_DIR);
  windDirection = analogRead(PIN_WIND_DIR); // Raw analog wind direction value
  windDirection = map(windDirection, 0, 4095, 0, 359); // Map wind direction to degrees (0-360°)

  // Disable power
  digitalWrite(PIN_SENSOR_PWR, LOW);

  // Correct for negative wind direction values
  if (windDirection > 360)
    windDirection -= 360;
  if (windDirection < 0)
    windDirection += 360;

  if (windSpeed == 0)
  {
    windDirection = 0.0;
  }

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

  // Print debug info
  //DEBUG_PRINT(F("Wind Speed: ")); DEBUG_PRINTLN(windSpeed);
  //DEBUG_PRINT(F("Wind Direction: ")); DEBUG_PRINTLN(windDirection);

  // Stop loop timer
  timer.read7911 = millis() - loopStartTime;
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

  DEBUG_PRINT("uStats.average(): "); DEBUG_PRINTLN(uStats.average());
  DEBUG_PRINT("vStats.average(): "); DEBUG_PRINTLN(vStats.average());

  // Calculate resultant mean wind direction
  float rvWindDirection = atan2(-1.0 * uStats.average(), -1.0 * vStats.average());
  DEBUG_PRINT("rvWindDirection: "); DEBUG_PRINTLN(rvWindDirection);
  rvWindDirection *= RAD_TO_DEG;  // Convert from radians to degrees
  DEBUG_PRINT("rvWindDirection: "); DEBUG_PRINTLN(rvWindDirection);

  // To do: Check if necessary
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

// ----------------------------------------------------------------------------
// MaxBotix MB7354 HRXL-MaxSonar-WRS5
// https://www.maxbotix.com/ultrasonic_sensors/mb7354.htm
// --------------------------------------------------
// Colour    Pin    Description             Pin
// --------------------------------------------------
// White    1       Temperature Sensor      Not connected
// Orange   2       Pulse Width Output      Not connected
// Brown    3       Analog Voltage Output   Analog In
// Green    4       Ranging Start/Stop      Not connected
// Blue     5       Serial Output           Not connected
// Red      6       Vcc                     5V
// Black    7       GND                     GND
//
// ----------------------------------------------------------------------------
// Read Maxbotix distance to surface
void readMb7354()
{

}
