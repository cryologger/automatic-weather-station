// Configure and read GNSS
void syncRtc()
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

  DEBUG_PRINTLN("Info: Beginning to listen for GNSS traffic...");

  GNSS_PORT.begin(9600);
  myDelay(1000);

  // Configure GNSS
  GNSS_PORT.println("$PMTK220,1000*1F"); // Set NMEA update rate to 1 Hz
  myDelay(100);
  GNSS_PORT.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"); // Set NMEA sentence output to GGA and RMC
  myDelay(100);
  //GNSS_PORT.println("$CDCMD,33,1*7C"); // Enable antenna updates
  //GNSS_PORT.println("$CDCMD,33,0*7D"); // Disable antenna updates

  // Attempt to acquire a valid GNSS position fix for up to specified timeout period
  while (!fixFound && millis() - loopStartTime < gnssTimeout * 10UL * 1000UL)
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
        if ((gnss.location.isValid() && gnss.date.isValid() && gnss.time.isValid()) &&
            (gnss.location.isUpdated() && gnss.date.isUpdated() && gnss.time.isUpdated()) &&
            (gnss.satellites.value() > 0))
        {
          DEBUG_PRINT(F(" Pass"));
          fixCounter++; // Increment fix counter

          // Wait until a specified number of GNSS fixes have been collected
          if ((fixCounter >= 5) && (gnss.satellites.value() > 0))
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
            DEBUG_PRINT(F("gnssEpoch: ")); DEBUG_PRINTLN(gnssEpoch);
            DEBUG_PRINT(F("rtcEpoch: ")); DEBUG_PRINTLN(rtcEpoch);
            DEBUG_PRINT(F("unixtime: ")); DEBUG_PRINTLN(unixtime);

            // Sync RTC with GNSS date and time only if gnssEpoch is in the future
            if (((gnssEpoch > 1630368000) && (gnssEpoch > unixtime)) || firstTimeFlag)
            {
              rtc.setEpoch(gnssEpoch);
              DEBUG_PRINT(F("Info: RTC synced ")); printDateTime();
            }
            else
            {
              DEBUG_PRINT(F("Warning: RTC not synced! GNSS time in the past! ")); printDateTime();
            }

            // Write data to buffer
            moSbdMessage.latitude = gnss.location.lat() * 1000000;
            moSbdMessage.longitude = gnss.location.lng() * 1000000;
            moSbdMessage.satellites = gnss.satellites.value();
            moSbdMessage.hdop = gnss.hdop.value();

            DEBUG_PRINT(F("Info: RTC drift ")); DEBUG_PRINT(rtcDrift); DEBUG_PRINTLN(F(" seconds"));
            blinkLed(5, 250);
          }
        }
        else
        {
          DEBUG_PRINT(F(" Fail"));
        }
      }
    }

    // Call callback during acquisition of GNSS fix
    ISBDCallback();

    // Exit function if no GNSS data is received after 5 seconds
    if ((millis() - loopStartTime) > 5000 && gnss.charsProcessed() < 10)
    {
      DEBUG_PRINTLN(F("Warning: No GNSS data received. Please check wiring."));
      break;
    }
  }

  if (!fixFound)
  {
    DEBUG_PRINTLN(F("Warning: No GNSS fix found!"));
  }

  // Close GNSS port
  GNSS_PORT.end();

  // Disable power to GNSS
  disableGnssPower();

  // Stop the loop timer
  timer.gnss = millis() - loopStartTime;
}
