// Measure battery voltage from 330kOhm/1MOhm divider
void readBattery()
{
  unsigned int loopStartTime = millis();

  voltage = 0.0;
  analogReadResolution(12);
  for (byte i = 0; i < samplesToAverage; i++)
  {
    voltage += analogRead(VBAT_PIN);
    delay(10);
  }
  voltage /= samplesToAverage;

  voltage *= ((330000.0 + 100000.0) / 100000.0); // Multiply back 100,000 / (330,000 + 100,000) kOhm
  voltage *= 2;     // Multiply back
  voltage *= 3.3;   // Multiply by 3.3V reference voltage
  voltage /= 4096;  // Convert to voltage

  // Add to statistics object
  batteryStats.add(voltage);

  Serial.print("voltage: "); Serial.println(voltage);

  unsigned int batteryLoopTime = millis() - loopStartTime;
  //Serial.print("readBattery() function execution: "); Serial.print(batteryLoopTime); Serial.println(F(" ms"));
}

// Non-blocking blink LED (https://forum.arduino.cc/index.php?topic=503368.0)
void blinkLed(byte led, byte ledFlashes, unsigned int ledDelay)
{
  byte i = 0;
  while (i < ledFlashes * 2)
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= ledDelay)
    {
      digitalWrite(led, !digitalRead(led));
      previousMillis = currentMillis;
      i++;
    }
  }
  // Turn off LED
  digitalWrite(led, LOW);
}

// Non-blocking delay (ms: duration)
// https://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void myDelay(unsigned long ms)
{
  unsigned long start = millis();         // Start: timestamp
  for (;;)
  {
    petDog();                             // Reset watchdog timer
    unsigned long now = millis();         // Now: timestamp
    unsigned long elapsed = now - start;  // Elapsed: duration
    if (elapsed >= ms)                    // Comparing durations: OK
      return;
  }
}
