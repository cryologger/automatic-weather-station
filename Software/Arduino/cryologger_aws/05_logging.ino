// Configure microSD
void configureSd()
{
#if LOGGING
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if microSD has been initialized
  if (!online.microSd)
  {
    // Initialize microSD
    if (!sd.begin(PIN_MICROSD_CS, SD_SCK_MHZ(4)))
    {
      DEBUG_PRINTLN("Warning - microSD failed to initialize. Reattempting...");

      // Delay between initialization attempts
      myDelay(2000);

      if (!sd.begin(PIN_MICROSD_CS, SD_SCK_MHZ(4)))
      {
        online.microSd = false;
        DEBUG_PRINTLN("Warning - microSD failed to initialize.");

        /*
          while (1)
          {
          // Force WDT to reset system
          blinkLed(PIN_LED_RED, 2, 250);
          delay(2000);
          }
        */
      }
      else
      {
        online.microSd = true; // Set flag
        DEBUG_PRINTLN("Info - microSD initialized.");
      }
    }
    else
    {
      online.microSd = true; // Set flag
      DEBUG_PRINTLN("Info - microSD initialized.");
    }
  }
  else
  {
    DEBUG_PRINTLN("Info - microSD already initialized.");
    return;
  }

  // Stop the loop timer
  timer.configMicroSd = millis() - loopStartTime;
#endif
}

// Create timestamped log file
void createLogFile()
{
#if LOGGING

  // Get timestamp log file name
  sprintf(logFileName, "AWS_%s_20%02d%02d%02d_%02d%02d%02d.csv",
          CRYOLOGGER_ID, rtc.getYear(), rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());

  DEBUG_PRINT("Info - Log file name: "); DEBUG_PRINTLN(logFileName);

  // Check if log file is open
  if (logFile.isOpen())
    logFile.close();

  // Create a new log file and open for writing
  // O_CREAT  - Create the file if it does not exist
  // O_APPEND - Seek to the end of the file prior to each write
  // O_WRITE  - Open the file for writing
  if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE))
  {
    DEBUG_PRINT("Warning - Failed to create log file"); DEBUG_PRINTLN(logFileName);
    return;
  }
  else
  {
    DEBUG_PRINT("Info - Created log file: "); DEBUG_PRINTLN(logFileName);
  }

  if (!logFile.isOpen())
  {
    DEBUG_PRINTLN(F("Unable to open file"));
  }

  // Update file create timestamp
  updateFileCreate(&logFile);

  // Write header to file
  logFile.println("sample,datetime,voltage,temperature_int,humidity_int,pressure_int,temperature_ext,"
                  "humidity_ext,pitch,roll,wind_speed,wind_direction,solar,snow_avg,snow_std,snow_max,"
                  "snow_min,snow_nan,latitude,longitude,satellites,hdop,"
                  "online_microsd,online_bme280,online_lsm303,timer_rtc,timer_battery,timer_microSd,"
                  "timer_gnss,timer_bme280,timer_lsm303,timer_sht31,timer_7911,timer_sp212,timer_mb7354,"
                  "timer_iridium,transmit_status,rtc_drift,free_ram,"
                  "sampleInterval,averageInterval,transmitInterval,retransmitLimit,gnssTimeout,iridiumTimeout");

  // Unused: timer_hmp60,timer_5103l


  // Close log file
  logFile.close();
#endif
}

void checkLogFile()
{
  // Record log file tracker the first time program runs
  if (firstTimeFlag)
  {
    if (loggingMode == 1) // Daily
      currentLogFile = rtc.getDay();
    else if (loggingMode == 2) // Monthly
      currentLogFile = rtc.getMonth();
    else if (loggingMode == 3) // Yearly
      currentLogFile = rtc.getYear();
    else // Default to monthly
      currentLogFile = rtc.getMonth();
  }

  // Update log file tracker
  if (loggingMode == 1) // Daily
    newLogFile = rtc.getDay();
  else if (loggingMode == 2) // Monthly
    newLogFile = rtc.getMonth();
  else if (loggingMode == 3) // Yearly
    newLogFile = rtc.getYear();
  else // Default to monthly
    newLogFile = rtc.getMonth();
}

