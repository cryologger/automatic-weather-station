void configureSd()
{
  // Initialize the SD card
  if (sd.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("SD card initialized.");
    logFlag = true;
    createLogFile(); // Create new log file
    blinkLed(LED_PIN, 20, 100);
  }
  else {
    Serial.println("Warning: Unable to initialize SD card!");
    logFlag = false;  // Disable data logging
    blinkLed(LED_BUILTIN, 20, 100);
  }

}
// Create log file
void createLogFile() 
{
  // Check if logging is enabled
  if (logFlag == true) 
  {
    if (file.isOpen())
      file.close();

    // Select a unique log file name
    for (unsigned int i = 0; i < 999; i++) 
    {
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

    // Read RTC date and time
    myRTC.read(tm);

    // Set the file's creation date and time
    if (!file.timestamp(T_CREATE, (tm.Year + 1970), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second)) 
    {
      Serial.println("Set create time failed");
    }

    // Write header to file
    file.println("SAMPLE,UNIXTIME,VBAT,TI,TA,RH,WS1,WD1,WG1,WGD1,");

    // Close log file
    file.close();

    Serial.println(F("New log file created: "));
    Serial.println(fileName);
  }
}

// Write data to log file
void logData() 
{

  // Check if logging is enabled
  if (logFlag == true) 
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
      file.print(",");
      file.print(unixtime);
      file.print(",");
      file.print(voltage);
      file.print(",");
      file.print(intTemperature);
      file.print(",");
      file.print(extTemperature);
      file.print(",");
      file.print(humidity);
      file.print(",");
      file.print(windSpeed1);
      file.print(",");
      file.print(windDirection1);
      file.print(",");
      file.print(windGust1);
      file.print(",");
      file.print(windGustDirection1);
      writeTimestamps();
      file.close();

#if DEBUG
      Serial.print(samplesSaved);
      Serial.print(",");
      Serial.print(unixtime);
      Serial.print(",");
      Serial.print(voltage);
      Serial.print(",");
      Serial.print(intTemperature);
      Serial.print(",");
      Serial.print(extTemperature);
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
      blinkLed(LED_PIN, 2, 100);
    }
    else {
      Serial.println(F("Unable to open file"));
    }
#endif

    // Force data to SD and update the directory entry to avoid data loss
    if (!file.sync() || file.getWriteError()) 
    {
      Serial.println(F("Write error"));
    }
  }
}

// Log file write and access timestamps
void writeTimestamps() 
{
  // Read RTC date and time
  myRTC.read(tm);

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
