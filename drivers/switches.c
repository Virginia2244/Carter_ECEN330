#include "switches.h"
#include "xil_io.h"
#include "xparameters.h"

#define TRI_OFFSET 0x4
#define BUTTONS_ON 0xf
// Helper funtion to read from the specified register
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset);
}

// Helper funciton to write to the specified register
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);
}

// Initializes the SWITCHES driver software and hardware.
void switches_init() { writeRegister(TRI_OFFSET, BUTTONS_ON); }

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
uint8_t switches_read() { return (uint8_t)readRegister(0); }