// Configure real-time clock (RTC)
void configureRtc()
{
  // Real-time clock (RTC) Configuration
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
    tm.Hour = 14;
    tm.Minute = 0;
    tm.Second = 0;
    tm.Day = 5;
    tm.Month = 5;
    tm.Year = 2021 - 1970;    // tmElements_t.Year is the offset from 1970
    time_t t = makeTime(tm);  // change the tm structure into time_t (seconds since epoch)
    myRTC.set(t);
  */

  // Set RTC alarm
#if DEBUG
  myRTC.setAlarm(ALM1_MATCH_SECONDS, 0, 0, 0, 1);     // Set initial alarm to occur at seconds rollover
#else if DEPLOY
  myRTC.setAlarm(ALM1_MATCH_MINUTES, 0, 0, 0, 1);     // Set initial alarm to occur at minutes rollover (start of new hour)
#endif
  myRTC.alarm(ALARM_1);                               // Ensure alarm 1 interrupt flag is cleared
  myRTC.alarmInterrupt(ALARM_1, true);                // Enable interrupt output for alarm 1

  // Print current date and time
  Serial.print(F("Datetime: ")); printDateTime(myRTC.get());

  // Print RTC's alarm date and time
  Serial.print(F("Next alarm: ")); printDateTime(alarmTime);
}

// RTC interrupt service routine (ISR)
void alarmIsr()
{
  alarmFlag = true;
}

// Measure internal temperature from DS3231 RTC
void readRtc()
{
  myRTC.read(tm);     // Read current date and time
  t = makeTime(tm);   // Change the tm structure into time_t (seconds since epoch)
  unixtime = t;       // Alarm 1 trigger time
  intTemperature = myRTC.temperature() / 4.0; // Internal DS3231 temperature

  // Add to statistics object
  rtcStats.add(intTemperature);

  // Write data to union
  message.unixtime = unixtime;
}

// Print the RTC's current date and time
void printDateTime(time_t t)
{
  char dateTimeBuffer[25];
  sprintf(dateTimeBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
          year(t), month(t), day(t),
          hour(t), minute(t), second(t));
  Serial.println(dateTimeBuffer);
}
