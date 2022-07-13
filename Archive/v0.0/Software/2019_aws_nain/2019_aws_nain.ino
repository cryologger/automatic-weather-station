/*
  Title:          Cryologger - Automatic Weather Station (AWS)
  Last modified:  January 23, 2019
  Author:         Adam Garbo
  Deployed:       Nain, NL

  Components:
  - Adafruit Feather M0 Adalogger
  - Adafruit DS3231 RTC Precision Featherwing
  - Davis Instruments 7911 Anemomenter
  - Davis Instruments 7817 Thermistor
  - Rock Seven RockBLOCK 9603
  - Pololu 3.3V, 600mA Step-Down Voltage Regulator D36V6F3
  - 256 MB microSD card (industrial rated)
  
  Comments:
  - Deployed in Nain, Newfoundland and Labrador in January, 2019.
*/

#include <Arduino.h>            // https://github.com/arduino/ArduinoCore-samd (Required before wiring_private.h)
#include <ArduinoLowPower.h>    // https://github.com/arduino-libraries/ArduinoLowPower
#include <DS3232RTC.h>          // https://github.com/JChristensen/DS3232RTC
#include <IridiumSBD.h>         // https://github.com/mikalhart/IridiumSBD
#include <math.h>               // https://www.nongnu.org/avr-libc/user-manual/group__avr__math.html
#include <Statistic.h>          // https://github.com/RobTillaart/Arduino/tree/master/libraries/Statistic
#include <SdFat.h>              // https://github.com/greiman/SdFat
#include <SparkFunBME280.h>     // https://github.com/sparkfun/SparkFun_BME280_Arduino_Library
#include <SPI.h>                // https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>               // https://www.arduino.cc/en/Reference/Wire
#include <wiring_private.h>     // pinPeripheral() function

// Required for Serial when selecting a board type other than Adafruit Feather M0
#if defined(ARDUINO_SAMD_ZERO) && defined(SERIAL_PORT_USBVIRTUAL)
#define Serial SERIAL_PORT_USBVIRTUAL
#endif

// Defined constants
#define GPIO_PWR_PIN              A3
#define LED_PIN                   8
#define RTC_INT_PIN               5
#define ROCKBLOCK_EN_PIN          6
#define ROCKBLOCK_SLEEP_PIN       12
#define ROCKBLOCK_RX_PIN          11
#define ROCKBLOCK_TX_PIN          10
#define THERMISTOR_PIN            A4
#define WIND_SPEED_PIN            A1
#define WIND_DIRECTION_PIN        A2
#define VBAT_PIN                  A0

#define DEBUG true

// Create a new Serial instance. For more information see: https://www.arduino.cc/en/Tutorial/SamdSercom
Uart Serial2 (&sercom1, ROCKBLOCK_RX_PIN, ROCKBLOCK_TX_PIN, SERCOM_RX_PAD_0, UART_TX_PAD_2);  // Create the new UART instance assigning it to pin 10 and 11
#define IridiumSerial Serial2

// Attach the interrupt handler to the SERCOM
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

// Object instantiations
BME280        mySensor;
DS3232RTC     myRTC(false);     // Tell constructor not to initialize the I2C bus
IridiumSBD    modem(IridiumSerial, ROCKBLOCK_SLEEP_PIN);
SdFat         sd;               // File system object
SdFile        file;             // Log file
time_t        t, unixtime, alarmTime;
tmElements_t  tm;

// Statistic objects
Statistic batteryStats;           // Battery voltage statistics
Statistic temperatureIntStats;    // BME280 internal temperature statistics
Statistic humidityStats;          // BME280 internal humidity statistics
Statistic pressureStats;          // BME280 internal pressure statistics
Statistic temperatureExtStats;    // Davis 7817 thermistor external temperature statistics
Statistic windSpeedStats;         // Anemometer wind speed statistics
Statistic vnStats;                // Anemoeter north-south wind vector component (v) statistics
Statistic veStats;                // Anemometer east-west wind vector component (u) statistics

