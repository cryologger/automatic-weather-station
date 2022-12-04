// Configure microSD
void configureSd()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if microSD has been initialized
  if (!online.microSd)
  {
    // Initialize microSD
    if (!sd.begin(PIN_MICROSD_CS, SD_SCK_MHZ(4)))
    {
      DEBUG_PRINTLN("Warning: microSD failed to initialize. Reattempting...");

      // Delay between initialization attempts
      myDelay(2000);

      if (!sd.begin(PIN_MICROSD_CS, SD_SCK_MHZ(4)))
      {
        DEBUG_PRINTLN("Warning: microSD failed to initialize.");
        online.microSd = false;

        while (1)
        {
          // Force WDT to reset system
          blinkLed(LED_BUILTIN, 2, 250);
          delay(2000);
        }
      }
      else
      {
        online.microSd = true; // Set flag
        DEBUG_PRINTLN("Info: microSD initialized.");
      }
    }
    else
    {
      online.microSd = true; // Set flag
      DEBUG_PRINTLN("Info: microSD initialized.");
    }
  }
  else
  {
    DEBUG_PRINTLN("Info: microSD already initialized.");
    return;
  }

  // Stop the loop timer
  timer.configMicroSd = millis() - loopStartTime;
}

// Create timestamped log file name
void getLogFileName()
{
  // Get logfile name
  sprintf(logFileName, "AWS_%d_20%02d%02d%02d_%02d%02d%02d.csv",
          CRYOLOGGER_ID, rtc.getYear(), rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());

  DEBUG_PRINT("Info: Log file name: "); DEBUG_PRINTLN(logFileName);
}

// Create log file
void createLogFile()
{
  configureSd();

  // Check if logging is enabled
  if (logFlag)
  {
    if (logFile.isOpen())
      logFile.close();

    // Create a new log file and open for writing
    // O_CREAT  - Create the file if it does not exist
    // O_APPEND - Seek to the end of the file prior to each write
    // O_WRITE  - Open the file for writing
    if (!logFile.open(logFileName, O_CREAT | O_APPEND | O_WRITE))
    {
      DEBUG_PRINT("Warning: Failed to create log file"); DEBUG_PRINTLN(logFileName);
      return;
    }
    else
    {
      DEBUG_PRINT("Info: Created log file "); DEBUG_PRINTLN(logFileName);
    }

    if (!logFile.isOpen())
    {
      DEBUG_PRINTLN(F("Unable to open file"));
    }

    // Read RTC date and time

    // Update file create timestamp
    updateFileCreate(&logFile);

    // Write header to file
    logFile.println("sample,datetime,voltage,temperature_int,humidity_int,pressure_int,temperature_ext,"
                    "humidity_ext,wind_speed,wind_direction,latitude,longitude,satellites,hdop,"
                    "online_bme280,online_lsm303,timer_battery,timer_gnss,timer_bme280,timer_lsm303,"
                    "timer_hmp60,timer_5103l,rtc_drift,free_ram");

    // Close log file
    logFile.close();
  }
}

