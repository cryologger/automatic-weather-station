/*
  Title:   Vaisala HMP60 Test Code
  Date:     June 29, 2023
  Author:   Adam Garbo

  Sensor:
  - Vaisala HMP60 Temperature/Relative Humidity sensor
  https://www.vaisala.com/en/products/instruments-sensors-and-other-measurement-devices/instruments-industrial-measurements/hmp60

  Notes:
  - Sensors manually configured for 0 to 2.5V output

  Wiring Diagram:
  -----------------------------------------------------
  Colour     Pin     Description
  -----------------------------------------------------
  Brown      12V     Power (5 - 28V)
  White      A3      CH1: Relative humidity (0 - 2.5V)
  Blue       GND     Ground
  Black      A4      CH2: Temperature (0 - 2.5V)
  Shield     GND     Earth ground
*/

void setup()
{
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
  (void)analogRead(A3);
  float sensorValue2 = analogRead(A3); // HMP60 humidity (CH1)
  (void)analogRead(A4);
  float sensorValue1 = analogRead(A4); // HMP60 temperature (CH2)


  //float temperatureExt = mapFloat(sensorValue1, 0, 1240, -60, 40);  // Map temperature from 0-1 V to -60-40°C
  //float humidityExt = mapFloat(sensorValue2, 0, 1240, 0, 100);      // Map humidity  0-1 V to 0-100%

  float temperatureExt = mapFloat(sensorValue1, 0, 3102, -60, 40);  // Map temperature from 0-2.5 V to -60 to 40°C
  float humidityExt = mapFloat(sensorValue2, 0, 3102, 0, 100);      // Map humidity 0-2.5 V to 0 to 100%

  float voltage1 = sensorValue1 * (3.3 / 4095.0);
  float voltage2 = sensorValue2 * (3.3 / 4095.0);

  Serial.print(F("temperatureExt: ")); Serial.print(sensorValue1); Serial.print(F(",")); Serial.print(voltage1, 4); Serial.print(F(",")); Serial.println(temperatureExt, 4);
  Serial.print(F("humidityExt: ")); Serial.print(sensorValue2); Serial.print(F(",")); Serial.print(voltage2, 4); Serial.print(F(",")); Serial.println(humidityExt, 4);
  delay(500);
}

// Map raw ADC values to floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
