#include "intervalTimer.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

// Offset
#define TCSR0 0x00
#define TLR0 0x04
#define TCR0 0x08
#define TCSR1 0x10
#define TLR1 0x14
#define TCR1 0x18

// Register bits
#define CASC 0b100000000000 // 11
#define ENALL 0b10000000000 // 10
#define PWMA0 0b1000000000  // 9
#define TOINT 0b100000000   // 8
#define ENT0 0b10000000     // 7
#define ENIT0 0b1000000     // 6
#define LOAD 0b100000       // 5
#define ARHT0 0b10000       // 4
#define CAPT0 0b1000        // 3
#define GENT0 0B100         // 2
#define UDT0 0b10           // 1
#define MDT0 0b1            // 0

// Conversion to things
#define SHIFT 32

// Reading the registers.
static uint32_t readRegister(uint8_t timerNumber, uint32_t offset) {
  switch (timerNumber) {
  case INTERVAL_TIMER_0:
    return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + offset);
  case INTERVAL_TIMER_1:
    return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
  case INTERVAL_TIMER_2:
    return Xil_In32(XPAR_AXI_TIMER_2_BASEADDR + offset);
  }
}

// writing to the registers
static void writeRegister(uint8_t timerNumber, uint32_t offset,
                          uint32_t value) {
  switch (timerNumber) {
  case INTERVAL_TIMER_0:
    return Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + offset, value);
  case INTERVAL_TIMER_1:
    return Xil_Out32(XPAR_AXI_TIMER_1_BASEADDR + offset, value);
  case INTERVAL_TIMER_2:
    return Xil_Out32(XPAR_AXI_TIMER_2_BASEADDR + offset, value);
  }
}

static void writeBit(uint8_t timerNumber, uint32_t offset, uint32_t bit,
                     bool state) {
  if (state) {
    writeRegister(timerNumber, offset, readRegister(timerNumber, offset) | bit);
  } else {
    writeRegister(timerNumber, offset,
                  readRegister(timerNumber, offset) & ~bit);
  }
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts up
// 2. Initialize both LOAD registers with zeros
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountUp(uint32_t timerNumber) {
  uint32_t load = 0;
  writeRegister(timerNumber, TLR0, load);
  writeRegister(timerNumber, TLR1, load);
  writeRegister(timerNumber, TCSR0, CASC | ARHT0);
  intervalTimer_reload(timerNumber);
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// 2. Initialize LOAD registers with appropriate values, given the `period`.
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period) {
  uint64_t load = period * (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
  writeRegister(timerNumber, TLR0, load);
  writeRegister(timerNumber, TLR1, load >> SHIFT);
  writeRegister(timerNumber, TCSR0, CASC | ARHT0 | UDT0);
  intervalTimer_reload(timerNumber);
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_start(uint32_t timerNumber) {
  writeBit(timerNumber, TCSR0, ENT0, 1);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_stop(uint32_t timerNumber) {
  writeBit(timerNumber, TCSR0, ENT0, 0);
}

// This function is called whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.  The load registers should have already
// been set in the appropriate `init` function, so there is no need to set
// them here.  You just need to enable the load (and remember to disable it
// immediately after otherwise you will be loading indefinitely).
void intervalTimer_reload(uint32_t timerNumber) {
  // Reloading them in such a way that there should be no weird values
  writeBit(timerNumber, TCSR0, LOAD, 1);
  writeBit(timerNumber, TCSR1, LOAD, 1);
  writeBit(timerNumber, TCSR1, LOAD, 0);
  writeBit(timerNumber, TCSR0, LOAD, 0);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
  uint64_t cycles = readRegister(timerNumber, TCR0) |
                    ((uint64_t)readRegister(timerNumber, TCR1) << SHIFT);
  switch (timerNumber) {
  case INTERVAL_TIMER_0:
    return cycles / (double)XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;
  case INTERVAL_TIMER_1:
    return cycles / (double)XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;
  case INTERVAL_TIMER_2:
    return cycles / (double)XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ;
  }
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber) {
  writeBit(timerNumber, TCSR0, ENIT0, 1);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber) {
  writeBit(timerNumber, TCSR0, ENIT0, 0);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber) {
  writeBit(timerNumber, TCSR0, TOINT, 1);
}