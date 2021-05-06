/*
  Title:    Cryologger Automatic Weather Station (AWS)
  Date:     May 6, 2021
  Author:   Adam Garbo

  Components:
  - Adafruit Feather M0 Adalogger
  - Adafruit DS3231 RTC Precision Featherwing
  - Davis Instruments 7911 Anemomenter
  - Davis Instruments 6830 Temperature/Relative Humidity
  - Rock Seven RockBLOCK 9603
  - 256 MB microSD card (Industrial rated)

  Comments:
  - Automatic weather station to be deployed in the summer of 2021 on Ellesmere Island.
*/

// ------------------------------------------------------------------------------------------------
// Libraries
// ------------------------------------------------------------------------------------------------
#include <Arduino.h>                // Must be included before wiring_private.h
#include <ArduinoLowPower.h>        // https://github.com/arduino-libraries/ArduinoLowPower
#include <DS3232RTC.h>              // https://github.com/JChristensen/DS3232RTC
#include <IridiumSBD.h>             // https://github.com/mikalhart/IridiumSBD
#include <math.h>                   // https://www.nongnu.org/avr-libc/user-manual/group__avr__math.html
#include <SAMD_AnalogCorrection.h>  // https://github.com/arduino/ArduinoCore-samd/tree/master/libraries/SAMD_AnalogCorrection
#include <sensirion.h>              // https://github.com/HydroSense/sensirion
#include <Statistic.h>              // https://github.com/RobTillaart/Statistic
#include <SdFat.h>                  // https://github.com/greiman/SdFat
#include <TinyGPS++.h>              // https://github.com/mikalhart/TinyGPSPlus
#include <SPI.h>                    // https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>                   // https://www.arduino.cc/en/Reference/Wire
#include <wiring_private.h>         // Required for creating new Serial instance with pinPeripheral() function 

// ------------------------------------------------------------------------------------------------
// Pin definitions
// ------------------------------------------------------------------------------------------------
#define PIN_RTC_INT           5
#define PIN_LED               8
#define PIN_IRIDIUM_EN        9
#define PIN_IRIDIUM_RX        10
#define PIN_IRIDIUM_TX        11
#define PIN_IRIDIUM_SLEEP     12
#define PIN_WIND_SPEED        A0
#define PIN_WIND_DIRECTION    A1
#define PIN_WIND_PWR          A2
#define PIN_TRH_PWR           A4
#define PIN_GPS_EN            A5
#define PIN_VBAT              A7

// Debugging constants
#define DEBUG         true    // Output debug messages to Serial Monitor
#define DEBUG_GPS     true    // Output GPS debug information
#define DEBUG_IRIDIUM true    // Output Iridium debug messages to Serial Monitor
#define DEPLOY        false   // Disable debugging messages for deployment

// ------------------------------------------------------------------------------------------------
// Port configuration
// ------------------------------------------------------------------------------------------------
// Create a new UART instance and assign it to pins 10 (RX) and 11 (TX).
// For more information see: https://www.arduino.cc/en/Tutorial/SamdSercom
Uart Serial2 (&sercom1, PIN_IRIDIUM_RX, PIN_IRIDIUM_TX, SERCOM_RX_PAD_2, UART_TX_PAD_0);

#define GPS_PORT      Serial1
#define IRIDIUM_PORT  Serial2

// Attach the interrupt handler to the SERCOM
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

// ------------------------------------------------------------------------------------------------
// Object instantiations
// ------------------------------------------------------------------------------------------------
DS3232RTC       myRTC(false);             // Tell constructor not to initialize the I2C bus
IridiumSBD      modem(IRIDIUM_PORT, PIN_IRIDIUM_SLEEP);
SdFat           sd;                       // File system object
SdFile          file;                     // Log file
sensirion       sht(20, 21);              // sht(data pin, clock pin);

// ------------------------------------------------------------------------------------------------
// Statistic objects
// ------------------------------------------------------------------------------------------------
Statistic batteryStats;         // Battery voltage statistics
Statistic humidityStats;        // Humidity statistics
Statistic rtcStats;             // Real-time clock statistics
Statistic extTemperatureStats;  // External temperature statistics
Statistic windSpeedStats;       // Anemometer wind speed statistics
Statistic vnStats;              // Anemometer north-south wind vector component (v) statistics
Statistic veStats;              // Anemometer east-west wind vector component (u) statistics

