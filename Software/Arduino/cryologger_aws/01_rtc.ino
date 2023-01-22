// Configure the real-time clock (RTC)
void configureRtc()
{
  // Alarm modes:
  // 0: MATCH_OFF          Never
  // 1: MATCH_SS           Every Minute
  // 2: MATCH_MMSS         Every Hour
  // 3: MATCH_HHMMSS       Every Day
  // 4: MATCH_DHHMMSS      Every Month
  // 5: MATCH_MMDDHHMMSS   Every Year
  // 6: MATCH_YYMMDDHHMMSS Once, on a specific date and a specific time

  // Initialize RTC
  rtc.begin();

  // Set time manually
  //rtc.setTime(hours, minutes, seconds);
  //rtc.setDate(day, month, year);
  //rtc.setEpoch();

  //rtc.setTime(11, 04, 30); // Must be in the form: rtc.setTime(11, 04, 30);
  
  // Set initial RTC alarm time
  rtc.setAlarmTime(0, 30, 0); // hours, minutes, seconds

  // Enable alarm for hour rollover match
  rtc.enableAlarm(rtc.MATCH_MMSS);
  //rtc.enableAlarm(rtc.MATCH_SS);

  // Attach alarm interrupt service routine (ISR)
  rtc.attachInterrupt(alarmIsr);

  alarmFlag = false; // Clear flag

  DEBUG_PRINT("Info: RTC initialized "); printDateTime();
  DEBUG_PRINT("Info: Initial alarm "); printAlarm();
  DEBUG_PRINT("Info: Alarm match "); DEBUG_PRINTLN(rtc.MATCH_MMSS);
}

// Read RTC
void readRtc()
{
  // Start the loop timer
  uint32_t loopStartTime = millis();

  // Get Unix Epoch time
  unixtime = rtc.getEpoch();

  sprintf(dateTime, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.getYear(), rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());

  // Write data to union
  moSbdMessage.unixtime = unixtime;

  // Stop the loop timer
  timer.readRtc = millis() - loopStartTime;
}

// Set RTC alarm
void setRtcAlarm()
{
  // Calculate next alarm
  alarmTime = unixtime + (sampleInterval * 60UL);
  DEBUG_PRINT(F("Info: unixtime ")); DEBUG_PRINTLN(unixtime);
  DEBUG_PRINT(F("Info: alarmTime ")); DEBUG_PRINTLN(alarmTime);

  // Check if alarm was set in the past or too far in the future
  if ((rtc.getEpoch() >= alarmTime) || ((alarmTime - unixtime) > 86400) || firstTimeFlag)
  {
    DEBUG_PRINTLN(F("Warning: RTC alarm set in the past, too far in the future, or program running for the first time."));

    // Set alarm for hour rollover match
    rtc.setAlarmTime(0, sampleInterval, 0); // hours, minutes, seconds

    // Enable alarm for hour rollover match
    //rtc.enableAlarm(rtc.MATCH_MMSS);
    rtc.enableAlarm(rtc.MATCH_SS);

    // Reset sample counter
    sampleCounter = 0;

    // Clear all statistics objects
    clearStats();

    DEBUG_PRINT("Info: "); printDateTime();
    DEBUG_PRINT("Info: Next alarm "); printAlarm();
    DEBUG_PRINT("Info: Alarm match "); DEBUG_PRINTLN(rtc.MATCH_MMSS);
  }
  else
  {
    DEBUG_PRINTLN(F("Info: Setting RTC alarm based on specified interval."));

    // Set alarm time
    rtc.setAlarmTime(hour(alarmTime), minute(alarmTime), 0); // hours, minutes, seconds

    // Set alarm date
    rtc.setAlarmDate(day(alarmTime), month(alarmTime), year(alarmTime) - 2000);

    // Enable alarm
    rtc.enableAlarm(rtc.MATCH_HHMMSS);

    DEBUG_PRINT("Info: "); printDateTime();
    DEBUG_PRINT("Info: Next alarm "); printAlarm();
    DEBUG_PRINT("Info: Alarm match "); DEBUG_PRINTLN(rtc.MATCH_HHMMSS);
  }

  // Clear flag
  alarmFlag = false;
}

void setCutoffAlarm()
{
  // Set alarm for hour rollover match
  rtc.setAlarmTime(0, 5, 0); // hours, minutes, seconds

  // Enable alarm for hour rollover match
  rtc.enableAlarm(rtc.MATCH_MMSS);

  // Clear flag
  alarmFlag = false;

  DEBUG_PRINT("Info: "); printDateTime();
  DEBUG_PRINT("Info: Next alarm "); printAlarm();
  DEBUG_PRINT("Info: Alarm match "); DEBUG_PRINTLN(rtc.MATCH_HHMMSS);
}

// RTC alarm interrupt service routine (ISR)
void alarmIsr()
{
  alarmFlag = true;
}

// Print the RTC's current date and time
void printDateTime()
{
  char dateTimeBuffer[25];
  sprintf(dateTimeBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.getYear(), rtc.getMonth(), rtc.getDay(),
          rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  DEBUG_PRINTLN(dateTimeBuffer);
}

// Print the RTC alarm
void printAlarm()
{
  char alarmBuffer[25];
  sprintf(alarmBuffer, "20%02d-%02d-%02d %02d:%02d:%02d",
          rtc.getAlarmYear(), rtc.getAlarmMonth(), rtc.getAlarmDay(),
          rtc.getAlarmHours(), rtc.getAlarmMinutes(), rtc.getAlarmSeconds());
  DEBUG_PRINTLN(alarmBuffer);
}
