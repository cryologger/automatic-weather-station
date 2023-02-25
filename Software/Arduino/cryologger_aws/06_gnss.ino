// Read GNSS
void readGnss()
{
  // Start loop timer
  unsigned long loopStartTime = millis();

  // Clear flags
  bool fixFound = false;
  bool charsSeen = false;

  // Reset GNSS fix counter
  byte fixCounter = 0;

  // Enable power to GNSS
  enableGnssPower();

  // Open serial port at 9600 baud
  GNSS_PORT.begin(9600);
  DEBUG_PRINTLN("Info - Beginning to listen for GNSS traffic...");
  myDelay(1000);

  // Configure GNSS
  // Note: a delay of at least 1 s is required after powering on GNSS module
  GNSS_PORT.println("$PMTK220,1000*1F"); // Set NMEA update rate to 1 Hz
  GNSS_PORT.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"); // Set NMEA sentence output frequencies to GGA and RMC
  //GNSS_PORT.println("$PGCMD,33,1*6C"); // Enable antenna updates
  //GNSS_PORT.println("$PGCMD,33,0*6D"); // Disable antenna updates

  // Look for GNSS signal for up to gnssTimeout
  while (!fixFound && millis() - loopStartTime < gnssTimeout * 1000UL)
  {
    if (GNSS_PORT.available())
    {
      charsSeen = true;
      char c = GNSS_PORT.read();
#if DEBUG_GNSS
      Serial.write(c); // Echo NMEA sentences to serial
#endif
      if (gnss.encode(c))
      {
        // Check if NMEA sentences have a valid fix and are not stale
        if ((gnssFix.value() > 0 && gnssFix.age() < 1000) &&
            (String(gnssValidity.value()) == "A" && gnssValidity.age() < 1000) &&
            gnss.satellites.value() > 0)
        {
          fixCounter++; // Increment fix counter

          // Wait until a specified number of GNSS fixes have been collected
          if (fixCounter >= 10)
          {
            fixFound = true;

            // Convert GNSS date and time to epoch time
            tm.Hour = gnss.time.hour();
            tm.Minute = gnss.time.minute();
            tm.Second = gnss.time.second();
            tm.Day = gnss.date.day();
            tm.Month = gnss.date.month();
            tm.Year = gnss.date.year() - 1970; // Offset from 1970
            unsigned long gnssEpoch = makeTime(tm); // Change the tm structure into time_t (seconds since epoch)

            // Get RTC epoch time
            unsigned long rtcEpoch = rtc.getEpoch();

            // Calculate RTC drift
            long rtcDrift = rtcEpoch - gnssEpoch;

            DEBUG_PRINTLN("");
            DEBUG_PRINT(F("Info - gnssEpoch: ")); DEBUG_PRINTLN(gnssEpoch);
            DEBUG_PRINT(F("Info - rtcEpoch: ")); DEBUG_PRINTLN(rtcEpoch);

            // Sync RTC with GNSS only if gnssEpoch is greater than current unixtime
            if ((gnssEpoch > unixtime) || firstTimeFlag)
            {
              rtc.setEpoch(gnssEpoch);

              DEBUG_PRINT(F("Info - RTC synced to: ")); printDateTime();
            }
            else
            {
              DEBUG_PRINT(F("Warning - RTC sync failed. GNSS time not accurate! ")); printDateTime();
            }

            // Record position information
            latitude = gnss.location.lat();
            longitude = gnss.location.lng();
            satellites = gnss.satellites.value();
            hdop = gnss.hdop.value();

            // Write data to buffer
            //moSbdMessage.latitude = gnss.location.lat() * 1000000;
            //moSbdMessage.longitude = gnss.location.lng() * 1000000;
            //moSbdMessage.satellites = gnss.satellites.value();
            //moSbdMessage.hdop = gnss.hdop.value();

            DEBUG_PRINT(F("Info - RTC drift ")); DEBUG_PRINT(rtcDrift); DEBUG_PRINTLN(F(" seconds"));
            blinkLed(PIN_LED_GREEN, 5, 100);
          }
        }
        else
        {
          //DEBUG_PRINT(F(" Fail"));
        }
      }
    }

    // Call callback during acquisition of GNSS fix
    ISBDCallback();

    // Exit function if no GNSS data is received after a specified duration
    if ((millis() - loopStartTime) > 5000 && gnss.charsProcessed() < 10)
    {
      DEBUG_PRINTLN(F("Warning - No GNSS data received. Please check wiring."));
      break;
    }
  }

  if (!fixFound)
  {
    DEBUG_PRINTLN(F("Warning - No GNSS fix found!"));
    blinkLed(PIN_LED_RED, 5, 100);
  }

  // Close GNSS port
  GNSS_PORT.end();

  // Disable power to GNSS
  disableGnssPower();

  // Stop the loop timer
  timer.readGnss = millis() - loopStartTime;
}
