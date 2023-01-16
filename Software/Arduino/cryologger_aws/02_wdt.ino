// Configure the Watchdog Timer (WDT) 
// Performs a system reset if loop() blocks for more than 8-16 seconds
void configureWdt()
{
  /*
  // Configure Generic Clock Controller 2 (GCLK1) for use with the Watchdog Timer (WDT) at 1.024kHz
  REG_GCLK_GENDIV = GCLK_GENDIV_ID(2) |           // Select Generic Clock Controller 2
                    GCLK_GENDIV_DIV(4) ;          // Divide the 32.768kHz clock source by divisor 32, where 2^(4 + 1): 32.768kHz/32=1.024kHz
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_ID(2) |         // Select GCLK2
                     GCLK_GENCTRL_GENEN |         // Enable GCLK2
                     GCLK_GENCTRL_SRC_OSCULP32K | // Set the clock source to the ultra low power oscillator (OSCULP32K)
                     GCLK_GENCTRL_DIVSEL |        // Set to divide by 2^(GCLK_GENDIV_DIV(4) + 1)
                     GCLK_GENCTRL_IDC;            // Set the duty cycle to 50/50 HIGH/LOW (REQUIRED?)
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization to complete
*/
  // Feed GCLK2 to WDT (preconfigured by the RTC) 
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_ID_WDT |        // Identify it as the WDT clock
                     GCLK_CLKCTRL_CLKEN |         // Enable the generic clock by setting the Clock Enabler bit
                     GCLK_CLKCTRL_GEN_GCLK2;      // Select GCLK2
  while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

  REG_WDT_EWCTRL = WDT_EWCTRL_EWOFFSET_8K;        // Set the Early Warning Interrupt Time Offset to 8 seconds   // WDT->EWCTRL.bit.EWOFFSET = 0xA;
  REG_WDT_INTENSET = WDT_INTENSET_EW;             // Enable the Early Warning interrupt                         // WDT->INTENSET.bit.EW = 1;
  REG_WDT_CONFIG = WDT_CONFIG_PER_16K;            // Set the WDT reset timeout to 16 seconds                    // WDT->CONFIG.bit.PER = 0xB;
  REG_WDT_CTRL = WDT_CTRL_ENABLE;                 // Enable the WDT in normal mode                              // WDT->CTRL.bit.ENABLE = 1;
  while (WDT->STATUS.bit.SYNCBUSY);               // Await synchronization of registers between clock domains

  // Configure and enable WDT interrupt
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);                  // Top priority
  NVIC_EnableIRQ(WDT_IRQn);
}

// Reset the WDT
void petDog()
{
  //DEBUG_PRINT("WDT interrupt: "); DEBUG_PRINTLN(wdtCounter);
  REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;  // Clear the WDT and restart time-out period // WDT->CLEAR.bit.CLEAR = 0xA5;
  while (WDT->STATUS.bit.SYNCBUSY);     // Await synchronization of registers between clock domains
  wdtFlag = false;                      // Clear WDT flag
  wdtCounter = 0;                       // Reset WDT interrupt counter
}

// WDT ISR
void WDT_Handler()
{
  REG_WDT_INTFLAG = WDT_INTFLAG_EW;   // Clear the Early Warning interrupt flag //WDT->INTFLAG.bit.EW = 1;

  // Perform system reset after 10 WDT interrupts (should not occur)
  if (wdtCounter < 10)
  {
    REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;  // Clear the WDT and restart time-out period //WDT->CLEAR.bit.CLEAR = 0xA5;
    while (WDT->STATUS.bit.SYNCBUSY);     // Await synchronization of registers between clock domains
  }
  else
  {
    //WDT->CTRL.bit.ENABLE = 0;         // Debugging only: Disable WDT
    //digitalWrite(PIN_LED_RED, HIGH);  // Debugging only: Turn on LED to indicate WDT trigger
    while (true);                     // Force WDT to reset the system
  }
  wdtFlag = true; // Set the WDT flag
  wdtCounter++; // Increment WDT interrupt counter
}
