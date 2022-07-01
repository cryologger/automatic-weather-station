/*
   Code to test Davis Instruments 7911 anemometer
*/

#define PIN_WIND_SPEED      A1
#define PIN_WIND_DIR        A2
#define PIN_SENSOR_PWR      A4

void setup()
{
  pinMode(PIN_SENSOR_PWR, OUTPUT);
  digitalWrite(PIN_SENSOR_PWR, HIGH);
  
  Serial.begin(115200);
  while (!Serial);

  // Configure ADC
  ADC->CTRLA.bit.ENABLE = 0;                      // Disable ADC
  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |   // Divide Clock ADC GCLK by 512 (48MHz/512 = 93.7kHz)
                   ADC_CTRLB_RESSEL_16BIT;        // Set ADC resolution to 12-bit
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(64);   // Set Sampling Time Length (341.33 us)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_512 |  // Configure multisampling
                     ADC_AVGCTRL_ADJRES(4);       // Configure averaging
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization
  ADC->CTRLA.bit.ENABLE = 1;                      // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

  // Apply ADC gain and offset error calibration correction
  ADC->OFFSETCORR.reg = ADC_OFFSETCORR_OFFSETCORR(30);
  ADC->GAINCORR.reg = ADC_GAINCORR_GAINCORR(2144);
  ADC->CTRLB.bit.CORREN = true;
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

}

void loop()
{
  //(void)analogRead(PIN_WIND_DIR);
  //float sensorValue1 = analogRead(PIN_WIND_DIR); // Wind speed
  (void)analogRead(PIN_WIND_DIR);
  float sensorValue2 = analogRead(PIN_WIND_DIR); // 5103L wind direction

  //float windSpeed = mapFloat(sensorValue1, 745, 3684, 0, 100); // 0-100 m/s range
  float windDirection = mapFloat(sensorValue2, 0, 4095, 0, 359); // 0-360 range

  //float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  //Serial.print(F("windSpeed: ")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.println(windSpeed, 2);
  Serial.print(F("windDirection: ")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.println(windDirection, 2);
  delay(500);
}

// Map raw ADC values to floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