// ------------------------------------------------------------------------------------------------
// User defined global variable declarations
// ------------------------------------------------------------------------------------------------
unsigned int          sampleInterval        = 60;          // Sleep duration (in seconds) between data sample acquisitions. Default = 5 minutes (300 seconds)
unsigned int          averageInterval       = 3;           // Number of samples to be averaged for each RockBLOCK transmission. Default = 12 (Hourly)
unsigned int          transmitInterval      = 100;          // Number of message to be included in a single transmission (340 byte limit). Default = 3 (Every 3 hours)
unsigned int          retransmitLimit       = 10;           // Maximum failed data transmissions to reattempt in a single message (340 byte limit). Default: 10
unsigned int          samplesPerFile        = 8640;         // Maximum samples stored in a file before new log file creation (Default: 30 days * 288 samples per day)

// ------------------------------------------------------------------------------------------------
// Global variable declarations
// ------------------------------------------------------------------------------------------------
const byte            chipSelect            = 4;            // MicroSD chip select pin
const byte            samplesToAverage      = 10;           // Number of samples to average
volatile bool         alarmFlag             = false;        // RTC interrupt service routine (ISR) flag
volatile bool         wdtFlag               = false;        // Watchdog Timer Early Warning interrupt flag
volatile byte         wdtCounter            = 0;            // Watchdog Timer trigger counter
volatile unsigned int revolutions           = 0;            // Wind speed 1 ISR revolutions counter
volatile unsigned int contactBounceTime     = 0;            // Anemometer debouncing variable
bool                  ledState              = LOW;          // Flag to toggle LED in blinkLed() function
bool                  logFlag               = true;         // MicroSD initilization flag
byte                  resetFlag             = 0;            // Watchdog Timer force reset flag
byte                  transmitBuffer[340]   = {};           // RockBLOCK transmission buffer
char                  fileName[12]          = "log000.csv"; // Log file naming convention
float                 humidity              = 0.0;          // SHT31 humidity (%)
float                 extTemperature        = 0.0;          // SHT31 temperature (°C)
float                 intTemperature        = 0.0;          // Internal RTC temperature (°C)
float                 voltage               = 0.0;          // Battery voltage in volts (V)
float                 windSpeed             = 0.0;          // Wind speed in metres per second (m/s)
float                 windGust              = 0.0;          // Wind gust speed in metres per second (m/s)
float                 windGustDirection     = 0.0;          // Wind gust direction in degrees (°)
unsigned int          windDirection         = 0;            // Wind direction in degrees (°)
unsigned int          samplePeriod          = 3;            // Wind speed sample period in seconds (s)
unsigned int          iterationCounter      = 0;            // Counter to track total number of program iterations (zero indicates a reset)
unsigned int          retransmitCounter     = 0;            // RockBLOCK failed data transmission counter
unsigned int          transmitCounter       = 0;            // RockBLOCK transmission interval counter
unsigned int          transmitDuration      = 0;            // RockBLOCK data transmission time variable
unsigned long         previousMillis        = 0;            // Global millis() timer
unsigned int          sampleCounter         = 0;            // Sensor measurement counter
unsigned int          samplesSaved          = 0;            // Log file sample counter
unsigned long         unixtime;
time_t                t, alarmTime;
tmElements_t          tm;

// ------------------------------------------------------------------------------------------------
// Data transmission unions/structures
// ------------------------------------------------------------------------------------------------
// Union to transmit Iridium Short Burst Data (SBD) Mobile Originated (MO) message
typedef union
{
  struct
  {
    uint32_t    unixtime;             // Date and time in time_t format           (4 bytes)
    int16_t     extTemperature;       // Mean external temperature (°C)           (2 bytes) (extTemperature * 100)
    int16_t     intTemperature;       // Mean internal temperature (°C)           (2 bytes)
    uint16_t    humidity;             // Mean humidity (%)                        (2 bytes) (humidity * 100)
    uint16_t    windSpeed;           // Resultant mean wind speed 1 (m/s)        (2 bytes) (windSpeed * 100)
    uint16_t    windDirection;       // Resultant mean wind direction 1 (°)      (2 bytes)
    uint16_t    windGust;            // Wind gust speed 1 (m/s)                  (2 bytes) (windGust * 100)
    uint16_t    windGustDirection;   // Wind gust direction 1 (°)                (2 bytes)
    uint16_t    voltage;              // Minimum battery voltage (mV)             (2 bytes) (voltage * 1000)
    uint16_t    transmitDuration;     // Debugging variable                       (2 bytes)
    uint16_t    iterationCounter;       // RockBLOCK data transmission counter      (2 bytes)
  } __attribute__((packed));                                                      // Total = 24 bytes
  uint8_t bytes[24];
} SBDMESSAGE;

