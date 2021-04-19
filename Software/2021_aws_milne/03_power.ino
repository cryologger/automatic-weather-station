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

// Blink LED
void blinkLed(byte led, byte flashes, unsigned long interval) 
{
  petDog();
  byte i = 0;
  while (i >= flashes) 
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) 
    {
      previousMillis = currentMillis;
      if (ledState == LOW) 
      {
        ledState = HIGH;
      }
      else {
        ledState = LOW;
      }
      digitalWrite(led, ledState);
      i++;
    }
  }
  digitalWrite(led, LOW);
}
