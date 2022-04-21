// Read temperature and humidity from Davis Instruments 6830
void readTrh() 
{
  // Enable power to temperature/relative humidity sensor
  //digitalWrite(PIN_TRH_PWR, HIGH); // Always on due to bug

  // Read sensor
  extTemperature = sht.readTemperatureC();
  humidity = sht.readHumidity();

  // Add to statistics object
  extTemperatureStats.add(extTemperature);
  humidityStats.add(humidity);

  Serial.print("Temperature: "); Serial.print(extTemperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");

  // Disable power to temperature/relative humidity sensor
  //digitalWrite(PIN_TRH_PWR, LOW);
}

// Measure wind speed and direction from Davis Instruments 7911 anemometer
void readAnemometer() 
{
  unsigned int startTime = millis();
  // Enable power to anemometer
  digitalWrite(PIN_WIND_PWR, HIGH);

  // Attach interrupts to wind speed input pins
  attachInterrupt(PIN_WIND_SPEED, windSpeedIsr, FALLING);
  revolutions = 0;

  // Measure wind speed for duration of samplePeriod
  while (millis() < startTime + (samplePeriod * 1000)) 
  {
    petDog(); // Reset Watchdog Timer
  }

  // Detach interrupts from wind speed input pins
  detachInterrupt(PIN_WIND_SPEED);

  // Calculate wind speed according to Davis Instruments formula: V = P(2.25/T)
  // V = speed in miles per hour
  // P = no. of pulses in sample period
  // T = duration of sample period in seconds
  windSpeed = revolutions * (2.25 / samplePeriod);    // Calculate wind speed 1 in miles per hour
  windSpeed *= 0.44704;                                // Convert wind speed 1 to metres per second

  // Measure wind direction
  for (byte i = 0; i < 5; i++) 
  {
    analogRead(PIN_WIND_DIRECTION);
    delay(1);
  }
  analogReadResolution(12);
  windDirection = analogRead(PIN_WIND_DIRECTION);        // Raw analog wind direction 1 value
  windDirection = map(windDirection, 0, 4095, 0, 359);    // Map wind direction 1 to degrees (0-360°)

  // Disable power to anemometer
  digitalWrite(PIN_WIND_PWR, LOW);

  // Correct for negative wind direction values
  if (windDirection > 360)
    windDirection -= 360;
  if (windDirection < 0)
    windDirection += 360;

  Serial.print(F("windSpeed: ")); Serial.println(windSpeed);
  Serial.print(F("windDirection: ")); Serial.println(windDirection);

  // Determine wind gust and direction 1
  if ((windSpeed > 0) && (windSpeed > windGust)) 
  {
    windGust = windSpeed;
    windGustDirection = windDirection;
  }

  // Calculate wind speed and direction 1 vectors
  float windDirectionRadians1 = windDirection * DEG_TO_RAD;    // Convert wind direction from degrees to radians
  float vn1 = -1.0 * windSpeed * cos(windDirectionRadians1);   // Magnitude of the north-south component (v) of the resultant vector mean wind
  float ve1 = -1.0 * windSpeed * sin(windDirectionRadians1);   // Magnitude of the east-west component (u) of the resultant vector mean wind

  // Write data to union
  message.windGust = windGust * 100;
  message.windGustDirection = windGustDirection;

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

  float rvWindSpeed = sqrt(sq(veStats.average()) + sq(vnStats.average())); // Resultant mean wind speed 1

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
  message.windSpeed = rvWindSpeed * 100;      // Resultant mean wind speed 1 (m/s)
  message.windDirection = rvWindDirection;    // Resultant mean wind direction 1 (°)
}

// Wind speed interrupt service routine (ISR)
void windSpeedIsr() 
{
  revolutions++;
}

// Calculate statistics and clear objects
void calculateStatistics() 
{
  // Write data to union
  message.voltage = batteryStats.minimum() * 1000;              // Minimum battery voltage (mV)
  message.extTemperature = extTemperatureStats.average() * 100; // Mean temperature (°C)
  message.humidity = humidityStats.average() * 100;             // Mean humidity (%)
  message.intTemperature = rtcStats.average() * 100;            // Mean RTC temperature (°C)

  // Clear statistics objects
  batteryStats.clear();
  humidityStats.clear();
  rtcStats.clear();
  extTemperatureStats.clear();
  windSpeedStats.clear();
  veStats.clear();
  vnStats.clear();
  windGust = 0.0;
  windGustDirection = 0.0;
}
