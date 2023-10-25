#include "buttons.h"
#include "xil_io.h"
#include "xparameters.h"

#define TRI_OFFSET 0x4
#define BUTTONS_ON 0xf

// Helper funtion to read from the specified register
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// Helper funciton to write to the specified register
static void writeRegister(uint32_t offset, uint32_t value) {
  Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}

// Initializes the button driver software and hardware.
void buttons_init() { writeRegister(TRI_OFFSET, BUTTONS_ON); }

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read() { return (uint8_t)readRegister(0); }