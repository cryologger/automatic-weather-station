// Calculate statistics
void calculateStats()
{
  // Write data to union
  moSbdMessage.temperatureExt = extTemperatureStats.average() * 100;  // Mean temperature (°C)
  moSbdMessage.humidityExt = extHumidityStats.average() * 100;        // Mean humidity (%)
  moSbdMessage.temperatureInt = intTemperatureStats.average() * 100;  // Mean temperature (°C)
  moSbdMessage.voltage = batteryStats.minimum() * 100;               // Mean battery voltage (V)


  windVectors();  // Calculate mean wind speed and direction vectors

  clearStats(); // Clear all statistics objects

}


// Clear all statistics objects
void clearStats()
{
  // Clear statistics objects
  batteryStats.clear();
  intTemperatureStats.clear();
  intHumidityStats.clear();
  intPressureStats.clear();
  extTemperatureStats.clear();
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
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(intTemperatureStats.count());     printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(intTemperatureStats.minimum());   printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(intTemperatureStats.maximum());   printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(intTemperatureStats.average());
  DEBUG_PRINT(F("Temp Ext"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(extTemperatureStats.count());     printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(extTemperatureStats.minimum());   printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(extTemperatureStats.maximum());   printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(extTemperatureStats.average());
  DEBUG_PRINT(F("Humidity"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));    DEBUG_PRINT(extHumidityStats.count());        printTab(1);
  DEBUG_PRINT(F("Min: "));        DEBUG_PRINT(extHumidityStats.minimum());      printTab(1);
  DEBUG_PRINT(F("Max: "));        DEBUG_PRINT(extHumidityStats.maximum());      printTab(1);
  DEBUG_PRINT(F("Mean: "));       DEBUG_PRINTLN(extHumidityStats.average());
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
}
