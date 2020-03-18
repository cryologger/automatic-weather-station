/*
  Title:    Cryologger Automatic Weather Station (AWS)
  Date:     June 21, 2019
  Author:   Adam Garbo
            
  Components:
  - Adafruit Feather M0 Adalogger
  - Adafruit DS3231 RTC Precision Featherwing
  - Davis Instruments 7911 Anemomenter
  - Davis Instruments 6830 Temperature/Relative Humidity
  - Rock Seven RockBLOCK 9603
  - Pololu 3.3V, 600mA Step-Down Voltage Regulator D36V6F3
  - 256 MB microSD card (Industrial rated)
  
  Comments:
  - Automatic weather station deployed on the Milne Glacier, Ellesmere Island, 
  Nunavut in the summer of 2019.
*/

// Libraries
#include <Arduino.h>            // https://github.com/arduino/ArduinoCore-samd (Required before wiring_private.h)
#include <ArduinoLowPower.h>    // https://github.com/arduino-libraries/ArduinoLowPower
#include <DS3232RTC.h>          // https://github.com/JChristensen/DS3232RTC
#include <IridiumSBD.h>         // https://github.com/mikalhart/IridiumSBD
#include <math.h>               // https://www.nongnu.org/avr-libc/user-manual/group__avr__math.html
#include <sensirion.h>          // https://github.com/HydroSense/sensirion
#include <Statistic.h>          // https://github.com/RobTillaart/Arduino/tree/master/libraries/Statistic
#include <SdFat.h>              // https://github.com/greiman/SdFat
#include <SPI.h>                // https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>               // https://www.arduino.cc/en/Reference/Wire
#include <wiring_private.h>     // pinPeripheral() function

// Defined constants
#define RTC_INT_PIN             5
#define ROCKBLOCK_EN_PIN        6
#define LED_PIN                 8
#define ROCKBLOCK_RX_PIN        10
#define ROCKBLOCK_TX_PIN        11
#define ROCKBLOCK_SLEEP_PIN     12
#define WIND_SPEED_1_PIN        A0
#define WIND_DIRECTION_1_PIN    A1
#define GPIO_PWR_1_PIN          A2    // Anemometer GPIO power pin
#define VBAT_PIN                A7

#define DEBUG true

// Create a new Serial instance. For more information see: https://www.arduino.cc/en/Tutorial/SamdSercom
Uart Serial2 (&sercom1, ROCKBLOCK_RX_PIN, ROCKBLOCK_TX_PIN, SERCOM_RX_PAD_2, UART_TX_PAD_0);  // Create the new UART instance assigning it to pin 10 and 11
#define IridiumSerial Serial2

// Attach the interrupt handler to the SERCOM
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

// Object instantiations
DS3232RTC       myRTC(false);             // Tell constructor not to initialize the I2C bus
IridiumSBD      modem(IridiumSerial, ROCKBLOCK_SLEEP_PIN);
SdFat           sd;                       // File system object
SdFile          file;                     // Log file
sensirion       sht(20, 21);              // sht(data pin, clock pin);
time_t          t, unixtime, alarmTime;
tmElements_t    tm;

// Statistic objects
Statistic batteryStats;           // Battery voltage statistics
Statistic humidityStats;          // Humidity statistics
Statistic rtcStats;
Statistic temperatureStats;       // Temperature statistics
Statistic windSpeedStats1;        // Anemometer wind speed statistics
Statistic vnStats1;               // Anemometer north-south wind vector component (v) statistics
Statistic veStats1;               // Anemometer east-west wind vector component (u) statistics

// User defined global variable declarations
uint32_t            sampleInterval          = 300;            // Sleep duration (in seconds) between data sample acquisitions. Default = 5 minutes (300 seconds)
uint16_t            averageInterval         = 12;             // Number of samples to be averaged for each RockBLOCK transmission. Default = 12 (Hourly)
uint16_t            transmitInterval        = 1;              // Number of message to be included in a single transmission (340 byte limit). Default = 3 (Every 3 hours)
uint16_t            maxRetransmitCounter    = 10;             // Maximum number of failed data transmissions to reattempt in a single message (340 byte limit). Default: 10
uint16_t            samplesPerFile          = 8640;           // Maximum number of samples stored in a file before new log file creation (Default: 30 days * 288 samples per day)