SBDMESSAGE message;
size_t messageSize = sizeof(message); // Size (in bytes) of data to be stored and transmitted

// ------------------------------------------------------------------------------------------------
// Setup
// ------------------------------------------------------------------------------------------------
void setup()
{
  // Pin assignments
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRH_PWR, OUTPUT);
  pinMode(PIN_WIND_PWR, OUTPUT);
  pinMode(PIN_WIND_SPEED, INPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_LED, LOW);
  digitalWrite(PIN_TRH_PWR, LOW);
  digitalWrite(PIN_WIND_PWR, LOW);

  // Start Serial at 115200 baud
  Serial.begin(115200);
  delay(4000); // Delay to allow user to open Serial Monitor

  printLine();
  Serial.println(F("Cryologger - Automatic Weather Station"));
  printLine();

  // I2C Configuration
  Wire.begin();           // Initialize I2C bus
  Wire.setClock(100000);  // Set I2C clock speed to 100kHz

  // Analog-to-digital converter (ADC) Configuration
  analogReadResolution(12); // Change the ADC resolution to 12 bits

  // Configure devices
  configureWatchdog();  // Configure Watchdog Timer (WDT)
  //configureSd();        // Configure microSD
  configureIridium();   // Configure Iridium 9603 transceiver
  printSettings();      // Print configuration settings
  configureRtc();       // Configure real-time clock (RTC)

  // Print operating mode
  Serial.print(F("Mode: "));
#if DEBUG
  Serial.println(F("DEBUG"));
#else if DEPLOY
  Serial.println(F("DEPLOY"));
#endif

  // Blink LED to indicate setup has completed
  blinkLed(LED_BUILTIN, 5, 100);
}

// ------------------------------------------------------------------------------------------------
// Loop
// ------------------------------------------------------------------------------------------------
void loop()
{
  // Check if alarm interrupt service routine was triggered
  if (alarmFlag)
  {
    // Check to see if the alarm flag is set (also resets the flag if set)
    if (myRTC.alarm(ALARM_1))
    {
      // Read date, time and temperature from the RTC
      readRtc();

      // Increment the sample counter
      sampleCounter++;

      // Pet the Watchdog Timer
      petDog();

      // Print date and time
      Serial.print(F("Alarm trigger: ")); printDateTime(t);

      // Perform measurements
      readBattery();    // Read battery voltage
      //readTrh();        // Read temperature and humidity
      readAnemometer(); // Read wind speed and direction
      //logData();        // Log data to microSD card

      // Perform statistics on measurements
      if (sampleCounter == averageInterval)
      {
        transmitCounter++; // Increment transmit counter
        windVectors();
        printStatistics();
        calculateStatistics();
        writeBuffer(); // Write message to transmit buffer array

        // Transmit data
        if (transmitCounter == transmitInterval)
        {
          transmitData();
          transmitCounter = 0;
        }
        sampleCounter = 0; // Reset sample counter
      }

      // Set the RTC alarm
      alarmTime = t + sampleInterval;   // Calculate next alarm

      // Check if alarm was set in the past
      if (alarmTime <= myRTC.get())
      {
        Serial.print(F("Warning! Alarm set in the past!"));
        t = myRTC.get(); // Read current date and time
        alarmTime = t + sampleInterval; // Calculate next alarm
        myRTC.setAlarm(ALM1_MATCH_MINUTES, 0, 0, 0, 1); // Set alarm to occur at minutes rolloever
      }
      else
      {
        myRTC.setAlarm(ALM1_MATCH_DATE, 0, minute(alarmTime), hour(alarmTime), day(alarmTime)); // Set alarm
      }
      myRTC.alarm(ALARM_1); // Ensure alarm 1 interrupt flag is cleared

      Serial.print(F("Next alarm: ")); printDateTime(alarmTime);

    }
    alarmFlag = false; // Clear alarm interrupt service routine flag
  }

  // Check for Watchdog Timer interrupts
  if (wdtFlag)
  {
    petDog(); // Reset the Watchdog Timer
  }

  // Blink LED to indicate WDT interrupt and nominal system operation
  blinkLed(LED_BUILTIN, 1, 100);

  // Enter deep sleep and wait for WDT or RTC alarm interrupt
  goToSleep();
}
