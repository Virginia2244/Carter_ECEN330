#include "interrupt_test.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include <stdio.h>

// Timer periods
#define TIMER_0_PERIOD 10
#define TIMER_1_PERIOD 1
#define TIMER_2_PERIOD .1

// led numbers
#define LED_0 2
#define LED_1 1
#define LED_2 0

// Helper funciton to togle the leds
static void toggle_led(uint8_t led) { leds_write(leds_read() ^ (1 << led)); }

static void led_0() {
  // Toggle the led
  toggle_led(LED_0);
  // acknoledge the interupts
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
}
static void led_1() {
  // Toggle the led
  toggle_led(LED_1);
  // acknoledge the interupts
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);
}
static void led_2() {
  // Toggle the led
  toggle_led(LED_2);
  // acknoledge the interupts
  intervalTimer_ackInterrupt(INTERVAL_TIMER_2);
}

/*
This function is a small test application of your interrupt controller.  The
goal is to use the three AXI Interval Timers to generate interrupts at
different rates (10Hz, 1Hz, 0.1Hz), and create interrupt handler functions
that change the LEDs at this rate.  For example, the 1Hz interrupt will flip
an LED value each second, resulting in LED that turns on for 1 second, off for
1 second, repeatedly.

For each interval timer:
    1. Initialize it as a count down timer with appropriate period.
    2. Enable the timer's interrupt output.
    3. Enable the associated interrupt input on the interrupt controller.
    4. Register an appropriate interrupt handler function (isr_timer0,
isr_timer1, isr_timer2).
    5. Start the timer.

Make sure you call `interrupts_init()` first!
*/
void interrupt_test_run() {
  // Initalizing LEDs
  leds_init();
  // Enabling interupts on the global level
  interrupts_init();
  // Initializing timers
  intervalTimer_initCountDown(INTERVAL_TIMER_0, TIMER_0_PERIOD);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, TIMER_1_PERIOD);
  intervalTimer_initCountDown(INTERVAL_TIMER_2, TIMER_2_PERIOD);
  // Enabling interrupts on the timer level
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_2);
  // Enabling interrupts on the AXI level
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_2_INTERRUPT_IRQ);
  // Linking LED funcitons to timer interupts function calls
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, led_0);
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, led_1);
  interrupts_register(INTERVAL_TIMER_2_INTERRUPT_IRQ, led_2);
  // Starting timers
  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_2);

  // getting the time to see if the timer is working
  while (1) {
    printf("Timer_0:%f\tTimer_1:%f\tTimer_2:%f\r",
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_0),
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_1),
           intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_2));
  }
}