// Global variable declarations
const uint8_t       chipSelect              = 4;              // MicroSD chip select pin
const uint8_t       samplesToAverage        = 10;             // Number of samples to average
volatile bool       alarmIsrWasCalled       = false;          // RTC interrupt service routine (ISR) flag
volatile bool       sleeping                = false;          // Watchdog Timer Early Warning interrupt flag
volatile uint16_t   revolutions1            = 0;              // Wind speed 1 ISR revolutions counter
volatile uint32_t   contactBounceTime       = 0;              //
bool                logging                 = true;           // MicroSD initilization flag
bool                messageSent             = true;           // RockBLOCK transmission flag
uint8_t             forceReset              = 0;              // Watchdog Timer force reset flag
uint8_t             transmitBuffer[340]     = {};             // RockBLOCK transmission buffer
char                fileName[12]            = "log000.csv";   // Log file naming convention
float               humidity                = 0.0;            // SHT31 humidity (%)
float               temperature             = 0.0;            // SHT31 temperature (°C)
float               temperatureDs3231       = 0.0;            // Internal RTC temperature (°C)
float               voltage                 = 0.0;            // Battery voltage in volts (V)
float               windSpeed1              = 0.0;            // Wind speed 1 in metres per second (m/s)
float               windGust1               = 0.0;            // Wind gust speed 1 in metres per second (m/s)
float               windGustDirection1      = 0.0;            // Wind gust direction 1 in degrees (°)
uint16_t            windDirection1          = 0;              // Wind direction 1 in degrees (°)
uint16_t            samplePeriod            = 3;              // Wind speed sample period in seconds (s)
uint16_t            retransmitCounter       = 0;              // RockBLOCK failed data transmission counter
uint16_t            messageCounter          = 0;              // RockBLOCK transmission counter
uint16_t            transmitCounter         = 0;              // RockBLOCK transmission interval counter
uint32_t            previousMillis          = 0;              // RockBLOCK callback function timer variable
uint32_t            transmitDuration        = 0;              // RockBLOCK data transmission time variable
uint16_t            sampleCounter           = 0;              // Sensor measurement counter
uint16_t            samplesSaved            = 0;              // Log file sample counter

extern "C" char *sbrk(int i);                                 // Free RAM function

// Structure and union to store and send data byte-by-byte via RockBLOCK
typedef union
{
  struct
  {
    uint32_t    unixtime;             // Date and time in time_t format           (4 bytes)
    uint16_t    temperatureDs3231;    // Mean temperature (°C)                    (2 bytes) ***BUG: CHANGE TO int16_t***
    int16_t     temperature;          // Mean temperature (°C)                    (2 bytes) (temperature * 100)
    uint16_t    humidity;             // Mean humidity (%)                        (2 bytes) (humidity * 100)
    uint16_t    windSpeed1;           // Resultant mean wind speed 1 (m/s)        (2 bytes) (windSpeed * 100)
    uint16_t    windDirection1;       // Resultant mean wind direction 1 (°)      (2 bytes)
    uint16_t    windGust1;            // Wind gust speed 1 (m/s)                  (2 bytes) (windGust * 100)
    uint16_t    windGustDirection1;   // Wind gust direction 1 (°)                (2 bytes)
    uint16_t    voltage;              // Minimum battery voltage (mV)             (2 bytes) (voltage * 1000)
    uint16_t    transmitDuration;     // Debugging variable                       (2 bytes)
    uint16_t    messageCounter;       // RockBLOCK data transmission counter      (2 bytes)

  } __attribute__((packed));                                                      // Total = 24 bytes
  uint8_t bytes[];
} SBDMESSAGE;

SBDMESSAGE message;
size_t messageSize = sizeof(message);     // Size (in bytes) of data to be stored and transmitted

