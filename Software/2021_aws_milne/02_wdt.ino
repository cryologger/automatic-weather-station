// Configure the Watchdog Timer to perform a system reset if loop() blocks for more than 8-16 seconds
void configureWatchdog() 
{
  // Set up the generic clock (GCLK2) used to clock the watchdog timer at 1.024kHz
  REG_GCLK_GENDIV = GCLK_GENDIV_DIV(4) |            // Divide the 32.768kHz clock source by divisor 32, where 2^(4 + 1): 32.768kHz/32=1.024kHz
                    GCLK_GENDIV_ID(2);              // Select Generic Clock (GCLK) 2
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  REG_GCLK_GENCTRL = GCLK_GENCTRL_DIVSEL |          // Set to divide by 2^(GCLK_GENDIV_DIV(4) + 1)
                     GCLK_GENCTRL_IDC |             // Set the duty cycle to 50/50 HIGH/LOW
                     GCLK_GENCTRL_GENEN |           // Enable GCLK2
                     GCLK_GENCTRL_SRC_OSCULP32K |   // Set the clock source to the ultra low power oscillator (OSCULP32K)
                     GCLK_GENCTRL_ID(2);            // Select GCLK2
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  // Feed GCLK2 to WDT (Watchdog Timer)
  REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN |           // Enable GCLK2 to the WDT
                     GCLK_CLKCTRL_GEN_GCLK2 |       // Select GCLK2
                     GCLK_CLKCTRL_ID_WDT;           // Feed the GCLK2 to the WDT
  while (GCLK->STATUS.bit.SYNCBUSY);                // Wait for synchronization

  WDT->EWCTRL.bit.EWOFFSET = 0xA;                   // Set the Early Warning Interrupt Time Offset to 8 seconds //REG_WDT_EWCTRL = WDT_EWCTRL_EWOFFSET_8K;
  WDT->INTENSET.bit.EW = 1;                         // Enable the Early Warning interrupt //REG_WDT_INTENSET = WDT_INTENSET_EW;
  WDT->CONFIG.bit.PER = 0xB;                        // Set the WDT reset timeout to 16 seconds //REG_WDT_CONFIG = WDT_CONFIG_PER_16K;
  WDT->CTRL.bit.ENABLE = 1;                         // Enable the WDT in normal mode //REG_WDT_CTRL = WDT_CTRL_ENABLE;
  while (WDT->STATUS.bit.SYNCBUSY);                 // Await synchronization of registers between clock domains

  // Configure and enable WDT interrupt
  NVIC_DisableIRQ(WDT_IRQn);
  NVIC_ClearPendingIRQ(WDT_IRQn);
  NVIC_SetPriority(WDT_IRQn, 0);                    // Top priority
  NVIC_EnableIRQ(WDT_IRQn);
}

// Pet the Watchdog Timer
void petDog() 
{
  watchdogCounter = 0;              // Clear Watchdog Timer trigger counter
  WDT->CLEAR.bit.CLEAR = 0xA5;      // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
  while (WDT->STATUS.bit.SYNCBUSY); // Await synchronization of registers between clock domains
}

// Watchdog Timer interrupt service routine
void WDT_Handler() 
{
  // Check if system is asleep
  if (sleepFlag) 
  {
    // Permit a number Watchdog Timer triggers before forcing system reset.
    if (watchdogCounter < 10) 
    {
      WDT->INTFLAG.bit.EW = 1;          // Clear the Early Warning interrupt flag //REG_WDT_INTFLAG = WDT_INTFLAG_EW;
      WDT->CLEAR.bit.CLEAR = 0xA5;      // Clear the Watchdog Timer and restart time-out period //REG_WDT_CLEAR = WDT_CLEAR_CLEAR_KEY;
      while (WDT->STATUS.bit.SYNCBUSY); // Await synchronization of registers between clock domains
    }
  }
  else {
    while (true);                     // Force Watchdog Timer to reset the system
  }
}
