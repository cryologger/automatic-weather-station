// Configure RockBLOCK 9603
void configureIridium()
{
  //modem.setPowerProfile(IridiumSBD::USB_POWER_PROFILE);     // Assume USB power
  modem.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE); // Assume battery power
  modem.adjustSendReceiveTimeout(180); // Adjust timeout for send/receive commands (default = 300 s)
}

// Write
void writeBuffer()
{
  iterationCounter++;
  message.iterationCounter = iterationCounter;

  // Concatenate current message with existing message(s) stored in transmit buffer
  memcpy(transmitBuffer + (sizeof(message) * (transmitCounter + (retransmitCounter * transmitInterval) - 1)), message.bytes, sizeof(message)); // Copy message to transmit buffer

  // Print data to Serial Monitor
  printUnion();             // Print data stored in union
  //printUnionBinary();       // Print data stored in union in binary format
  //printTransmitBuffer();    // Print data stored transmit buffer array in binary format

  // Clear data stored in union
  memset(message.bytes, 0x00, sizeof(message));
}

// Transmit data via the RockBLOCK 9603 transceiver
void transmitData()
{

  // Start loop timer
  unsigned long loopStartTime = millis();
  unsigned int err;

  // Start the serial power connected to the RockBLOCK modem
  IRIDIUM_PORT.begin(19200);

  // Assign pins 10 & 11 SERCOM functionality
  pinPeripheral(PIN_IRIDIUM_RX, PIO_SERCOM);
  pinPeripheral(PIN_IRIDIUM_TX, PIO_SERCOM);

  // Begin satellite modem operation
  Serial.println(F("Starting modem..."));
  err = modem.begin();
  if (err == ISBD_SUCCESS)
  {
    byte inBuffer[240];  // Buffer to store incoming transmission (240 byte limit)
    size_t inBufferSize = sizeof(inBuffer);
    memset(inBuffer, 0x00, sizeof(inBuffer)); // Clear inBuffer array

    // Transmit and receieve data in binary format
    Serial.println(F("Attempting to transmit data..."));
    err = modem.sendReceiveSBDBinary(transmitBuffer, (sizeof(message) * (transmitCounter + (retransmitCounter * transmitInterval))), inBuffer, inBufferSize);

    // Check if transmission was successful
    if (err == ISBD_SUCCESS)
    {
      retransmitCounter = 0;
      memset(transmitBuffer, 0x00, sizeof(transmitBuffer)); // Clear transmit buffer array

      // Check for incoming message. If no inbound message is available, inBufferSize will be zero
      if (inBufferSize > 0)
      {

        // Print inBuffer size and values of each incoming byte of data
        Serial.print(F("Inbound buffer size is: ")); Serial.println(inBufferSize);
        for (byte i = 0; i < inBufferSize; i++)
        {
          Serial.print(F("Address: "));
          Serial.print(i);
          Serial.print(F("\tValue: "));
          Serial.println(inBuffer[i], HEX);
        }

        // Recompose variables using bitshift
        uint8_t  resetFlagBuffer            = (((uint8_t)inBuffer[10] << 0) & 0xFF);
        uint16_t retransmitLimitBuffer = (((uint16_t)inBuffer[9] << 0) & 0xFF) +
                                         (((uint16_t)inBuffer[8] << 8) & 0xFFFF);
        uint16_t transmitIntervalBuffer     = (((uint16_t)inBuffer[7] << 0) & 0xFF) +
                                              (((uint16_t)inBuffer[6] << 8) & 0xFFFF);
        uint16_t averageIntervalBuffer      = (((uint16_t)inBuffer[5] << 0) & 0xFF) +
                                              (((uint16_t)inBuffer[4] << 8) & 0xFFFF);
        uint32_t sampleIntervalBuffer       = (((uint32_t)inBuffer[3] << 0) & 0xFF) +
                                              (((uint32_t)inBuffer[2] << 8) & 0xFFFF) +
                                              (((uint32_t)inBuffer[1] << 16) & 0xFFFFFF) +
                                              (((uint32_t)inBuffer[0] << 24) & 0xFFFFFFFF);

        // Check validity of incoming data
        if ((sampleIntervalBuffer > 0  && sampleIntervalBuffer <= 86400) &&
            (averageIntervalBuffer > 0  && averageIntervalBuffer <= 24) &&
            (transmitIntervalBuffer > 0  && transmitIntervalBuffer <= 10) &&
            (retransmitLimitBuffer > 0  && retransmitLimitBuffer <= 10) &&
            (resetFlagBuffer == 0 || resetFlagBuffer == 255))
        {
          sampleInterval = sampleIntervalBuffer;              // Update sample interval
          averageInterval = averageIntervalBuffer;            // Update average interval
          transmitInterval = transmitIntervalBuffer;          // Update transmit interval
          retransmitLimit = retransmitLimitBuffer;  // Update max retransmit counter
          resetFlag = resetFlagBuffer;                        // Update force reset flag
        }
      }
    }
    else {
      Serial.print(F("Transmission failed: error "));
      Serial.println(err);
    }
  }
  else
  {
    Serial.print(F("Begin failed: error "));
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED) {
      Serial.println(F("Warning: No modem detected. Please check wiring."));
    }
    return;
  }

  // If transmission or modem begin fails
  if (err != ISBD_SUCCESS)
  {
    retransmitCounter++;

    // Reset counter if retransmit counter is exceeded
    if (retransmitCounter >= retransmitLimit)
    {
      retransmitCounter = 0;
      memset(transmitBuffer, 0x00, sizeof(transmitBuffer));   // Clear transmitBuffer array
    }
  }

  // Power down the modem
  Serial.println(F("Putting the RockBLOCK 9603 to sleep."));
  err = modem.sleep();
  if (err != ISBD_SUCCESS)
  {
    Serial.print(F("Sleep failed: error "));
    Serial.println(err);
  }

  // Close the serial port connected to the RockBLOCK modem
  IRIDIUM_PORT.end();

  // Stop loop timer
  unsigned long loopEndTime = millis() - loopStartTime;
  Serial.print(F("transmitData() function execution: ")); Serial.print(loopEndTime); Serial.println(F(" ms"));

  // Write data to union
  message.transmitDuration = loopEndTime / 1000;

  Serial.print(F("retransmitCounter: ")); Serial.println(retransmitCounter);

  // Check if reset flag was transmitted
  if (resetFlag == 255)
  {
    while (1); // Wait for Watchdog Timer to reset system
  }
}

// Non-blocking RockBLOCK callback function can be called during transmit or GPS signal acquisition
bool ISBDCallback()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > 1000)
  {
    previousMillis = currentMillis;
    petDog(); // Reset the Watchdog Timer
    readBattery(); // Measure battery voltage during Iridium transmission
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Blink LED
  }
  return true;
}

#if DEBUG_IRIDIUM
// Callback to sniff the conversation with the Iridium modem
void ISBDConsoleCallback(IridiumSBD * device, char c)
{
  Serial.write(c);
}

// Callback to to monitor Iridium modem library's run state
void ISBDDiagsCallback(IridiumSBD * device, char c)
{
  Serial.write(c);
}
#endif