void setup()
{
  // Pin assignments
  pinMode(GPIO_PWR_1_PIN, OUTPUT);    // Anemometer GPIO power pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ROCKBLOCK_EN_PIN, OUTPUT);
  pinMode(WIND_SPEED_1_PIN, INPUT);
  digitalWrite(GPIO_PWR_1_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(ROCKBLOCK_EN_PIN, LOW);

  Serial.begin(115200);
  //while (!Serial);      // Wait for USB Serial (for use when monitoring Serial)
  delay(10000);           // Wait for USB Serial (for use when not monitoring Serial)

  // Configure Watchdog Timer
  configureWatchdog();

  // Define device power profile assuming battery power
  modem.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);
  modem.adjustSendReceiveTimeout(120); // ***CHANGE TO 180***
  modem.adjustATTimeout(20);

  // Initialize DS3231
  myRTC.begin();                                // Initialize the I2C bus
  myRTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);  // Initialize the alarms to known values
  myRTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  myRTC.alarm(ALARM_1);                         // Clear the alarm flags
  myRTC.alarm(ALARM_2);
  myRTC.alarmInterrupt(ALARM_1, false);         // Clear the alarm interrupt flags
  myRTC.alarmInterrupt(ALARM_2, false);
  myRTC.squareWave(SQWAVE_NONE);

  // Configure an interrupt on the INT/SQW pin
  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  LowPower.attachInterruptWakeup(RTC_INT_PIN, alarmIsr, FALLING);

  /*
      // Manually set the RTC time
      tm.Hour = 18;
      tm.Minute = 56;
      tm.Second = 45;
      tm.Day = 21;
      tm.Month = 6;
      tm.Year = 2019 - 1970;    // tmElements_t.Year is the offset from 1970
      time_t t = makeTime(tm);  // change the tm structure into time_t (seconds since epoch)
      myRTC.set(t);
  */

  // Print current date and time
  printDateTime(myRTC.get());

  Serial.print(F("messageSize: ")); Serial.println(messageSize);

  // Initialize microSD card
  if (sd.begin(chipSelect, SPI_FULL_SPEED)) {
    logging = true;
    createLogFile();  // Create new log file
    blink(LED_PIN, 20, 100);
  }
  else
  {
    logging = false;  // Disable data logging if microSD initialization fails
    Serial.println("Unable to initialize SD card");
    blink(LED_BUILTIN, 20, 100);
  }

  // Set alarm 1
  //myRTC.setAlarm(ALM1_MATCH_SECONDS, 0, 0, 0, 1);     // Set initial alarm to occur at seconds rollover
  myRTC.setAlarm(ALM1_MATCH_MINUTES, 0, 5, 0, 1);     // Set initial alarm to occur at minutes rollover (start of new hour)
  myRTC.alarm(ALARM_1);                               // Ensure alarm 1 interrupt flag is cleared
  myRTC.alarmInterrupt(ALARM_1, true);                // Enable interrupt output for alarm 1
}

void loop()
{
  if (alarmIsrWasCalled)
  {
    // Check to see if the alarm flag is set (also resets the flag if set)
    if (myRTC.alarm(ALARM_1))
    {
      sampleCounter++;

      resetWatchdog();    // Pet the dog
      myRTC.read(tm);     // Read current date and time
      t = makeTime(tm);   // Change the tm structure into time_t (seconds since epoch)
      printDateTime(t);   // Print ALARM_1 date and time

      // Perform measurements
      readDs3231();       // Read date and time and temperature
      readBattery();      // Read battery voltage
      readTrh();          // Read temperature and humidity
      readAnemometer();   // Read wind speed and direction
      logData();          // Log data to microSD card

      // Perform statistics on samples and write message to transmit buffer
      if (sampleCounter == averageInterval)
      {
        transmitCounter++;
        Serial.print(F("transmitCounter: ")); Serial.println(transmitCounter);
        windVectors();
        printStatistics();
        calculateStatistics();
        writeBuffer();          // Write message to transmit buffer array

        // Transmit data
        if (transmitCounter == transmitInterval)
        {
          transmitData();
          transmitCounter = 0;
          Serial.print(F("Transmit counter: ")); Serial.println(transmitCounter);
        }
        sampleCounter = 0;
        Serial.print(F("Sample counter: ")); Serial.println(sampleCounter);
      }

      // Set alarm 1
      alarmTime = t + sampleInterval;   // Calculate next alarm
      myRTC.setAlarm(ALM1_MATCH_DATE, second(alarmTime), minute(alarmTime), hour(alarmTime), day(alarmTime)); // Set alarm
      myRTC.alarm(ALARM_1);             // Ensure alarm 1 interrupt flag is cleared
      Serial.print(F("Next alarm: "));
      printDateTime(alarmTime);

      // Ensure alarm 1 is not set in the past
      if (myRTC.get() >= alarmTime)
      {
        t = myRTC.get();                  // Read current date and time
        alarmTime = t + sampleInterval;   // Calculate next alarm
        myRTC.setAlarm(ALM1_MATCH_DATE, second(alarmTime), minute(alarmTime), hour(alarmTime), day(alarmTime)); // Set alarm
        myRTC.alarm(ALARM_1);             // Ensure alarm 1 interrupt flag is cleared

        Serial.print(F("Alarm set in the past! New alarm time: "));
        printDateTime(alarmTime);
      }
    }
    alarmIsrWasCalled = false;
  }
  sleeping = true;
  blink(LED_BUILTIN, 2, 100);
  LowPower.deepSleep();   // Enter deep sleep
}

// RTC interrupt service routine (ISR)
void alarmIsr()
{
  alarmIsrWasCalled = true;
}

// Measure internal temperature from DS3231 RTC
void readDs3231()
{
  unixtime = t; // Alarm 1 trigger time
  temperatureDs3231 = myRTC.temperature() / 4.0; // Internal DS3231 temperature

  // Add to statistics object
  rtcStats.add(temperatureDs3231);

  // Write data to union
  message.unixtime = unixtime;

}

