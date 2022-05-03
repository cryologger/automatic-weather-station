// Read battery voltage from voltage divider
void readBattery()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  myDelay(1000);

  // Measure external battery voltage across 10/1 MΩ resistor divider (1/10 divider)
  voltage = analogRead(PIN_VBAT);
  voltage  *=  ((10000000.0 + 1000000.0) / 1000000.0); // Multiply back 1 MOhm / (10 MOhm + 1 MOhm)
  voltage *= 3.3;   // Multiply by 3.3V reference voltage
  voltage /= 4096;  // Convert to voltage

  // Measure LiPo battery voltage across 100 kΩ/100 kΩ onboard resistor divider (1/2 divider)
  //float voltage = analogRead(A7);
  //voltage = voltage / samples * 3.3 * 2 / 4096.0;
  
  // Write data to union
  moSbdMessage.voltage = voltage * 100;

  // Add to statistics object
  batteryStats.add(voltage);

  // Stop loop timer
  timer.battery = millis() - loopStartTime;
}

// Disable serial port
void disableSerial()
{
#if DEBUG
  SERIAL_PORT.end(); // Close serial port
  USBDevice.detach(); // Safely detach USB prior to sleeping
#endif
}

// Enable serial port
void enableSerial()
{
#if DEBUG
  USBDevice.attach(); // Re-attach USB
  SERIAL_PORT.begin(115200);
  //myDelay(3000); // Non-blocking delay to allow user to open Serial Monitor
#endif
}

// Enable power to IMU
void enable5V()
{
  digitalWrite(PIN_5V_EN, HIGH);
  myDelay(500);
}

// Disable power to IMU
void disable5V()
{
  digitalWrite(PIN_5V_EN, LOW);
}

// Enable power to sensors
void enable12V()
{
  digitalWrite(PIN_12V_EN, HIGH);
  myDelay(500);
}

// Disable power to sensors
void disable12V()
{
  digitalWrite(PIN_12V_EN, LOW);
}

// Enable power to GNSS
void enableGnssPower()
{
  digitalWrite(PIN_GNSS_EN, LOW);
}

// Disable power to GNSS
void disableGnssPower()
{
  digitalWrite(PIN_GNSS_EN, HIGH);
}

// Enable power to RockBLOCK 9603
void enableIridiumPower()
{
  digitalWrite(PIN_5V_EN, HIGH);
}

// Disable power to RockBLOCK 9603
void disableIridiumPower()
{
  digitalWrite(PIN_5V_EN, LOW);
}

//
void prepareForSleep()
{
  // Disable serial
  disableSerial();

  // Clear online union
  memset(&online, 0, sizeof(online));

  // Clear timer union
  memset(&timer, 0, sizeof(timer));
}

// Enter deep sleep
void goToSleep()
{
  // Clear first-time flag after initial power-down
  if (firstTimeFlag)
  {
    firstTimeFlag = false;
  }

  // Enter deep sleep
  LowPower.deepSleep();

  // Sleep until next alarm match
  //rtc.standbyMode();

  /* Code sleeps here and awaits RTC or WDT interrupt */
}

// Wake from deep sleep
void wakeUp()
{
  // Enable serial port
  enableSerial();
  blinkLed(4, 250);
}

// Non-blocking blink LED (https://forum.arduino.cc/index.php?topic=503368.0)
void blinkLed(byte ledFlashes, unsigned int ledDelay)
{
  byte i = 0;
  while (i < ledFlashes * 2)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= ledDelay)
    {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      previousMillis = currentMillis;
      i++;
    }
  }
  // Ensure LED is off at end of blink cycle
  digitalWrite(LED_BUILTIN, LOW);
}

// Non-blocking delay (milliseconds)
// https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void myDelay(unsigned long ms)
{
  unsigned long start = millis(); // Start: timestamp
  for (;;)
  {
    petDog();                            // Reset watchdog timer
    unsigned long now = millis();        // Now: timestamp
    unsigned long elapsed = now - start; // Elapsed: duration
    if (elapsed >= ms)                   // Comparing durations: OK
      return;
  }
}
