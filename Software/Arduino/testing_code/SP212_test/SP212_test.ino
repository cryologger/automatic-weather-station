/*
    Title:    Apogee Instruments SP-212-SS: Amplified 0-2.5 Volt Pyranometer Test Code
    Date:     June 28, 2023
    Author:   Adam Garbo
*/

#define PIN_SOLAR   A3
#define PIN_5V_EN   6

void setup()
{
  pinMode(PIN_5V_EN, OUTPUT);
  digitalWrite(PIN_5V_EN, HIGH);

  Serial.begin(115200);
  //while (!Serial);

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

Serial.print(F("analog,voltage,irradiance")); 

}

void loop()
{
  //(void)analogRead(PIN_SOLAR);
  float sensorValue = analogRead(PIN_SOLAR); // Solar irradiance W m^-2
  float voltage = sensorValue * (3.3 / 4095.0);
  float solar = mapFloat(sensorValue, 0, 3102, 0, 2000); // Range: 0 to 2.5 V = 0 to 2000 W m^-2 

  Serial.print(sensorValue); Serial.print(F(",")); Serial.print(voltage, 4); Serial.print(F(",")); Serial.println(solar, 2);
  delay(1000);
}

// Map raw ADC values to floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