// Measure battery voltage from 330kOhm/1MOhm divider
void readBattery()
{
  uint32_t loopStartTime = millis();

  voltage = 0.0;
  analogReadResolution(12);
  for (uint8_t i = 0; i < samplesToAverage; i++)
  {
    voltage += analogRead(VBAT_PIN);
    delay(10);
  }
  voltage /= samplesToAverage;

  voltage *= ((330000.0 + 100000.0) / 100000.0); // Multiply back 100,000 / (330,000 + 100,000) kOhm
  //voltage *= 2;      // Multiply back
  //voltage *= 3.3;   // Multiply by 3.3V reference voltage ***BUG: UNCOMMENT***
  voltage /= 4096;  // Convert to voltage

  // Add to statistics object
  batteryStats.add(voltage);

  Serial.print("voltage: "); Serial.println(voltage);

  uint32_t batteryLoopTime = millis() - loopStartTime;
  //Serial.print("readBattery() function execution: "); Serial.print(batteryLoopTime); Serial.println(F(" ms"));
}

void readTrh()
{
  // Enable power to temperature/relative humidity sensor
  //digitalWrite(GPIO_PWR_2_PIN, HIGH); // Always on due to bug

  // Read sensor
  temperature = sht.readTemperatureC();
  humidity = sht.readHumidity();
  Wire.begin(); // Reinitialize I2C ***BUG***

  // Write data to union
  message.temperature = temperature * 100; // ***BUG: REMOVE***
  message.humidity = humidity * 100; // ***BUG: REMOVE***

  // Add to statistics object
  temperatureStats.add(temperature);
  humidityStats.add(humidity);

  Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");

  // Disable power to temperature/relative humidity sensor
  //digitalWrite(GPIO_PWR_2_PIN, LOW);
}