// User defined global variable declarations
uint32_t            sampleInterval          = 300;            // Sleep duration (in seconds) between data sample acquisitions. Default = 5 minutes (300 seconds)
uint16_t            averageInterval         = 12;             // Number of samples to be averaged for each RockBLOCK transmission. Default = 12 (Hourly)
uint16_t            transmitInterval        = 1;              // Number of message to be included in a single transmission (340 byte limit). Default = 3 (Every 3 hours)
uint16_t            maxRetransmitCounter    = 10;             // Maximum number of failed data transmissions to reattempt in a single message (340 byte limit). Default: 10
uint16_t            samplesPerFile          = 288;            // Maximum number of samples stored in a file before new log file creation (Default: 3 days * 288 samples per day)

// Global variable declarations
const uint8_t       chipSelect              = 4;              // MicroSD chip select pin
const uint8_t       nominalTemperature      = 25;             // Thermistor temperature for nominal resistance (°C)
const uint8_t       samplesToAverage        = 10;             // Thermistor samples to average
const uint16_t      betaCoefficient         = 3850;           // Thermistor beta coefficient
const uint16_t      seriesResistor          = 10000;          // Thermistor resistor divider value (Ohms)
const uint16_t      nominalResistance       = 10000;          // Thermistor nominal resistance at 25°C
volatile bool       alarmIsrWasCalled       = false;          // RTC interrupt service routine (ISR) flag
volatile bool       sleeping                = false;          // Watchdog Timer Early Warning interrupt flag
volatile uint16_t   revolutions             = 0;              // Wind speed ISR revolutions counter
volatile uint32_t   contactBounceTime       = 0;
bool                logging                 = true;           // MicroSD initilization flag
bool                messageSent             = true;           // RockBLOCK transmission flag
uint8_t             transmitBuffer[340]     = {};             // RockBLOCK transmission buffer
char                fileName[12]            = "log000.csv";   // Log file naming convention
float               humidityBme280          = 0.0;            // BME280 humidity (%)
float               pressureBme280          = 0.0;            // BME280 pressure (Pa)
float               temperature             = 0.0;            // Thermistor temperature (°C)
float               temperatureBme280       = 0.0;            // BME280 temperature (°C)
float               temperatureDs3231       = 0.0;            // Internal RTC temperature (°C)
float               voltage                 = 0.0;            // Battery voltage in volts (V)
float               windSpeed               = 0.0;            // Wind speed in metres per second (m/s)
uint16_t            samplesSaved            = 0;              // Log file sample counter
uint16_t            samplePeriod            = 3;              // Wind speed sample period in seconds (s)
uint16_t            retransmitCounter       = 0;              // RockBLOCK failed data transmission counter
uint16_t            windDirection           = 0;              // Wind direction in degrees (°)
uint16_t            sampleCounter           = 0;              // RockBLOCK transmission interval counter
uint16_t            messageCounter          = 0;              // RockBLOCK transmission interval counter
uint16_t            iterationCounter        = 0;              // RockBLOCK transmission counter
uint32_t            previousMillis          = 0;              // RockBLOCK callback function timer variable
uint32_t            transmitDuration        = 0;              // RockBLOCK data transmission time variable

extern "C" char *sbrk(int i);                                 // Free RAM function

// Structure and union to store and send data byte-by-byte via RockBLOCK
typedef union
{
  struct
  {
    uint32_t    unixtime;             // Date and time in time_t format             (4 bytes)
    int16_t     temperatureExt;       // Mean thermistor temperature (°C)           (2 bytes) (temperatureExternal * 100)
    int16_t     temperatureInt;       // Mean internal temperature (°C)             (2 bytes) (temperatureInternal * 100)
    uint16_t    humidity;             // Mean internal humidity (%)                 (2 bytes) (humidity * 100)
    uint32_t    pressure;             // Mean internal pressure (Pa)                (4 bytes)
    uint16_t    windSpeed;            // Resultant mean wind speed (m/s)            (2 bytes) (windSpeed * 100)
    uint16_t    windDirection;        // Resultant mean wind direction (°)          (2 bytes)
    uint16_t    windGust;             // Maximum wind gust speed (m/s)              (2 bytes) (windGust * 100)
    uint16_t    windGustDirection;    // Maximum wind speed direction (°)           (2 bytes)
    uint16_t    voltage;              // Minimum battery voltage (mV)               (2 bytes) (voltage * 1000)
    uint16_t    transmitDuration;     // Debugging variable                         (2 bytes)
    uint16_t    iterationCounter;     // RockBLOCK data transmission counter        (2 bytes)

  } __attribute__((packed));                                                        // Total = 28 bytes
  uint8_t bytes[];
} SBDMESSAGE;

