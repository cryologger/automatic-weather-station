/*
  Title:    R.M. Young Wind Monitor 5103L Test Code
  Date:     June 29, 2023
  Author:   Adam Garbo

  Sensor:
  - R.M. Young Wind Monitor 5103L 4-20 mA
  https://www.youngusa.com/product/wind-monitor/

  Notes:
  - 150 Ohm 0.1% resistor used to produce convert ideal
  voltage range: 0.6 - 3.0 V

  Wiring Diagram:
  --------------------------------------------------
  Colour     Pin       Description
  --------------------------------------------------
  Black      12V       Wind speed + (WS+)
  Red        A1        Wind speed - (WS-)
  White      12V       Wind direction + (WD+
  Green      A2        Wind direction - (WD-)
  Shield     GND       Earth ground
*/
#define PIN_WIND_SPEED  A1
#define PIN_WIND_DIR    A2
#define PIN_12V_EN      5   // 12 V step-up/down regulator
#define PIN_5V_EN       6   // 5V step-down regulator

void setup()
{
  pinMode(PIN_5V_EN, OUTPUT);
  pinMode(PIN_12V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, HIGH);   // Enable 5V power
  digitalWrite(PIN_12V_EN, HIGH);  // Enable 12V power

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
  ADC->OFFSETCORR.reg = ADC_OFFSETCORR_OFFSETCORR(0);
  ADC->GAINCORR.reg = ADC_GAINCORR_GAINCORR(2048);
  ADC->CTRLB.bit.CORREN = true;
  while (ADC->STATUS.bit.SYNCBUSY);               // Wait for synchronization

}

void loop()
{
  read5103L();
  delay(1000);
}

void read5103L()
{
  (void)analogRead(PIN_WIND_SPEED);
  float sensorValue1 = analogRead(PIN_WIND_SPEED); // Wind speed
  (void)analogRead(PIN_WIND_DIR);
  float sensorValue2 = analogRead(PIN_WIND_DIR); // Wind direction

  float windSpeed = mapFloat(sensorValue1, 745, 3691, 0, 100); // 0-100 m/s range
  float windDirection = mapFloat(sensorValue2, 745, 3691, 0, 360); // 0-360 range

  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  Serial.print(F("windSpeed: ")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.println(windSpeed, 2);
  Serial.print(F("windDirection: ")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.println(windDirection, 2);
}

// Map raw ADC values to floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
