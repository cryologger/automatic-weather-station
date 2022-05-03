// Calculate statistics
void calculateStats()
{
  // Write data to union
  moSbdMessage.temperatureExt = temperatureExtStats.average() * 100;  // Mean temperature (°C)
  moSbdMessage.humidityExt = humidityExtStats.average() * 100;        // Mean humidity (%)
  moSbdMessage.temperatureInt = temperatureIntStats.average() * 100;  // Mean temperature (°C)
  moSbdMessage.voltage = batteryStats.minimum() * 100;                // Mean battery voltage (V)

  // Calculate mean wind speed and direction vectors
  windVectors();

  // Clear all statistics objects
  clearStats();
}

// Clear statistics objects
void clearStats()
{
  batteryStats.clear();
  temperatureExtStats.clear();
  humidityExtStats.clear();
  temperatureIntStats.clear();
  windSpeedStats.clear();
  veStats.clear();
  vnStats.clear();
}

// Print statistics
void printStats()
{
  DEBUG_PRINTLN();
  printLine();
  DEBUG_PRINTLN(F("Statistics"));
  printLine();
  DEBUG_PRINT(F("Voltage"));      printTab(2);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(batteryStats.count());            printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(batteryStats.minimum());          printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(batteryStats.maximum());          printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(batteryStats.average());
  DEBUG_PRINT(F("Temp Int"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(temperatureIntStats.count());     printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(temperatureIntStats.minimum());   printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(temperatureIntStats.maximum());   printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(temperatureIntStats.average());
  DEBUG_PRINT(F("Temp Ext"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(temperatureExtStats.count());     printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(temperatureExtStats.minimum());   printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(temperatureExtStats.maximum());   printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(temperatureExtStats.average());
  DEBUG_PRINT(F("Humidity"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(humidityExtStats.count());        printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(humidityExtStats.minimum());      printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(humidityExtStats.maximum());      printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(humidityExtStats.average());
  DEBUG_PRINT(F("Wind speed"));   printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(windSpeedStats.count());          printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(windSpeedStats.minimum());        printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(windSpeedStats.maximum());        printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(windSpeedStats.average());
  DEBUG_PRINT(F("vn"));           printTab(2);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(vnStats.count());                 printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(vnStats.minimum());               printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(vnStats.maximum());               printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(vnStats.average());
  DEBUG_PRINT(F("ve"));           printTab(2);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(veStats.count());                 printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(veStats.minimum());               printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(veStats.maximum());               printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(veStats.average());
  DEBUG_PRINT(F("Wind gust speed: "));      printTab(1);  DEBUG_PRINTLN(windGustSpeed);
  DEBUG_PRINT(F("Wind gust direction: "));  printTab(1);  DEBUG_PRINTLN(windGustDirection);
}