SBDMESSAGE message;
size_t messageSize = sizeof(message);     // Size (in bytes) of data to be stored and transmitted

void setup()
{
  // Pin assignments
  pinMode(GPIO_PWR_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ROCKBLOCK_EN_PIN, OUTPUT);
  pinMode(THERMISTOR_PIN, INPUT);
  pinMode(WIND_SPEED_PIN, INPUT);
  digitalWrite(GPIO_PWR_PIN, LOW);
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
  modem.adjustSendReceiveTimeout(60);
  modem.adjustATTimeout(20);

  // BME280
  mySensor.beginI2C();
  mySensor.setMode(MODE_SLEEP);

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
      tm.Hour = 17;
      tm.Minute = 07;
      tm.Second = 0;
      tm.Day = 23;
      tm.Month = 1;
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
      readDs3231();
      readBattery();
      readBme280();
      readThermistor();
      readAnemometer();
      logData();

      // Perform statistics on samples and write message to transmit buffer
      if (sampleCounter == averageInterval)
      {
        messageCounter++;
        Serial.print(F("messageCounter: ")); Serial.println(messageCounter);
        windVectors();
        printStatistics();
        calculateStatistics();
        writeBuffer();          // Write message to transmit buffer array

        // Transmit data
        if (messageCounter == transmitInterval)
        {
          transmitData();
          messageCounter = 0;
          Serial.print(F("messageCounter: ")); Serial.println(messageCounter);
        }
        sampleCounter = 0;
        Serial.print(F("sampleCounter: ")); Serial.println(sampleCounter);
      }

      // Set alarm 1
      alarmTime = t + sampleInterval;  // Calculate next alarm

      myRTC.setAlarm(ALM1_MATCH_DATE, second(alarmTime), minute(alarmTime), hour(alarmTime), day(alarmTime)); // Set alarm
      myRTC.alarm(ALARM_1);             // Ensure alarm 1 interrupt flag is cleared
      Serial.print(F("Next alarm: "));
      printDateTime(alarmTime);
      // Ensure alarm 1 is not set in the past
      if (myRTC.get() >= alarmTime)
      {
        t = myRTC.get();                  // Read current date and time
        alarmTime = t + sampleInterval;    // Calculate next alarm
        myRTC.setAlarm(ALM1_MATCH_DATE, second(alarmTime), minute(alarmTime), hour(alarmTime), day(alarmTime)); // Set alarm
        myRTC.alarm(ALARM_1);             // Ensure alarm 1 interrupt flag is cleared

        Serial.print(F("Alarm set in the past! New alarm time: "));
        printDateTime(alarmTime);
      }
    }
    alarmIsrWasCalled = false;
  }
  sleeping = true;
  blink(LED_BUILTIN, 1, 5);
  LowPower.deepSleep();
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

  // Write data to union
  message.unixtime = unixtime;
}

// Measure battery voltage from 330kOhm/1MOhm divider
void readBattery()
{
  voltage = 0;
  for (uint8_t i = 0; i < samplesToAverage; i++)
  {
    voltage += (analogRead(VBAT_PIN) - 5);
    delay(10);
  }
  voltage /= samplesToAverage;
  voltage *= 4.0868; // Multiply back (1035000) + 335300 / 335300
  voltage *= 3.3;   // Multiply by 3.3V reference voltage
  voltage /= 1024;  // Convert to voltage

  // Add to statistics object
  batteryStats.add(voltage);
}

