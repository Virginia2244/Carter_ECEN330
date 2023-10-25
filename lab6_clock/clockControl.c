#include "clockControl.h"
#include "clockDisplay.h"
#include "touchscreen.h"
#include <stdio.h>

#define UPDATE_NUM_PERIOD .1
#define DELAY_NUM_PERIOD .5
#define CLOCK_CONTROL_DEBUG 1

// The clock has these statuses
typedef enum {
  WAITING, // Waiting for the screen to be pressed
  LONG_PRESS_DELAY,
  INC_DEC,
  FAST_UPDATE
} clock_status_t;

static clock_status_t clock_status;
static uint32_t delay_cnt;
static uint32_t delay_num_tics;
static uint8_t update_cnt;
static uint8_t update_num_tics;

// Initialize the clock control state machine, with a given period in
// seconds.
void clockControl_init(double period_s) {
  clock_status = WAITING;
  delay_cnt = 0;
  delay_num_tics = DELAY_NUM_PERIOD / period_s;
  update_cnt = 0;
  update_num_tics = UPDATE_NUM_PERIOD / period_s;
}

void debug_clock_status() {
  static clock_status_t previous_clock_status = FAST_UPDATE;
  if (previous_clock_status != clock_status) {
    previous_clock_status = clock_status;

    printf("CLOCK_STATUS\tclock_state: ");
    // Printing the current state
    switch (previous_clock_status) {
    case WAITING:
      printf("WAITING");
      break;
    case LONG_PRESS_DELAY:
      printf("LONG_PRESS_DELAY");
      break;
    case INC_DEC:
      printf("INC_DEC");
      break;
    case FAST_UPDATE:
      printf("FAST_UPDATE");
      break;
    }
    printf("\n");
  }
}

// Tick the clock control state machine
void clockControl_tick() {
  // Mealy outputs and changing states
  switch (clock_status) {
  case WAITING:
    // Checking to see if the touchscreen has been pressed. If it has go to
    // LONG_PRESS_DELAY. If we missed a press, go to INC_DEC
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      clock_status = LONG_PRESS_DELAY;
    } else if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      clock_status = INC_DEC;
    }
    break;
  case LONG_PRESS_DELAY:
    //  Wait for some time (DELAY_NUM_PERIOD) before going to fast decrement. If
    //  the touchscreen is released before enough time has passed go to
    //  FAST_UPDATE
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      clock_status = INC_DEC;
    } else if (delay_cnt >= delay_num_tics) {
      clock_status = FAST_UPDATE;
      delay_cnt = 0;
    }
    break;
  case INC_DEC:
    // Clear the touchscreen status and return to WAITING
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      clock_status = WAITING;
      clockDisplay_performIncDec(touchscreen_get_location());
      touchscreen_ack_touch();
    }
    break;
  case FAST_UPDATE:
    // While the touchscreen is being pressed call the inc/dec funciton 10 times
    // a second.
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      clock_status = WAITING;
      update_cnt = 0;
      touchscreen_ack_touch();
    } else if (update_cnt >= update_num_tics) {
      clockDisplay_performIncDec(touchscreen_get_location());
      update_cnt = 0;
    }
    break;
  }

  // More outputs
  switch (clock_status) {
  case WAITING:
    break;
  case LONG_PRESS_DELAY:
    delay_cnt++;
    break;
  case INC_DEC:
    break;
  case FAST_UPDATE:
    update_cnt++;
    break;
  }

  // Turning the debug statements on or off
  if (CLOCK_CONTROL_DEBUG) {
    debug_clock_status();
  }
}