// Write data to log file
void logData()
{
#if LOGGING
  // Configure microSD
  configureSd();

  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if microSD is online
  if (online.microSd)
  {
    // Check if new log file should be created
    checkLogFile();
    if (currentLogFile != newLogFile)
    {
      createLogFile();
      currentLogFile = newLogFile;
      samplesSaved = 0;
    }

    // Write to microSD card
    if (logFile.open(logFileName, O_APPEND | O_WRITE))
    {
      // Sensor information
      samplesSaved++; //  Increment sample count
      logFile.print(samplesSaved);        logFile.print(",");
      logFile.print(dateTime);            logFile.print(",");
      logFile.print(voltage);             logFile.print(",");
      logFile.print(temperatureInt);      logFile.print(",");
      logFile.print(humidityInt);         logFile.print(",");
      logFile.print(pressureInt);         logFile.print(",");
      logFile.print(temperatureExt);      logFile.print(",");
      logFile.print(humidityExt);         logFile.print(",");
      logFile.print(pitch);               logFile.print(",");
      logFile.print(roll);                logFile.print(",");
      logFile.print(windSpeed);           logFile.print(",");
      logFile.print(windDirection);       logFile.print(",");
      logFile.print(solar);               logFile.print(",");
      logFile.print(snowDepthAvg);        logFile.print(",");
      logFile.print(snowDepthStd);        logFile.print(",");
      logFile.print(snowDepthMax);        logFile.print(",");
      logFile.print(snowDepthMin);        logFile.print(",");
      logFile.print(snowDepthNan);        logFile.print(",");
      logFile.print(latitude, 6);         logFile.print(",");
      logFile.print(longitude, 6);        logFile.print(",");
      logFile.print(satellites);          logFile.print(",");
      logFile.print(hdop);                logFile.print(",");

      // Online information
      logFile.print(online.microSd);      logFile.print(",");
      logFile.print(online.bme280);       logFile.print(",");
      logFile.print(online.lsm303);       logFile.print(",");

      // Timer information
      logFile.print(timer.readRtc);       logFile.print(",");
      logFile.print(timer.readBattery);   logFile.print(",");
      logFile.print(timer.configMicroSd); logFile.print(",");
      logFile.print(timer.readGnss);      logFile.print(",");
      logFile.print(timer.readBme280);    logFile.print(",");
      logFile.print(timer.readLsm303);    logFile.print(",");
      //logFile.print(timer.readHmp60);     logFile.print(",");
      //logFile.print(timer.read5103L);     logFile.print(",");
      logFile.print(timer.readSht31);     logFile.print(",");
      logFile.print(timer.read7911);      logFile.print(",");
      logFile.print(timer.readSp212);     logFile.print(",");
      logFile.print(timer.readMb7354);     logFile.print(",");
      logFile.print(timer.iridium);       logFile.print(",");

      // Debugging information
      logFile.print(transmitStatus);      logFile.print(",");
      logFile.print(rtcDrift);            logFile.print(",");
      logFile.print(freeRam());           logFile.print(",");

      // Sampling information
      logFile.print(sampleInterval);      logFile.print(",");
      logFile.print(averageInterval);     logFile.print(",");
      logFile.print(transmitInterval);    logFile.print(",");
      logFile.print(retransmitLimit);     logFile.print(",");
      logFile.print(gnssTimeout);         logFile.print(",");
      logFile.println(iridiumTimeout);

      // Update file access timestamps
      updateFileAccess(&logFile);

      // Force data to SD and update the directory entry to avoid data loss
      if (!logFile.sync())
      {
        DEBUG_PRINTLN(F("Warning - microSD sync error!"));
      }

      // Close the log file
      if (!logFile.close())
      {
        DEBUG_PRINTLN("Warning - Failed to close log file!");
        //closeFailCounter++; // Count number of failed file closes
      }

#if DEBUG
      // Print logged data to Serial Monitor

      DEBUG_PRINT("Info - Logging data to: "); DEBUG_PRINTLN(logFileName);
      DEBUG_PRINT(samplesSaved);        DEBUG_PRINT(",");
      DEBUG_PRINT(dateTime);            DEBUG_PRINT(",");
      DEBUG_PRINT(voltage);             DEBUG_PRINT(",");
      DEBUG_PRINT(temperatureInt);      DEBUG_PRINT(",");
      DEBUG_PRINT(humidityInt);         DEBUG_PRINT(",");
      DEBUG_PRINT(pressureInt);         DEBUG_PRINT(",");
      DEBUG_PRINT(temperatureExt);      DEBUG_PRINT(",");
      DEBUG_PRINT(humidityExt);         DEBUG_PRINT(",");
      DEBUG_PRINT(pitch);               DEBUG_PRINT(",");
      DEBUG_PRINT(roll);                DEBUG_PRINT(",");
      DEBUG_PRINT(windSpeed);           DEBUG_PRINT(",");
      DEBUG_PRINT(windDirection);       DEBUG_PRINT(",");
      DEBUG_PRINT(solar);               DEBUG_PRINT(",");
      DEBUG_PRINT(snowDepthAvg);        DEBUG_PRINT(",");
      DEBUG_PRINT(snowDepthStd);        DEBUG_PRINT(",");
      DEBUG_PRINT(snowDepthMax);        DEBUG_PRINT(",");
      DEBUG_PRINT(snowDepthMin);        DEBUG_PRINT(",");
      DEBUG_PRINT(snowDepthNan);        DEBUG_PRINT(",");
      DEBUG_PRINT_DEC(latitude, 6);     DEBUG_PRINT(",");
      DEBUG_PRINT_DEC(longitude, 6);    DEBUG_PRINT(",");
      DEBUG_PRINT(satellites);          DEBUG_PRINT(",");
      DEBUG_PRINT(hdop);                DEBUG_PRINT(",");

      // Online information
      DEBUG_PRINT(online.microSd);      DEBUG_PRINT(",");
      DEBUG_PRINT(online.bme280);       DEBUG_PRINT(",");
      DEBUG_PRINT(online.lsm303);       DEBUG_PRINT(",");

      // Timer information
      DEBUG_PRINT(timer.readRtc);       DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readBattery);   DEBUG_PRINT(",");
      DEBUG_PRINT(timer.configMicroSd); DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readGnss);      DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readBme280);    DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readLsm303);    DEBUG_PRINT(",");
      //DEBUG_PRINT(timer.readHmp60);     DEBUG_PRINT(",");
      //DEBUG_PRINT(timer.read5103L);     DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readSht31);     DEBUG_PRINT(",");
      DEBUG_PRINT(timer.read7911);      DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readSp212);     DEBUG_PRINT(",");
      DEBUG_PRINT(timer.iridium);       DEBUG_PRINT(",");

      // Debugging information
      DEBUG_PRINT(transmitStatus);      DEBUG_PRINT(",");
      DEBUG_PRINT(rtcDrift);            DEBUG_PRINT(",");
      DEBUG_PRINT(freeRam());           DEBUG_PRINT(",");

      // Sampling information
      DEBUG_PRINT(sampleInterval);      DEBUG_PRINT(",");
      DEBUG_PRINT(averageInterval);     DEBUG_PRINT(",");
      DEBUG_PRINT(transmitInterval);    DEBUG_PRINT(",");
      DEBUG_PRINT(retransmitLimit);     DEBUG_PRINT(",");
      DEBUG_PRINT(gnssTimeout);         DEBUG_PRINT(",");
      DEBUG_PRINTLN(iridiumTimeout);
#endif
      blinkLed(PIN_LED_GREEN, 2, 100);
    }
    else
    {
      DEBUG_PRINTLN(F("Warning - Unable to open file!"));
    }
  }
  else
  {
    DEBUG_PRINTLN(F("Warning - microSD is offline!"));
  }

  // Stop the loop timer
  timer.writeMicroSd = millis() - loopStartTime;
#endif
}


// Update the file create timestamp
void updateFileCreate(FsFile *dataFile)
{
  if (!dataFile->timestamp(T_CREATE, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINT("Warning - Could not update file create timestamp.");
  }
}

// Update the file access and write timestamps
void updateFileAccess(FsFile *dataFile)
{
  if (!dataFile->timestamp(T_ACCESS, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINTLN("Warning - Could not update file access timestamp.");
  }
  if (!dataFile->timestamp(T_WRITE, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINTLN("Warning - Could not update file write timestamp.");
  }
}