// Write data to log file
void logData()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Check if logging is enabled
  if (logFlag)
  {
    // Check that maximum file sample limit has not been exceeded
    if (samplesSaved >= samplesPerFile)
    {
      createLogFile();
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
      //logFile.print(solar);               logFile.print(",");
      logFile.print(windSpeed);           logFile.print(",");
      logFile.print(windDirection);       logFile.print(",");
      logFile.print(latitude, 6);          logFile.print(",");
      logFile.print(longitude, 6);         logFile.print(",");
      logFile.print(satellites);          logFile.print(",");
      logFile.print(hdop);                logFile.print(",");
      // Debugging information (select configuration)
      logFile.print(online.bme280);       logFile.print(",");
      logFile.print(online.lsm303);       logFile.print(",");
      logFile.print(timer.battery);       logFile.print(",");
      logFile.print(timer.gnss);          logFile.print(",");
      logFile.print(timer.readBme280);    logFile.print(",");
      logFile.print(timer.readLsm303);    logFile.print(",");
      //logFile.print(timer.readSp212);     logFile.print(",");
      logFile.print(timer.readHmp60);     logFile.print(",");
      logFile.print(timer.read5103L);     logFile.print(",");
      logFile.print(rtcDrift);            logFile.print(",");
      logFile.println(freeRam());

      // Update file access timestamps
      updateFileAccess(&logFile);

      // Force data to SD and update the directory entry to avoid data loss
      if (!logFile.sync())
      {
        DEBUG_PRINTLN(F("Warning: microSD sync error"));
      }

      // Close the log file
      if (!logFile.close())
      {
        DEBUG_PRINTLN("Warning: Failed to close log file!");
        //closeFailCounter++; // Count number of failed file closes
      }

#if DEBUG
      // Print logged data to Serial Monitor
      DEBUG_PRINT(samplesSaved);        DEBUG_PRINT(",");
      DEBUG_PRINT(dateTime);            DEBUG_PRINT(",");
      DEBUG_PRINT(voltage);             DEBUG_PRINT(",");
      DEBUG_PRINT(temperatureInt);      DEBUG_PRINT(",");
      DEBUG_PRINT(humidityInt);         DEBUG_PRINT(",");
      DEBUG_PRINT(pressureInt);         DEBUG_PRINT(",");
      DEBUG_PRINT(temperatureExt);      DEBUG_PRINT(",");
      DEBUG_PRINT(humidityExt);         DEBUG_PRINT(",");
      //DEBUG_PRINT(solar);               DEBUG_PRINT(",");
      DEBUG_PRINT(windSpeed);           DEBUG_PRINT(",");
      DEBUG_PRINT(windDirection);       DEBUG_PRINT(",");
      DEBUG_PRINT_DEC(latitude, 6);     DEBUG_PRINT(",");
      DEBUG_PRINT_DEC(longitude, 6);    DEBUG_PRINT(",");
      DEBUG_PRINT(satellites);          DEBUG_PRINT(",");
      DEBUG_PRINT(hdop);                DEBUG_PRINT(",");
      // Debugging information (select configuration)
      DEBUG_PRINT(online.bme280);       DEBUG_PRINT(",");
      DEBUG_PRINT(online.lsm303);       DEBUG_PRINT(",");
      DEBUG_PRINT(timer.battery);       DEBUG_PRINT(",");
      DEBUG_PRINT(timer.gnss);          DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readBme280);    DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readLsm303);    DEBUG_PRINT(",");
      //DEBUG_PRINT(timer.readSp212);     DEBUG_PRINT(",");
      DEBUG_PRINT(timer.readHmp60);     DEBUG_PRINT(",");
      DEBUG_PRINT(timer.read5103L);     DEBUG_PRINT(",");
      DEBUG_PRINT(rtcDrift);            DEBUG_PRINT(",");
      DEBUG_PRINTLN(freeRam());
#endif
      blinkLed(PIN_LED, 2, 100);
    }
    else
    {
      DEBUG_PRINTLN(F("Warning: Unable to open file!"));
    }
  }

  // Stop the loop timer
  timer.writeMicroSd = millis() - loopStartTime;
}


// Update the file create timestamp
void updateFileCreate(FsFile *dataFile)
{
  // Update the file create timestamp
  if (!dataFile->timestamp(T_CREATE, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINT("Warning: Could not update file create timestamp.");
  }
}

// Update the file access and write timestamps
void updateFileAccess(FsFile *dataFile)
{
  // Update the file access and write timestamps
  if (!dataFile->timestamp(T_ACCESS, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINTLN("Warning: Could not update file access timestamp.");
  }
  if (!dataFile->timestamp(T_WRITE, (rtc.getYear() + 2000), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds()))
  {
    DEBUG_PRINTLN("Warning: Could not update file write timestamp.");
  }
}
