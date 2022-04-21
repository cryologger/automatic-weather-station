void printLine()
{
  for (byte i = 0; i < 80; i++)
  {
    Serial.print("-");
  }
  Serial.println();
}

void printTab(byte _times)
{
  for (byte i = 0; i < _times; i++)
  {
    Serial.print("\t");
  }
}

// Print user-defined beacon settings
void printSettings()
{
  printLine();
  Serial.println("Current Settings");
  printLine();
  Serial.print("sampleInterval: ");   printTab(1);  Serial.println(sampleInterval);
  Serial.print("averageInterval: ");  printTab(1);  Serial.println(averageInterval);
  Serial.print("transmitInterval: "); printTab(1);  Serial.println(transmitInterval);
  Serial.print("retransmitLimit: ");  printTab(1);  Serial.println(retransmitLimit);
  Serial.print("samplesPerFile: ");   printTab(1);  Serial.println(samplesPerFile);
  printLine();
}

// Print statistics
void printStatistics()
{
  Serial.println();
  printLine();
  Serial.println(F("Statistics"));
  printLine();
  Serial.print(F("Voltage\t\t"));
  Serial.print(F("Samples: ")); Serial.print(batteryStats.count());
  Serial.print(F("\tMin: "));   Serial.print(batteryStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(batteryStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(batteryStats.average());
  Serial.print(F("Temp Int\t"));
  Serial.print(F("Samples: ")); Serial.print(rtcStats.count());
  Serial.print(F("\tMin: ")); Serial.print(rtcStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(rtcStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(rtcStats.average());
  Serial.print(F("Temp Ext\t"));
  Serial.print(F("Samples: ")); Serial.print(extTemperatureStats.count());
  Serial.print(F("\tMin: ")); Serial.print(extTemperatureStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(extTemperatureStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(extTemperatureStats.average());
  Serial.print(F("Humidity\t"));
  Serial.print(F("Samples: ")); Serial.print(humidityStats.count());
  Serial.print(F("\tMin: ")); Serial.print(humidityStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(humidityStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(humidityStats.average());
  Serial.print(F("Wind speed 1\t"));
  Serial.print(F("Samples: ")); Serial.print(windSpeedStats.count());
  Serial.print(F("\tMin: ")); Serial.print(windSpeedStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(windSpeedStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(windSpeedStats.average());
  Serial.print(F("vn 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(vnStats.count());
  Serial.print(F("\tMin: ")); Serial.print(vnStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(vnStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(vnStats.average());
  Serial.print(F("ve 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(veStats.count());
  Serial.print(F("\tMin: ")); Serial.print(veStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(veStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(veStats.average());
}

// Print union/structure
void printUnion()
{
  Serial.println();
  printLine();
  Serial.println(F("Union/structure"));
  printLine();
  Serial.print(F("unixtime:"));             printTab(2); Serial.println(message.unixtime);
  Serial.print(F("intTemperature:"));       printTab(2); Serial.println(message.intTemperature);
  Serial.print(F("extTemperature:"));       printTab(2); Serial.println(message.extTemperature);
  Serial.print(F("humidity:"));             printTab(2); Serial.println(message.humidity);
  Serial.print(F("windSpeed:"));            printTab(2); Serial.println(message.windSpeed);
  Serial.print(F("windDirection:"));        printTab(1); Serial.println(message.windDirection);
  Serial.print(F("windGust:"));             printTab(2); Serial.println(message.windGust);
  Serial.print(F("windGustDirection:"));    printTab(1); Serial.println(message.windGustDirection);
  Serial.print(F("voltage:"));              printTab(2); Serial.println(message.voltage);
  Serial.print(F("transmitDuration:"));     printTab(1); Serial.println(message.transmitDuration);
  Serial.print(F("iterationCounter:"));     printTab(2); Serial.println(message.iterationCounter);
  printLine();
}

// Print contents of union/structure
void printUnionBinary()
{
  Serial.println();
  Serial.println(F("Union/structure"));
  printLine();
  Serial.println(F("Byte\tHex\tBinary"));
  for (unsigned int i = 0; i < sizeof(message); ++i) {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(message.bytes[i], HEX);
    Serial.print("\t");
    Serial.println(message.bytes[i], BIN);
  }
}

// Print contents of transmiff buffer array
void printTransmitBuffer()
{
  Serial.println();
  Serial.println(F("Transmit buffer"));
  printLine();
  Serial.println(F("Byte\tHex\tBinary"));
  for (unsigned int i = 0; i < 340; i++)
  {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(transmitBuffer[i], HEX);
    Serial.print("\t");
    Serial.println(transmitBuffer[i], BIN);
  }
}
