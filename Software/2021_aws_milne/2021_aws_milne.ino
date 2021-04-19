/*
  Title:    Cryologger Automatic Weather Station (AWS)
  Date:     April 19, 2021
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
#include <Arduino.h>            // Must be included before wiring_private.h
#include <ArduinoLowPower.h>    // https://github.com/arduino-libraries/ArduinoLowPower
#include <DS3232RTC.h>          // https://github.com/JChristensen/DS3232RTC
#include <IridiumSBD.h>         // https://github.com/mikalhart/IridiumSBD
#include <math.h>               // https://www.nongnu.org/avr-libc/user-manual/group__avr__math.html
#include <sensirion.h>          // https://github.com/HydroSense/sensirion
#include <Statistic.h>          // https://github.com/RobTillaart/Statistic
#include <SdFat.h>              // https://github.com/greiman/SdFat
#include <TinyGPS++.h>          // https://github.com/mikalhart/TinyGPSPlus
#include <SPI.h>                // https://www.arduino.cc/en/Reference/SPI
#include <Wire.h>               // https://www.arduino.cc/en/Reference/Wire
#include <wiring_private.h>     // Required for creating new Serial instance with pinPeripheral() function 

// ------------------------------------------------------------------------------------------------
// Pin definitions
// ------------------------------------------------------------------------------------------------
#define RTC_INT_PIN             5
#define LED_PIN                 8
#define ROCKBLOCK_RX_PIN        10
#define ROCKBLOCK_TX_PIN        11
#define ROCKBLOCK_SLEEP_PIN     12
#define WIND_SPEED_1_PIN        A0
#define WIND_DIRECTION_1_PIN    A1
#define GPIO_PWR_1_PIN          A2    // Anemometer GPIO power pin
#define VBAT_PIN                A7

// Debugging constants
#define DEBUG         true    // Output debug messages to Serial Monitor
#define DIAGNOSTICS   true    // Output Iridium diagnostic messages to Serial Monitor
#define DEPLOY        false   // Disable debugging messages for deployment

// ------------------------------------------------------------------------------------------------
// Port configuration and definitions
// ------------------------------------------------------------------------------------------------
// Create a new UART instance assigning it to pin 10 and 11.
// For more information see: https://www.arduino.cc/en/Tutorial/SamdSercom
Uart Serial2 (&sercom1, ROCKBLOCK_RX_PIN, ROCKBLOCK_TX_PIN, SERCOM_RX_PAD_2, UART_TX_PAD_0);
#define IridiumSerial Serial2

// Attach the interrupt handler to the SERCOM
void SERCOM1_Handler() 
{
  Serial2.IrqHandler();
}

// ------------------------------------------------------------------------------------------------
// Object instantiations
// ------------------------------------------------------------------------------------------------
DS3232RTC       myRTC(false);             // Tell constructor not to initialize the I2C bus
IridiumSBD      modem(IridiumSerial, ROCKBLOCK_SLEEP_PIN);
SdFat           sd;                       // File system object
SdFile          file;                     // Log file
sensirion       sht(20, 21);              // sht(data pin, clock pin);
time_t          t;
unsigned long   unixtime;
time_t          alarmTime;
tmElements_t    tm;

// Statistic objects
Statistic batteryStats;         // Battery voltage statistics
Statistic humidityStats;        // Humidity statistics
Statistic rtcStats;             // Real-time clock statistics
Statistic extTemperatureStats;  // Extermal temperature statistics
Statistic windSpeedStats1;      // Anemometer wind speed statistics
Statistic vnStats1;             // Anemometer north-south wind vector component (v) statistics
Statistic veStats1;             // Anemometer east-west wind vector component (u) statistics

// ------------------------------------------------------------------------------------------------
// User defined global variable declarations
// ------------------------------------------------------------------------------------------------
unsigned int          sampleInterval        = 300;          // Sleep duration (in seconds) between data sample acquisitions. Default = 5 minutes (300 seconds)
unsigned int          averageInterval       = 12;           // Number of samples to be averaged for each RockBLOCK transmission. Default = 12 (Hourly)
unsigned int          transmitInterval      = 1;            // Number of message to be included in a single transmission (340 byte limit). Default = 3 (Every 3 hours)
unsigned int          maxRetransmitCounter  = 10;           // Maximum failed data transmissions to reattempt in a single message (340 byte limit). Default: 10
unsigned int          samplesPerFile        = 8640;         // Maximum samples stored in a file before new log file creation (Default: 30 days * 288 samples per day)

// ------------------------------------------------------------------------------------------------
// Global variable declarations
// ------------------------------------------------------------------------------------------------
const byte            chipSelect            = 4;            // MicroSD chip select pin
const byte            samplesToAverage      = 10;           // Number of samples to average
volatile bool         alarmFlag             = false;        // RTC interrupt service routine (ISR) flag
volatile bool         sleepFlag             = false;        // Watchdog Timer Early Warning interrupt flag
volatile byte         watchdogCounter       = 0;            // Watchdog Timer trigger counter
volatile unsigned int revolutions1          = 0;            // Wind speed 1 ISR revolutions counter
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
float                 windSpeed1            = 0.0;          // Wind speed 1 in metres per second (m/s)
float                 windGust1             = 0.0;          // Wind gust speed 1 in metres per second (m/s)
float                 windGustDirection1    = 0.0;          // Wind gust direction 1 in degrees (°)
unsigned int          windDirection1        = 0;            // Wind direction 1 in degrees (°)
unsigned int          samplePeriod          = 3;            // Wind speed sample period in seconds (s)
unsigned int          retransmitCounter     = 0;            // RockBLOCK failed data transmission counter
unsigned int          messageCounter        = 0;            // RockBLOCK transmission counter
unsigned int          transmitCounter       = 0;            // RockBLOCK transmission interval counter
unsigned int          previousMillis        = 0;            // RockBLOCK callback function timer variable
unsigned int          transmitDuration      = 0;            // RockBLOCK data transmission time variable
unsigned int          sampleCounter         = 0;            // Sensor measurement counter
unsigned int          samplesSaved          = 0;            // Log file sample counter

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
    uint16_t    windSpeed1;           // Resultant mean wind speed 1 (m/s)        (2 bytes) (windSpeed * 100)
    uint16_t    windDirection1;       // Resultant mean wind direction 1 (°)      (2 bytes)
    uint16_t    windGust1;            // Wind gust speed 1 (m/s)                  (2 bytes) (windGust * 100)
    uint16_t    windGustDirection1;   // Wind gust direction 1 (°)                (2 bytes)
    uint16_t    voltage;              // Minimum battery voltage (mV)             (2 bytes) (voltage * 1000)
    uint16_t    transmitDuration;     // Debugging variable                       (2 bytes)
    uint16_t    messageCounter;       // RockBLOCK data transmission counter      (2 bytes)
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
  pinMode(GPIO_PWR_1_PIN, OUTPUT);    // Anemometer GPIO power pin
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WIND_SPEED_1_PIN, INPUT);
  digitalWrite(GPIO_PWR_1_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  // Start Serial at 115200 baud
  Serial.begin(115200);
  delay(5000); // Delay to allow user to open Serial Monitor

  Serial.println(F("--------------------------------------"));
  Serial.println(F("Cryologger - Automatic Weather Station"));
  Serial.println(F("--------------------------------------"));

  // I2C Configuration
  Wire.begin();           // Initialize I2C bus
  Wire.setClock(100000);  // Set I2C clock speed to 100kHz

  // Analog-to-digital converter (ADC) Configuration
  analogReadResolution(12); // Change the ADC resolution to 12 bits

  // Watchdog Timer Configuration
  configureWatchdog();
  configureSd();
  configureIridium();
  configureRtc(); // Configure RTC

  // Blink LED to indicate setup has completed
  blinkLed(LED_PIN, 10, 100);
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
      // Read the date, time and temperature from the RTC
      readRtc();

      // Increment the sample counter
      sampleCounter++;

      // Pet the Watchdog Timer
      petDog();

      // Print date and time
      Serial.print(F("Alarm trigger: ")); printDateTime(t);

      // Perform measurements
      readBattery();    // Read battery voltage
      readTrh();        // Read temperature and humidity
      readAnemometer(); // Read wind speed and direction
      logData();        // Log data to microSD card

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
  sleepFlag = true; // Clear sleep flag

#if DEBUG
  blinkLed(LED_BUILTIN, 1, 500);
#endif

#if DEPLOY
  blinkLed(LED_BUILTIN, 1, 10);
  LowPower.deepSleep(); // Enter deep sleep
#endif
}
