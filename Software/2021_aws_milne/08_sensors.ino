
// Read temperature and humidity from Davis Instruments 6830
void readTrh() 
{
  // Enable power to temperature/relative humidity sensor
  //digitalWrite(GPIO_PWR_2_PIN, HIGH); // Always on due to bug

  // Read sensor
  extTemperature = sht.readTemperatureC();
  humidity = sht.readHumidity();

  // Add to statistics object
  extTemperatureStats.add(extTemperature);
  humidityStats.add(humidity);

  Serial.print("Temperature: "); Serial.print(extTemperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");

  // Disable power to temperature/relative humidity sensor
  //digitalWrite(GPIO_PWR_2_PIN, LOW);
}

// Measure wind speed and direction from Davis Instruments 7911 anemometer
void readAnemometer() 
{
  unsigned int startTime = millis();
  // Enable power to anemometer
  digitalWrite(GPIO_PWR_1_PIN, HIGH);

  // Attach interrupts to wind speed input pins
  attachInterrupt(WIND_SPEED_1_PIN, windSpeedIsr1, FALLING);
  revolutions1 = 0;

  // Measure wind speed for duration of samplePeriod
  while (millis() < startTime + (samplePeriod * 1000)) 
  {
    petDog(); // Reset Watchdog Timer
  }

  // Detach interrupts from wind speed input pins
  detachInterrupt(WIND_SPEED_1_PIN);

  // Calculate wind speed according to Davis Instruments formula: V = P(2.25/T)
  // V = speed in miles per hour
  // P = no. of pulses in sample period
  // T = duration of sample period in seconds
  windSpeed1 = revolutions1 * (2.25 / samplePeriod);    // Calculate wind speed 1 in miles per hour
  windSpeed1 *= 0.44704;                                // Convert wind speed 1 to metres per second

  // Measure wind direction
  for (byte i = 0; i < 5; i++) 
  {
    analogRead(WIND_DIRECTION_1_PIN);
    delay(1);
  }
  analogReadResolution(12);
  windDirection1 = analogRead(WIND_DIRECTION_1_PIN);        // Raw analog wind direction 1 value
  windDirection1 = map(windDirection1, 0, 4095, 0, 359);    // Map wind direction 1 to degrees (0-360°)

  // Disable power to anemometer
  digitalWrite(GPIO_PWR_1_PIN, LOW);

  // Correct for negative wind direction values
  if (windDirection1 > 360)
    windDirection1 -= 360;
  if (windDirection1 < 0)
    windDirection1 += 360;

  Serial.print(F("windSpeed1: ")); Serial.println(windSpeed1);
  Serial.print(F("windDirection1: ")); Serial.println(windDirection1);

  // Determine wind gust and direction 1
  if ((windSpeed1 > 0) && (windSpeed1 > windGust1)) 
  {
    windGust1 = windSpeed1;
    windGustDirection1 = windDirection1;
  }

  // Calculate wind speed and direction 1 vectors
  float windDirectionRadians1 = windDirection1 * DEG_TO_RAD;    // Convert wind direction from degrees to radians
  float vn1 = -1.0 * windSpeed1 * cos(windDirectionRadians1);   // Magnitude of the north-south component (v) of the resultant vector mean wind
  float ve1 = -1.0 * windSpeed1 * sin(windDirectionRadians1);   // Magnitude of the east-west component (u) of the resultant vector mean wind

  // Write data to union
  message.windGust1 = windGust1 * 100;
  message.windGustDirection1 = windGustDirection1;

  // Add to wind statistics 1
  windSpeedStats1.add(windSpeed1);
  vnStats1.add(vn1);
  veStats1.add(ve1);
}

// Calculate resultant mean wind speed and direction vectors
// For more information see: http://www.webmet.com/met_monitoring/622.html
void windVectors()
{
  float rvWindDirection1 = atan2(veStats1.average(), vnStats1.average()); // Resultant mean wind direction
  rvWindDirection1 *= RAD_TO_DEG;  // Convert from radians to degrees

  if (rvWindDirection1 < 0) 
  {
    rvWindDirection1 += 360;
  }

  float rvWindSpeed1 = sqrt(sq(veStats1.average()) + sq(vnStats1.average())); // Resultant mean wind speed 1

  if ((rvWindDirection1 == 0) && (rvWindSpeed1 != 0)) 
  {
    rvWindDirection1 = 360;
  }

  if (rvWindSpeed1 == 0) 
  {
    rvWindDirection1 = 0;
  }

  // Wind direction "from" correction
  if (rvWindDirection1 < 180) 
  {
    rvWindDirection1 += 180;
  }
  else if (rvWindDirection1 > 180) 
  {
    rvWindDirection1 -= 180;
  }

  // Write data to union
  message.windSpeed1 = rvWindSpeed1 * 100;      // Resultant mean wind speed 1 (m/s)
  message.windDirection1 = rvWindDirection1;    // Resultant mean wind direction 1 (°)
}

// Wind speed 1 interrupt service routine (ISR)
void windSpeedIsr1() 
{
  revolutions1++;
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
  windSpeedStats1.clear();
  veStats1.clear();
  vnStats1.clear();
  windGust1 = 0.0;
  windGustDirection1 = 0.0;
}