void readBme280() {
  mySensor.setMode(MODE_FORCED);
  temperatureBme280 = mySensor.readTempC();
  humidityBme280 = mySensor.readFloatHumidity();
  pressureBme280 = mySensor.readFloatPressure();

  temperatureIntStats.add(temperatureBme280);
  humidityStats.add(humidityBme280);
  pressureStats.add(pressureBme280);
}

// Measure temperature from Davis Instruments 7817 thermistor
void readThermistor()
{
  digitalWrite(GPIO_PWR_PIN, HIGH);

  float average = 0;
  for (uint8_t i = 0; i < samplesToAverage; i++)
  {
    average += analogRead(THERMISTOR_PIN);
    delay(10);
  }
  digitalWrite(GPIO_PWR_PIN, LOW);

  // Average samples
  average /= samplesToAverage;
  average = 1023 / average - 1;                     // Convert to resistance
  average = seriesResistor / average;

  // Steinhart–Hart equation
  float steinhart = average / nominalResistance;    // (R/Ro)
  steinhart = log(steinhart);                       // ln(R/Ro)
  steinhart /= betaCoefficient;                     // 1/B * ln(R/Ro)
  steinhart += 1.0 / (nominalTemperature + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                      // Invert
  steinhart -= 273.15;                              // Convert to Celsius
  temperature = steinhart;

  // Add to statistics object
  temperatureExtStats.add(steinhart);
}

// Measure wind speed and direction from Davis Instruments 7911 anemometer
void readAnemometer()
{
  // Measure wind speed
  uint32_t startTime = millis();
  digitalWrite(GPIO_PWR_PIN, HIGH);
  attachInterrupt(WIND_SPEED_PIN, windSpeedIsr, FALLING);
  revolutions = 0;
  while (millis() < startTime + (samplePeriod * 1000))
  {
    // Measure wind speed for duration of samplePeriod
  }
  detachInterrupt(WIND_SPEED_PIN);
  // Wind speed formula: V = P(2.25/T) where V = speed in mph, P = no. of pulses in sample period, T = duration of sample period in seconds
  windSpeed = revolutions * (2.25 / samplePeriod);   // Wind speed in miles per hour
  //windSpeed *= 1.609344;    // Convert to kilometres per hour
  windSpeed *= 0.44704;     // Convert to metres per second

  // Measure wind direction
  for (uint8_t i = 0; i < 5; i++)
  {
    analogRead(WIND_DIRECTION_PIN);
    delay(1);
  }
  windDirection = analogRead(WIND_DIRECTION_PIN);         // Read raw analog wind direction value
  windDirection = map(windDirection, 0, 1023, 0, 359);    // Map wind direction to degrees (0-360°)

  digitalWrite(GPIO_PWR_PIN, LOW);

  if (windDirection > 360)
    windDirection -= 360;

  if (windDirection < 0)
    windDirection += 360;

  // True wind direction
  //if (windDirection > 180)
  //  windDirection -= 180;
  //else if (windDirection < 180)
  //  windDirection += 180;

  // Wind gust and direction
  if ((windSpeed > 0) && (windSpeed * 100 > message.windGust))
  {
    message.windGust = windSpeed * 100;
    if (windDirection > 180)
      message.windGustDirection = windDirection - 180;
    if (windDirection < 180)
      message.windGustDirection = windDirection + 180;
  }

  // Wind vector calculations
  float windDirectionRadians = windDirection * DEG_TO_RAD;  // Convert wind direction from degrees to radians
  float vn = -1.0 * windSpeed * cos(windDirectionRadians);  // Magnitude of the north-south component (v) of the resultant vector mean wind
  float ve = -1.0 * windSpeed * sin(windDirectionRadians);  // Magnitude of the east-west component (u) of the resultant vector mean wind

  // Add to wind statistics
  windSpeedStats.add(windSpeed);
  vnStats.add(vn);
  veStats.add(ve);
}

// Calculate resultant mean wind speed and direction vectors
// For more information see: http://www.webmet.com/met_monitoring/622.html
void windVectors()
{
  float rvWindDirection = atan2(veStats.average(), vnStats.average()); // Resultant mean wind direction
  rvWindDirection *= RAD_TO_DEG;  // Convert from radians to degrees

  if (rvWindDirection < 0)
    rvWindDirection += 360;

  float rvWindSpeed = sqrt(sq(veStats.average()) + sq(vnStats.average())); // Resultant mean wind speed

  if ((rvWindDirection == 0) & (rvWindSpeed != 0))
    rvWindDirection = 360;

  if (rvWindSpeed == 0)
    rvWindDirection = 0;

  // Write data to union
  message.windSpeed = rvWindSpeed * 100;   // Resultant mean wind speed (m/s)
  message.windDirection = rvWindDirection;   // Resultant mean wind direction (°)
}

// Wind speed interrupt service routine
void windSpeedIsr()
{
  revolutions++;
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
      Serial.println(F("Unable to open file"));

    // Read current date and time from the RTC
    myRTC.read(tm);

    // Set the file's creation date and time
    if (!file.timestamp(T_CREATE, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
      Serial.println("Set create time failed");

    // Write header to file
    file.println("unixtime,batteryVoltage,temperatureDs3231,temperatureBme280,humidityBme280,pressureBme280,temperature,windSpeed,windDirection,freeRam,samplesSaved");

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

    if (file.open(fileName, O_APPEND | O_WRITE))
    {
      samplesSaved++;  //  Increment sample count of current file
      file.print(unixtime);
      file.write(",");
      file.print(voltage);
      file.write(",");
      file.print(temperatureDs3231);
      file.write(",");
      file.print(temperatureBme280);
      file.write(",");
      file.print(humidityBme280);
      file.write(",");
      file.print(pressureBme280);
      file.write(",");
      file.print(temperature);
      file.write(",");
      file.print(windSpeed);
      file.write(",");
      file.print(windDirection);
      file.write(",");
      file.print(freeRam());
      file.write(",");
      file.println(samplesSaved);

      writeTimestamps();
      file.close();

      Serial.print(unixtime);
      Serial.print(",");
      Serial.print(voltage);
      Serial.print(",");
      Serial.print(temperatureDs3231);
      Serial.print(",");
      Serial.print(temperatureBme280);
      Serial.print(",");
      Serial.print(humidityBme280);
      Serial.print(",");
      Serial.print(pressureBme280);
      Serial.print(",");
      Serial.print(temperature);
      Serial.print(",");
      Serial.print(windSpeed);
      Serial.print(",");
      Serial.print(windDirection);
      Serial.print(",");
      Serial.print(freeRam());
      Serial.print(",");
      Serial.println(samplesSaved);
      blink(LED_PIN, 2, 100);
    }
    else
    {
      Serial.println("Unable to open file");
    }

    // Force data to SD and update the directory entry to avoid data loss
    if (!file.sync() || file.getWriteError())
    {
      Serial.println("Write error");
      delay(10000);
    }
  }
}

// Log file write and access timestamps
void writeTimestamps()
{
  myRTC.read(tm); // Read current date and time from the RTC

  // Set the file's last write/modification date and time
  if (!file.timestamp(T_WRITE, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
    Serial.println("Set write time failed");

  // Set the file's last access date and time
  if (!file.timestamp(T_ACCESS, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second))
    Serial.println("Set access time failed");
}

// Calculate statistics and clear objects
void calculateStatistics()
{
  message.voltage = batteryStats.minimum() * 1000;                // Minimum battery voltage (mV)
  message.temperatureExt = temperatureExtStats.average() * 100;   // Mean thermistor temperature (°C)
  message.temperatureInt = temperatureIntStats.average() * 100;   // Mean BME280 temperature (°C)
  message.humidity = humidityStats.average() * 100;               // Mean BME280 humidity (%)
  message.pressure = pressureStats.average();               // Mean BME280 pressure (Pa)

  batteryStats.clear();
  temperatureExtStats.clear();
  temperatureIntStats.clear();
  humidityStats.clear();
  pressureStats.clear();
  windSpeedStats.clear();
  veStats.clear();
  vnStats.clear();
}

void writeBuffer()
{
  iterationCounter++;
  message.iterationCounter = iterationCounter;

  // Concatenate current message with existing message(s) stored in transmit buffer
  memcpy(transmitBuffer + (sizeof(message) * (messageCounter + (retransmitCounter * transmitInterval) - 1)), message.bytes, sizeof(message)); // Copy message to transmit buffer

  // Print union/structure
  printUnion();
  printUnionBinary();
  printTransmitBuffer();

  memset(message.bytes, 0x00, sizeof(message));   // Clear data stored in union
}

// Transmit data via the RockBLOCK 9603 satellite modem
void transmitData()
{
  uint16_t err;
  uint32_t loopStartTime = millis()
  digitalWrite(ROCKBLOCK_EN_PIN, HIGH);

  // Start the serial power connected to the RockBLOCK modem
  IridiumSerial.begin(19200);

  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

  // Start the RockBLOCK modem
  err = modem.begin();
  if (err == ISBD_SUCCESS)
  {
    uint8_t inBuffer[10];   // Buffer to store incomming RockBLOCK transmission (340 byte limit per message)
    size_t inBufferSize = sizeof(inBuffer);
    memset(inBuffer, 0x00, sizeof(inBuffer));   // Clear  inBuffer array

#if DEBUG
    // Iridium modem diagnostics
    int signalQuality = -1;
    err = modem.getSignalQuality(signalQuality);
    if (err != ISBD_SUCCESS)
    {
      Serial.print("SignalQuality failed: error ");
      Serial.println(err);
      return;
    }
    Serial.print("Signal quality: ");
    Serial.println(signalQuality);
#endif

    // Transmit and receieve data in binary format
    err = modem.sendReceiveSBDBinary(transmitBuffer, (sizeof(message) * (messageCounter + (retransmitCounter * transmitInterval))), inBuffer, inBufferSize);

    if (err == ISBD_SUCCESS)
    {
      messageCounter = 0;
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
        uint32_t sampleIntervalBuffer = (((uint32_t)inBuffer[3] << 0) & 0xFF) + (((uint32_t)inBuffer[2] << 8) & 0xFFFF) + (((uint32_t)inBuffer[1] << 16) & 0xFFFFFF) + (((uint32_t)inBuffer[0] << 24) & 0xFFFFFFFF);
        uint16_t averageIntervalBuffer  = (((uint16_t)inBuffer[5] << 0) & 0xFF) + (((uint16_t)inBuffer[4] << 8) & 0xFFFF);
        uint16_t transmitIntervalBuffer  = (((uint16_t)inBuffer[7] << 0) & 0xFF) + (((uint16_t)inBuffer[6] << 8) & 0xFFFF);
        uint16_t maxRetransmitCounterBuffer  = (((uint16_t)inBuffer[9] << 0) & 0xFF) + (((uint16_t)inBuffer[8] << 8) & 0xFFFF);

        // Check validity of incoming data
        if ((sampleIntervalBuffer > 0  && sampleIntervalBuffer <= 86400) && (averageIntervalBuffer > 0  && averageIntervalBuffer <= 24) && (transmitIntervalBuffer > 0  && transmitIntervalBuffer <= 10) && (maxRetransmitCounterBuffer > 0  && maxRetransmitCounterBuffer <= 10))
        {
          sampleInterval = sampleIntervalBuffer;
          averageInterval = averageIntervalBuffer;
          transmitInterval = transmitIntervalBuffer;
          maxRetransmitCounter = maxRetransmitCounterBuffer;
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
    Serial.print("Begin failed: error ");
    Serial.println(err);
  }

  // If transmission or modem begin fail, stored message in transmit buffer
  if (err != ISBD_SUCCESS)
  {
    retransmitCounter++;  // Increment retransmit counter

    // Reset retransmit counter if maximum messages reattempts is exceeded
    if (retransmitCounter >= maxRetransmitCounter)
    {
      messageCounter = 0;
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
  digitalWrite(ROCKBLOCK_EN_PIN, LOW);

#if DEBUG
  Serial.print(F("transmitDuration: ")); Serial.println(transmitDuration / 1000);
  Serial.print(F("retransmitCounter: ")); Serial.println(retransmitCounter);
  Serial.print(F("sampleInterval: ")); Serial.println(sampleInterval);
  Serial.print(F("averageInterval: ")); Serial.println(averageInterval);
  Serial.print(F("transmitInterval: ")); Serial.println(transmitInterval);
  Serial.print(F("maxRetransmitCounter: ")); Serial.println(maxRetransmitCounter);
#endif
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
void ISBDConsoleCallback(IridiumSBD *device, char c)
{
#if DEBUG
  Serial.write(c);
#endif
}

// Callback to to monitor library's run state
void ISBDDiagsCallback(IridiumSBD *device, char c)
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
  Serial.print(F("Temperature Ext\t"));
  Serial.print(F("Samples: ")); Serial.print(temperatureExtStats.count());
  Serial.print(F("\tMin: ")); Serial.print(temperatureExtStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(temperatureExtStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(temperatureExtStats.average());
  Serial.print(F("Temperature Int\t"));
  Serial.print(F("Samples: ")); Serial.print(temperatureIntStats.count());
  Serial.print(F("\tMin: ")); Serial.print(temperatureIntStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(temperatureIntStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(temperatureIntStats.average());
  Serial.print(F("Humidity\t"));
  Serial.print(F("Samples: ")); Serial.print(humidityStats.count());
  Serial.print(F("\tMin: ")); Serial.print(humidityStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(humidityStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(humidityStats.average());
  Serial.print(F("Pressure\t"));
  Serial.print(F("Samples: ")); Serial.print(pressureStats.count());
  Serial.print(F("\tMin: ")); Serial.print(pressureStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(pressureStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(pressureStats.average());
  Serial.print(F("Wind speed\t"));
  Serial.print(F("Samples: ")); Serial.print(windSpeedStats.count());
  Serial.print(F("\tMin: ")); Serial.print(windSpeedStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(windSpeedStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(windSpeedStats.average());
  Serial.print(F("vn\t\t"));
  Serial.print(F("Samples: ")); Serial.print(vnStats.count());
  Serial.print(F("\tMin: ")); Serial.print(vnStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(vnStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(vnStats.average());
  Serial.print(F("ve\t\t"));
  Serial.print(F("Samples: ")); Serial.print(veStats.count());
  Serial.print(F("\tMin: ")); Serial.print(veStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(veStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(veStats.average());
}

// Print union/structure
void printUnion()
{
  Serial.println();
  Serial.println(F("Union/structure"));
  Serial.println(F("==================================="));
  Serial.print(F("unixtime:\t\t")); Serial.println(message.unixtime);
  Serial.print(F("temperatureExt:\t\t")); Serial.println(message.temperatureExt);
  Serial.print(F("temperatureInt:\t\t")); Serial.println(message.temperatureInt);
  Serial.print(F("humidity:\t\t")); Serial.println(message.humidity);
  Serial.print(F("pressure:\t\t")); Serial.println(message.pressure);
  Serial.print(F("windSpeed:\t\t")); Serial.println(message.windSpeed);
  Serial.print(F("windDirection:\t\t")); Serial.println(message.windDirection);
  Serial.print(F("windGust:\t\t")); Serial.println(message.windGust);
  Serial.print(F("windGustDirection:\t")); Serial.println(message.windGustDirection);
  Serial.print(F("voltage:\t\t")); Serial.println(message.voltage);
  Serial.print(F("transmitDuration:\t")); Serial.println(message.transmitDuration);
  Serial.print(F("iterationCounter:\t")); Serial.println(message.iterationCounter);

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
    while (true);                                   // Force Watchdog Timer to reset the system
  }
}
