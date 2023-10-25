#include "interrupts.h"
#include "armInterrupts.h"
#include "xil_io.h"
#include "xparameters.h"
#include <stdio.h>

// Register address mapping
#define IPR 0x04
#define IER 0x08
#define IAR 0X0C
#define SIE 0X10
#define CIE 0X14
#define MER 0X1C

// Funciton things
#define FUNCTION_QUANTITY 3
#define MER_ENABLE 3

// List of functions to call upon interuprs
static void (*isrFcnPtrs[FUNCTION_QUANTITY])() = {NULL};

// Reading the registers.
static uint32_t readRegister(uint32_t offset) {
  return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + offset);
}

// writing to the registers
static void writeRegister(uint32_t offset, uint32_t value) {
  return Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + offset, value);
}

static void interrupts_isr() {
  uint32_t registers = readRegister(IPR);
  // Loop through each interrupt input
  for (uint8_t i = 0; i < FUNCTION_QUANTITY; i++) {
    // Check if it has an interrupt pending
    if (registers & (1 << i)) {
      // Check if there is a callback
      if (isrFcnPtrs[i]) {
        // Call the callback function
        isrFcnPtrs[i]();
      }
      // Agnolaging the interupt
      writeRegister(IAR, 1 << i);
    }
  }
}

// Initialize interrupt hardware
// This function should:
// 1. Configure AXI INTC registers to:
//  - Enable interrupt output (see Master Enable Register)
//  - Disable all interrupt input lines.
// 2. Enable the Interrupt system on the ARM processor, and register an ISR
// handler function. This is done by calling:
//  - armInterrupts_init()
//  - armInterrupts_setupIntc(isr_fcn_ptr)
//  - armInterrupts_enable()
void interrupts_init() {
  // Enabling interrupt output
  writeRegister(MER, MER_ENABLE);
  // Disable all interrupt input lines
  writeRegister(IER, 0x00);
  armInterrupts_init();
  armInterrupts_setupIntc(interrupts_isr);
  armInterrupts_enable();
}

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()) { isrFcnPtrs[irq] = fcn; }

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq) { writeRegister(SIE, 1 << irq); }

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq) { writeRegister(CIE, 1 << irq); }