// Measure wind speed and direction from Davis Instruments 7911 anemometer
void readAnemometer()
{
  uint32_t startTime = millis();
  // Enable power to anemometer
  digitalWrite(GPIO_PWR_1_PIN, HIGH);

  // Attach interrupts to wind speed input pins
  attachInterrupt(WIND_SPEED_1_PIN, windSpeedIsr1, FALLING);
  revolutions1 = 0;

  // Measure wind speed for duration of samplePeriod
  while (millis() < startTime + (samplePeriod * 1000))
  {
    resetWatchdog(); // Reset Watchdog Timer
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
  for (uint8_t i = 0; i < 5; i++)
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
    rvWindDirection1 += 360;

  float rvWindSpeed1 = sqrt(sq(veStats1.average()) + sq(vnStats1.average())); // Resultant mean wind speed 1

  if ((rvWindDirection1 == 0) && (rvWindSpeed1 != 0))
    rvWindDirection1 = 360;

  if (rvWindSpeed1 == 0)
    rvWindDirection1 = 0;

  // Wind direction "from" correction
  if (rvWindDirection1 < 180)
    rvWindDirection1 += 180;
  else if (rvWindDirection1 > 180)
    rvWindDirection1 -= 180;

  // Write data to union
  message.windSpeed1 = rvWindSpeed1 * 100;      // Resultant mean wind speed 1 (m/s)
  message.windDirection1 = rvWindDirection1;    // Resultant mean wind direction 1 (°)
}

// Wind speed 1 interrupt service routine (ISR)
void windSpeedIsr1()
{
  revolutions1++;
}

// Create log file
void createLogFile()
{
  if (logging == true)
  {
    if (file.isOpen())
      file.close();

    // Select a unique log file name
    for (uint16_t i = 0; i < 999; i++) {
      snprintf(fileName, sizeof(fileName), "log%03d.csv", i);
      // If O_CREAT and O_EXCL are set, open() will fail if the file already exists
      if (file.open(fileName, O_CREAT | O_EXCL | O_WRITE))
      {
        break; // Break out of loop upon successful file creation
      }
    }

    if (!file.isOpen())
    {
      Serial.println(F("Unable to open file"));
    }

    // Read current date and time from the RTC
    myRTC.read(tm);

    // Set the file's creation date and time
    if (!file.timestamp(T_CREATE, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
    {
      Serial.println("Set create time failed");
    }

    // Write header to file
    file.println("SAMPLE,UNIXTIME,VBAT,TI,TA,RH,WS1,WD1,WG1,WGD1,WS2,WD2,WG2,WGD2,RAM");

    // Close log file
    file.close();

    Serial.println(F("New log file created: "));
    Serial.println(fileName);
  }
}

// Write data to log file
void logData()
{

  if (logging == true)
  {
    // Check that maximum file sample limit has not been exceeded
    if (samplesSaved >= samplesPerFile)
    {
      createLogFile();
      samplesSaved = 0;
    }

    // Write to microSD card
    if (file.open(fileName, O_APPEND | O_WRITE))
    {
      samplesSaved++;   //  Increment sample count of current file
      file.print(samplesSaved);
      file.write(",");
      file.print(unixtime);
      file.write(",");
      file.print(voltage);
      file.write(",");
      file.print(temperatureDs3231);
      file.write(",");
      file.print(temperature);
      file.write(",");
      file.print(humidity);
      file.write(",");
      file.print(windSpeed1);
      file.write(",");
      file.print(windDirection1);
      file.write(",");
      file.print(windGust1);
      file.write(",");
      file.print(windGustDirection1);
      file.write(",");
      file.println(freeRam());
      writeTimestamps();
      file.close();

      Serial.print(samplesSaved);
      Serial.print(",");
      Serial.print(unixtime);
      Serial.print(",");
      Serial.print(voltage);
      Serial.print(",");
      Serial.print(temperatureDs3231);
      Serial.print(",");
      Serial.print(temperature);
      Serial.print(",");
      Serial.print(humidity);
      Serial.print(",");
      Serial.print(windSpeed1);
      Serial.print(",");
      Serial.print(windDirection1);
      Serial.print(",");
      Serial.print(windGust1);
      Serial.print(",");
      Serial.print(windGustDirection1);
      Serial.print(",");
      Serial.println(freeRam());
      blink(LED_PIN, 2, 100);
    }
    else
    {
      Serial.println(F("Unable to open file"));
    }

    // Force data to SD and update the directory entry to avoid data loss
    if (!file.sync() || file.getWriteError())
    {
      Serial.println(F("Write error"));
      delay(10000); // ***REMOVE***
    }
  }
}

// Log file write and access timestamps
void writeTimestamps()
{
  myRTC.read(tm); // Read current date and time from the RTC

  // Set the file's last write/modification date and time
  if (!file.timestamp(T_WRITE, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
  {
    Serial.println(F("Set write time failed"));
  }

  // Set the file's last access date and time
  if (!file.timestamp(T_ACCESS, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
  {
    Serial.println(F("Set access time failed"));
  }
}

// Calculate statistics and clear objects
void calculateStatistics()
{
  // Write data to union
  message.voltage = batteryStats.minimum() * 1000;          // Minimum battery voltage (mV)
  message.temperature = temperatureStats.average() * 100;   // Mean temperature (°C)
  message.humidity = humidityStats.average() * 100;         // Mean humidity (%)
  message.temperatureDs3231 = rtcStats.average() * 100;     // Mean RTC temperature (°C)

  // Clear statistics objects
  batteryStats.clear();
  humidityStats.clear();
  rtcStats.clear();
  temperatureStats.clear();
  windSpeedStats1.clear();
  veStats1.clear();
  vnStats1.clear();
  windGust1 = 0.0;
  windGustDirection1 = 0.0;
}

// Write
void writeBuffer()
{
  messageCounter++;
  message.messageCounter = messageCounter;

  // Concatenate current message with existing message(s) stored in transmit buffer
  memcpy(transmitBuffer + (sizeof(message) * (transmitCounter + (retransmitCounter * transmitInterval) - 1)), message.bytes, sizeof(message)); // Copy message to transmit buffer

  // Print data
  printUnion();             // Print data stored in union
  //printUnionBinary();       // Print data stored in union in binary format
  //printTransmitBuffer();    // Print data stored transmit buffer array in binary format

  // Clear data stored in union
  memset(message.bytes, 0x00, sizeof(message));
}

// Transmit data via the RockBLOCK 9603 satellite modem
void transmitData()
{
  uint16_t err;
  uint32_t loopStartTime = millis();

  // Enable power to RockBLOCK
  digitalWrite(ROCKBLOCK_EN_PIN, HIGH);

  // Start the serial power connected to the RockBLOCK modem
  IridiumSerial.begin(19200);

  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(ROCKBLOCK_RX_PIN, PIO_SERCOM);
  pinPeripheral(ROCKBLOCK_TX_PIN, PIO_SERCOM);

  // Start the RockBLOCK modem
  err = modem.begin();
  if (err == ISBD_SUCCESS)
  {
    uint8_t inBuffer[11];   // Buffer to store incoming RockBLOCK transmission (340 byte limit per message)
    size_t inBufferSize = sizeof(inBuffer);
    memset(inBuffer, 0x00, sizeof(inBuffer));   // Clear  inBuffer array

#if DEBUG
    // Iridium modem diagnostics
    int signalQuality = -1;
    err = modem.getSignalQuality(signalQuality);
    if (err != ISBD_SUCCESS)
    {
      Serial.print(F("SignalQuality failed: error "));
      Serial.println(err);
      return;
    }
    Serial.print(F("Signal quality: "));
    Serial.println(signalQuality);
#endif

    // Transmit and receieve data in binary format
    err = modem.sendReceiveSBDBinary(transmitBuffer, (sizeof(message) * (transmitCounter + (retransmitCounter * transmitInterval))), inBuffer, inBufferSize);

    if (err == ISBD_SUCCESS)
    {
      transmitCounter = 0;
      retransmitCounter = 0;
      memset(transmitBuffer, 0x00, sizeof(transmitBuffer));   // Clear transmit buffer array

      // Check for incoming message. If no inbound message is available, inBufferSize = 0
      if (inBufferSize > 0)
      {
        // Print inBuffer size and values of each incoming byte of data
        Serial.print(F("Inbound buffer size is: ")); Serial.println(inBufferSize);
        for (uint8_t i = 0; i < inBufferSize; i++)
        {
          Serial.print(F("address: ")); Serial.print(i); Serial.print(F("\tvalue: ")); Serial.println(inBuffer[i], HEX);
        }

        // Recompose variables using bitshift
        uint8_t  forceResetBuffer           = (((uint8_t)inBuffer[10] << 0) & 0xFF);
        uint16_t maxRetransmitCounterBuffer = (((uint16_t)inBuffer[9] << 0) & 0xFF) +
                                              (((uint16_t)inBuffer[8] << 8) & 0xFFFF);
        uint16_t transmitIntervalBuffer     = (((uint16_t)inBuffer[7] << 0) & 0xFF) +
                                              (((uint16_t)inBuffer[6] << 8) & 0xFFFF);
        uint16_t averageIntervalBuffer      = (((uint16_t)inBuffer[5] << 0) & 0xFF) +
                                              (((uint16_t)inBuffer[4] << 8) & 0xFFFF);
        uint32_t sampleIntervalBuffer       = (((uint32_t)inBuffer[3] << 0) & 0xFF) +
                                              (((uint32_t)inBuffer[2] << 8) & 0xFFFF) +
                                              (((uint32_t)inBuffer[1] << 16) & 0xFFFFFF) +
                                              (((uint32_t)inBuffer[0] << 24) & 0xFFFFFFFF);

        // Check validity of incoming data
        if ((sampleIntervalBuffer > 0  && sampleIntervalBuffer <= 86400) &&
            (averageIntervalBuffer > 0  && averageIntervalBuffer <= 24) &&
            (transmitIntervalBuffer > 0  && transmitIntervalBuffer <= 10) &&
            (maxRetransmitCounterBuffer > 0  && maxRetransmitCounterBuffer <= 10) &&
            (forceResetBuffer == 0 || forceResetBuffer == 255))
        {
          sampleInterval = sampleIntervalBuffer;
          averageInterval = averageIntervalBuffer;
          transmitInterval = transmitIntervalBuffer;
          maxRetransmitCounter = maxRetransmitCounterBuffer;
          forceReset = forceResetBuffer;
        }
      }
    }
    else
    {
      Serial.print(F("Transmission failed: error "));
      Serial.println(err);
    }
  }
  else
  {
    Serial.print(F("Begin failed: error "));
    Serial.println(err);
  }

  // If transmission or modem begin fail, stored message in transmit buffer
  if (err != ISBD_SUCCESS)
  {
    retransmitCounter++;  // Increment retransmit counter

    // Reset retransmit counter if maximum messages reattempts is exceeded
    if (retransmitCounter >= maxRetransmitCounter)
    {
      transmitCounter = 0;
      retransmitCounter = 0;
      memset(transmitBuffer, 0x00, sizeof(transmitBuffer));   // Clear transmitBuffer array
    }
  }

  transmitDuration = millis() - loopStartTime;
  message.transmitDuration = transmitDuration / 1000;

  // Put the RockBLOCK into low power "sleep" mode
  err = modem.sleep();
  if (err != ISBD_SUCCESS)
  {
    Serial.print(F("Sleep failed: error "));
    Serial.println(err);
  }

  // Close the serial port connected to the RockBLOCK modem
  IridiumSerial.end();

  // Disable power to RockBLOCK
  digitalWrite(ROCKBLOCK_EN_PIN, LOW);

#if DEBUG
  Serial.print(F("transmitDuration: ")); Serial.println(transmitDuration / 1000);
  Serial.print(F("retransmitCounter: ")); Serial.println(retransmitCounter);
  Serial.print(F("sampleInterval: ")); Serial.println(sampleInterval);
  Serial.print(F("averageInterval: ")); Serial.println(averageInterval);
  Serial.print(F("transmitInterval: ")); Serial.println(transmitInterval);
  Serial.print(F("maxRetransmitCounter: ")); Serial.println(maxRetransmitCounter);
  Serial.print(F("forceReset: ")); Serial.println(forceReset);
#endif

  // Check if forced reset is required
  if (forceReset == 255)
  {
    digitalWrite(13, HIGH);
    while (true); // Watchdog Timer will reset system afer 8-16 seconds
  }
}

// RockBLOCK callback function
bool ISBDCallback()   // This function can be repeatedly called during data transmission or GPS signal acquisitionb
{
#if DEBUG
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, (millis() / 1000) % 2 == 1 ? HIGH : LOW);
  pinMode(LED_BUILTIN, INPUT);
#endif

  uint32_t currentMillis = millis();
  if (currentMillis - previousMillis >= 2000)
  {
    previousMillis = currentMillis;
    readBattery();    // Measure battery voltage
    resetWatchdog();  // Pet the dog
  }
  return true;
}

// Callback to sniff the conversation with the Iridium modem
void ISBDConsoleCallback(IridiumSBD * device, char c)
{
#if DEBUG
  Serial.write(c);
#endif
}

// Callback to to monitor library's run state
void ISBDDiagsCallback(IridiumSBD * device, char c)
{
#if DEBUG
  Serial.write(c);
#endif
}

// LED activity indicator
void blink(uint8_t pin, uint16_t flashes, uint16_t duration)
{
  pinMode(pin, OUTPUT);
  for (uint16_t i = 0; i < flashes; i++)
  {
    digitalWrite(pin, HIGH);
    delay(duration);
    digitalWrite(pin, LOW);
    delay(duration);
  }
  pinMode(pin, INPUT);
}

// Print current time and date
void printDateTime(time_t t)
{
  Serial.print((day(t) < 10) ? "0" : ""); Serial.print(day(t), DEC); Serial.print('/');
  Serial.print((month(t) < 10) ? "0" : ""); Serial.print(month(t), DEC); Serial.print('/');
  Serial.print(year(t), DEC); Serial.print(' ');
  Serial.print((hour(t) < 10) ? "0" : ""); Serial.print(hour(t), DEC); Serial.print(':');
  Serial.print((minute(t) < 10) ? "0" : ""); Serial.print(minute(t), DEC); Serial.print(':');
  Serial.print((second(t) < 10) ? "0" : ""); Serial.println(second(t), DEC);
}

// Print statisticsitre
void printStatistics()
{
  Serial.println();
  Serial.println(F("Statistics"));
  Serial.println(F("============================================================================"));
  Serial.print(F("Voltage\t\t"));
  Serial.print(F("Samples: ")); Serial.print(batteryStats.count());
  Serial.print(F("\tMin: "));   Serial.print(batteryStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(batteryStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(batteryStats.average());
  Serial.print(F("Temperature (RTC)\t"));
  Serial.print(F("Samples: ")); Serial.print(rtcStats.count());
  Serial.print(F("\tMin: ")); Serial.print(rtcStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(rtcStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(rtcStats.average());
  Serial.print(F("Temperature\t"));
  Serial.print(F("Samples: ")); Serial.print(temperatureStats.count());
  Serial.print(F("\tMin: ")); Serial.print(temperatureStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(temperatureStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(temperatureStats.average());
  Serial.print(F("Humidity\t"));
  Serial.print(F("Samples: ")); Serial.print(humidityStats.count());
  Serial.print(F("\tMin: ")); Serial.print(humidityStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(humidityStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(humidityStats.average());
  Serial.print(F("Wind speed 1\t"));
  Serial.print(F("Samples: ")); Serial.print(windSpeedStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(windSpeedStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(windSpeedStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(windSpeedStats1.average());
  Serial.print(F("vn 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(vnStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(vnStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(vnStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(vnStats1.average());
  Serial.print(F("ve 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(veStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(veStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(veStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(veStats1.average());
}

// Print union/structure
void printUnion()
{
  Serial.println();
  Serial.println(F("Union/structure"));
  Serial.println(F("==================================="));
  Serial.print(F("unixtime:\t\t")); Serial.println(message.unixtime);
  Serial.print(F("temperatureDs3231:\t")); Serial.println(message.temperatureDs3231);
  Serial.print(F("temperature:\t\t")); Serial.println(message.temperature);
  Serial.print(F("humidity:\t\t")); Serial.println(message.humidity);
  Serial.print(F("windSpeed 1:\t\t")); Serial.println(message.windSpeed1);
  Serial.print(F("windDirection 1:\t")); Serial.println(message.windDirection1);
  Serial.print(F("windGust 1:\t\t")); Serial.println(message.windGust1);
  Serial.print(F("windGustDirection 1:\t")); Serial.println(message.windGustDirection1);
  Serial.print(F("voltage:\t\t")); Serial.println(message.voltage);
  Serial.print(F("transmitDuration:\t\t")); Serial.println(message.transmitDuration);
  Serial.print(F("messageCounter:\t\t")); Serial.println(message.messageCounter);

}

// Print contents of union/structure
void printUnionBinary()
{
  Serial.println();
  Serial.println(F("Union/structure"));
  Serial.println(F("========================="));
  Serial.println(F("Byte\tHex\tBinary"));
  for (uint16_t i = 0; i < sizeof(message); ++i)
  {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(message.bytes[i], HEX);
    Serial.print("\t");
    Serial.println(message.bytes[i], BIN);
  }
}

// Print contents of transmiff buffer array
void printTransmitBuffer()
{
  Serial.println();
  Serial.println(F("Transmit buffer"));
  Serial.println(F("========================="));
  Serial.println(F("Byte\tHex\tBinary"));
  for (uint16_t i = 0; i < 340; i++)
  {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(transmitBuffer[i], HEX);
    Serial.print("\t");
    Serial.println(transmitBuffer[i], BIN);
  }
}

// Display availale RAM
int freeRam()
{
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}

// Set up the WDT to perform a system reset if the loop() blocks for more than 16 seconds
void configureWatchdog()
{
  // Set up the generic clock (GCLK2) used to clock the watchdog timer at 1.024kHz
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(4) |            // Divide the 32.768kHz clock source by divisor 32, where 2^(4 + 1): 32.768kHz/32=1.024kHz
                    GCLK_GENDIV_ID(2);              // Select Generic Clock (GCLK) 2
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_DIVSEL |          // Set to divide by 2^(GCLK_GENDIV_DIV(4) + 1)
                     GCLK_GENCTRL_IDC |             // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |           // Enable GCLK2
                     GCLK_GENCTRL_SRC_OSCULP32K |   // Set the clock source to the ultra low power oscillator (OSCULP32K)
                     GCLK_GENCTRL_ID(2);            // Select GCLK2
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  // Feed GCLK2 to WDT (Watchdog Timer)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |           // Enable GCLK2 to the WDT
                     GCLK_CLKCTRL_GEN_GCLK2 |       // Select GCLK2
                     GCLK_CLKCTRL_ID_WDT;           // Feed the GCLK2 to the WDT
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  WDT->EWCTRL.bit.EWOFFSET = 0xA;                   // Set the Early Warning Interrupt Time Offset to 8 seconds //REG_WDT_EWCTRL = WDT_EWCTRL_EWOFFSET_8K;
  WDT->INTENSET.bit.EW = 1;                         // Enable the Early Warning interrupt //REG_WDT_INTENSET = WDT_INTENSET_EW;
  WDT->CONFIG.bit.PER = 0xB;                        // Set the WDT reset timeout to 16 seconds //REG_WDT_CONFIG = WDT_CONFIG_PER_16K;
  WDT->CTRL.bit.ENABLE = 1;                         // Enable the WDT in normal mode //REG_WDT_CTRL = WDT_CTRL_ENABLE;
  while (WDT->STATUS.bit.SYNCBUSY);                 // Await synchronization of registers between clock domains

  // Configure and enable WDT interrupt
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);                    // Top priority
  NVIC_EnableIRQ(WDT_IRQn);
}

// Pet the Watchdog Timer
void resetWatchdog()
{
  WDT->CLEAR.bit.CLEAR = 0xA5;                      // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
  while (WDT->STATUS.bit.SYNCBUSY);                 // Await synchronization of registers between clock domains
}

// Watchdog Timer interrupt service routine (ISR)
void WDT_Handler()
{
  if (sleeping)
  {
    sleeping = false;
    WDT->INTFLAG.bit.EW = 1;                        // Clear the Early Warning interrupt flag //REG_WDT_INTFLAG = WDT_INTFLAG_EW;
    WDT->CLEAR.bit.CLEAR = 0xA5;                    // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
    while (WDT->STATUS.bit.SYNCBUSY);               // Await synchronization of registers between clock domains
  }
  else
  {
    //WDT->CTRL.bit.ENABLE = 0;                       // Debug: Disable Watchdog Timer
    //pinMode(13, OUTPUT);
    //digitalWrite(13, HIGH);                         // Debug: Turn on LED to indicate Watchdog trigger
    while (true);                                   // Force Watchdog Timer to reset the system
  }
}
