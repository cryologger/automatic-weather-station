// Calculate statistics
void calculateStats()
{
  // Write data to union
  moSbdMessage.voltage = batteryStats.minimum() * 1000;               // Mean battery voltage (mV)
  moSbdMessage.extTemperature = extTemperatureStats.average() * 100;  // Mean temperature (°C)
  moSbdMessage.extHumidity = extHumidityStats.average() * 100;        // Mean humidity (%)
  moSbdMessage.intTemperature = intTemperatureStats.average() * 100;  // Mean temperature (°C)

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
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(batteryStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(batteryStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(batteryStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(batteryStats.average());
  DEBUG_PRINT(F("Temp Int"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(intTemperatureStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(intTemperatureStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(intTemperatureStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(intTemperatureStats.average());
  DEBUG_PRINT(F("Temp Ext"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(extTemperatureStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(extTemperatureStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(extTemperatureStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(extTemperatureStats.average());
  DEBUG_PRINT(F("Humidity"));     printTab(1);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(extHumidityStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(extHumidityStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(extHumidityStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(extHumidityStats.average());
  DEBUG_PRINT(F("Wind speed"));   printTab(1);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(windSpeedStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(windSpeedStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(windSpeedStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(windSpeedStats.average());
  DEBUG_PRINT(F("vn"));           printTab(2);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(vnStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(vnStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(vnStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(vnStats.average());
  DEBUG_PRINT(F("ve"));           printTab(2);
  DEBUG_PRINT(F("Samples: "));                  DEBUG_PRINT(veStats.count());
  DEBUG_PRINT(F("Min: "));        printTab(1);  DEBUG_PRINT(veStats.minimum());
  DEBUG_PRINT(F("Max: "));        printTab(1);  DEBUG_PRINT(veStats.maximum());
  DEBUG_PRINT(F("Mean: "));       printTab(1);  DEBUG_PRINTLN(veStats.average());
}
