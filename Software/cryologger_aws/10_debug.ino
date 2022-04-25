void printLine()
{
  for (byte i = 0; i < 80; i++)
  {
    DEBUG_PRINT("-");
  }
  DEBUG_PRINTLN();
}

void printTab(byte _times)
{
  for (byte i = 0; i < _times; i++)
  {
    DEBUG_PRINT("\t");
  }
}

// Print user-defined beacon settings
void printSettings()
{
  printLine();
  DEBUG_PRINTLN("Current Settings");
  printLine();
  DEBUG_PRINT("alarmInterval: ");     printTab(2);  DEBUG_PRINTLN(alarmInterval);
  DEBUG_PRINT("transmitInterval: ");  printTab(1);  DEBUG_PRINTLN(transmitInterval);
  DEBUG_PRINT("retransmitCounter: "); printTab(1);  DEBUG_PRINTLN(retransmitCounter);
  DEBUG_PRINT("retransmitLimit: ");   printTab(1);  DEBUG_PRINTLN(retransmitLimit);
  DEBUG_PRINT("resetFlag: ");         printTab(2);  DEBUG_PRINTLN(resetFlag);
  printLine();
}

void printTimers()
{
  printLine();
  DEBUG_PRINTLN("Function Execution Timers");
  printLine();
  DEBUG_PRINT("readRtc: ");       printTab(1);  DEBUG_PRINTLN(timer.rtc);
  DEBUG_PRINT("readDps310: ");    printTab(1);  DEBUG_PRINTLN(timer.dps310);
  DEBUG_PRINT("readLsm303: ");    printTab(1);  DEBUG_PRINTLN(timer.lsm303);
  DEBUG_PRINT("readHmp60: ");     printTab(1);  DEBUG_PRINTLN(timer.hmp60);
  DEBUG_PRINT("readGnss: ");      printTab(1);  DEBUG_PRINTLN(timer.gnss);
  DEBUG_PRINT("transmitData: ");  printTab(1);  DEBUG_PRINTLN(timer.iridium);
  DEBUG_PRINT("freeRam(): ");     printTab(1);  DEBUG_PRINTLN(freeRam());

  printLine();
}

// Print contents of union/structure storing Mobile Originated (MO) SBD message data
void printMoSbd()
{

  printLine();
  DEBUG_PRINTLN("MO-SBD Message Data");
  printLine();
  
    DEBUG_PRINT("unixtime:");         printTab(2);  DEBUG_PRINTLN(moSbdMessage.unixtime);
    DEBUG_PRINT("intTemperature:");   printTab(2);  DEBUG_PRINTLN(moSbdMessage.intTemperature);
    DEBUG_PRINT("intPressure:");      printTab(2);  DEBUG_PRINTLN(moSbdMessage.intPressure);
    DEBUG_PRINT("extTemperature:");   printTab(2);  DEBUG_PRINTLN(moSbdMessage.extTemperature);
    DEBUG_PRINT("extHumidity:");      printTab(2);  DEBUG_PRINTLN(moSbdMessage.extHumidity);
    DEBUG_PRINT("pitch:");            printTab(3);  DEBUG_PRINTLN(moSbdMessage.pitch);
    DEBUG_PRINT("roll:");             printTab(3);  DEBUG_PRINTLN(moSbdMessage.roll);
    DEBUG_PRINT("latitude:");         printTab(2);  DEBUG_PRINTLN(moSbdMessage.latitude);
    DEBUG_PRINT("longitude:");        printTab(2);  DEBUG_PRINTLN(moSbdMessage.longitude);
    DEBUG_PRINT("satellites:");       printTab(2);  DEBUG_PRINTLN(moSbdMessage.satellites);
    DEBUG_PRINT("hdop:");             printTab(3);  DEBUG_PRINTLN(moSbdMessage.hdop);
    DEBUG_PRINT("voltage:");          printTab(2);  DEBUG_PRINTLN(moSbdMessage.voltage);
    DEBUG_PRINT("transmitDuration:"); printTab(1);  DEBUG_PRINTLN(moSbdMessage.transmitDuration);
    DEBUG_PRINT("transmitStatus:");   printTab(2);  DEBUG_PRINTLN(moSbdMessage.transmitStatus);
    DEBUG_PRINT("iterationCounter:"); printTab(1);  DEBUG_PRINTLN(moSbdMessage.iterationCounter);
  
  printLine();

}

// Print contents of union/structure storing Mobile Originated (MT) SBD message data
void printMtSbd()
{
  printLine();
  DEBUG_PRINTLN("MT-SBD Message Data");
  printLine();
  DEBUG_PRINT("alarmInterval:");    printTab(2);  DEBUG_PRINTLN(mtSbdMessage.alarmInterval);
  DEBUG_PRINT("transmitInterval:"); printTab(1);  DEBUG_PRINTLN(mtSbdMessage.transmitInterval);
  DEBUG_PRINT("retransmitLimit:");  printTab(1);  DEBUG_PRINTLN(mtSbdMessage.retransmitLimit);
  DEBUG_PRINT("resetFlag:");        printTab(2);  DEBUG_PRINTLN(mtSbdMessage.resetFlag);
  printLine();
}

// Print contents of union/structure
void printMoSbdHex()
{
  DEBUG_PRINTLN("MO-SBD Union/structure ");
  printLine();
  char tempData[16];
  DEBUG_PRINTLN("Byte\tHex");
  for (int i = 0; i < sizeof(moSbdMessage); ++i)
  {
    sprintf(tempData, "%d\t0x%02X", i, moSbdMessage.bytes[i]);
    DEBUG_PRINTLN(tempData);
  }
  printLine();
}

// Print contents of transmit buffer
void printMoSbdBuffer()
{
  printLine();
  DEBUG_PRINTLN("MO-SBD Transmit buffer");
  printLine();
  char tempData[16];
  DEBUG_PRINTLN("Byte\tHex");
  for (int i = 0; i < moSbdBufferSize; ++i)
  {
    sprintf(tempData, "%d\t0x%02X", i, moSbdBuffer[i]);
    DEBUG_PRINTLN(tempData);
  }
}

// Print contents of transmit buffer
void printMtSbdBuffer()
{
  printLine();
  DEBUG_PRINTLN("MT-SBD Transmit buffer");
  printLine();
  // Print contents of mtSbdBuffer in hexadecimal
  char tempData[16];
  DEBUG_PRINTLN("Byte\tHex");
  for (int i = 0; i < mtSbdBufferSize; ++i)
  {
    sprintf(tempData, "%d\t0x%02X", i, mtSbdBuffer[i]);
    DEBUG_PRINTLN(tempData);
  }
}

// Function to print available 32K SRAM memory
extern "C" char *sbrk(int i);
int freeRam()
{
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}
