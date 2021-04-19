// Print current time and date
void printDateTime(time_t t) 
{
  Serial.print((day(t) < 10) ? "0" : ""); Serial.print(day(t), DEC); Serial.print('/');
  Serial.print((month(t) < 10) ? "0" : ""); Serial.print(month(t), DEC); Serial.print('/');
  Serial.print(year(t), DEC); Serial.print(' ');
  Serial.print((hour(t) < 10) ? "0" : ""); Serial.print(hour(t), DEC); Serial.print(':');
  Serial.print((minute(t) < 10) ? "0" : ""); Serial.print(minute(t), DEC); Serial.print(':');
  Serial.print((second(t) < 10) ? "0" : ""); Serial.println(second(t), DEC);
}

// Print statisticsitre
void printStatistics() 
{
  Serial.println();
  Serial.println(F("Statistics"));
  Serial.println(F("============================================================================"));
  Serial.print(F("Voltage\t\t"));
  Serial.print(F("Samples: ")); Serial.print(batteryStats.count());
  Serial.print(F("\tMin: "));   Serial.print(batteryStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(batteryStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(batteryStats.average());
  Serial.print(F("Temperature (RTC)\t"));
  Serial.print(F("Samples: ")); Serial.print(rtcStats.count());
  Serial.print(F("\tMin: ")); Serial.print(rtcStats.minimum());
  Serial.print(F("\tMax: ")); Serial.print(rtcStats.maximum());
  Serial.print(F("\tMean: ")); Serial.println(rtcStats.average());
  Serial.print(F("Temperature\t"));
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
  Serial.print(F("Samples: ")); Serial.print(windSpeedStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(windSpeedStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(windSpeedStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(windSpeedStats1.average());
  Serial.print(F("vn 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(vnStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(vnStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(vnStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(vnStats1.average());
  Serial.print(F("ve 1\t\t"));
  Serial.print(F("Samples: ")); Serial.print(veStats1.count());
  Serial.print(F("\tMin: ")); Serial.print(veStats1.minimum());
  Serial.print(F("\tMax: ")); Serial.print(veStats1.maximum());
  Serial.print(F("\tMean: ")); Serial.println(veStats1.average());
}

// Print union/structure
void printUnion() 
{
  Serial.println();
  Serial.println(F("Union/structure"));
  Serial.println(F("==================================="));
  Serial.print(F("unixtime:\t\t")); Serial.println(message.unixtime);
  Serial.print(F("intTemperature:\t")); Serial.println(message.intTemperature);
  Serial.print(F("extTemperature:\t\t")); Serial.println(message.extTemperature);
  Serial.print(F("humidity:\t\t")); Serial.println(message.humidity);
  Serial.print(F("windSpeed 1:\t\t")); Serial.println(message.windSpeed1);
  Serial.print(F("windDirection 1:\t")); Serial.println(message.windDirection1);
  Serial.print(F("windGust 1:\t\t")); Serial.println(message.windGust1);
  Serial.print(F("windGustDirection 1:\t")); Serial.println(message.windGustDirection1);
  Serial.print(F("voltage:\t\t")); Serial.println(message.voltage);
  Serial.print(F("transmitDuration:\t\t")); Serial.println(message.transmitDuration);
  Serial.print(F("messageCounter:\t\t")); Serial.println(message.messageCounter);
}

// Print contents of union/structure
void printUnionBinary() 
{
  Serial.println();
  Serial.println(F("Union/structure"));
  Serial.println(F("========================="));
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
  Serial.println(F("========================="));
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
