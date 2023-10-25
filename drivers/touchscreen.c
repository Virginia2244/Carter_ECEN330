#include "touchscreen.h"
#include "display.h"
#include "stdio.h"

#define PERIOD .05
#define ROUND_UP .5
#define TOUCHSCREEN_DEBUG 0

// Variable for my state_status
typedef enum {
  WAITING,      // Touchscreen is idle (not pressed)
  ADC_SETTLING, // Touchscreen is pressed and waiting to for the x,y locaion to
                // be secured
  PRESSED_ST    // Touchscreen is pressed and enough time has passed
} state_status_t;

static state_status_t state_status;
static touchscreen_status_t acknowledge_status;
static uint16_t adc_timer;
static uint16_t adc_settle_ticks;
static display_point_t screen_touch;
static uint8_t screen_touch_z;

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds) {
  state_status = WAITING;
  acknowledge_status = TOUCHSCREEN_IDLE;
  adc_timer = 0;
  adc_settle_ticks = (PERIOD / period_seconds) + ROUND_UP;
  screen_touch.x = 0;
  screen_touch.y = 0;
  screen_touch_z = 0;
};

// Debug print statements
void touchscreen_debug() {
  static state_status_t previous_state_status = PRESSED_ST;
  static touchscreen_status_t previous_acknowledge_status =
      TOUCHSCREEN_RELEASED;
  // Checking to see if the state or the acknoledge status has changed
  if (previous_state_status != state_status ||
      previous_acknowledge_status != acknowledge_status) {

    previous_state_status = state_status;
    previous_acknowledge_status = acknowledge_status;

    printf("TOUCHSCREEN\tstate_state: ");
    // Printing the current state
    switch (previous_state_status) {
    case WAITING:
      printf("WAITING\t");
      break;
    case ADC_SETTLING:
      printf("ADC_SETTLING");
      break;
    case PRESSED_ST:
      printf("PRESSED_ST\t");
      break;
    }
    // Printing the acknoledge status and the cordinates of the screen touch
    printf("\tacknoledge_status: %i\t x:%i, y:%i\n", acknowledge_status,
           screen_touch.x, screen_touch.y);
  }
}

// Tick the touchscreen driver state machine
void touchscreen_tick() {
  // Updating the state and mealy outputs
  switch (state_status) {
  case WAITING:
    // Clearing the old touch data nd moving to the ADC_SETTLING state
    if (display_isTouched()) {
      state_status = ADC_SETTLING;
      display_clearOldTouchData();
    }
    break;
  case ADC_SETTLING:
    // Making sure that the button has been touched for the 50ms before sending
    // the state to PRESSED_ST
    if (!display_isTouched()) {
      state_status = WAITING;
    } else if (display_isTouched() && (adc_timer == adc_settle_ticks)) {
      state_status = PRESSED_ST;
      display_getTouchedPoint(&screen_touch.x, &screen_touch.y,
                              &screen_touch_z);
    }
    break;
  case PRESSED_ST:
    // Waiting for the button to be released before moving back to WAITING
    if (!display_isTouched()) {
      state_status = WAITING;
      acknowledge_status = TOUCHSCREEN_RELEASED;
    }
    break;
  }

  // Executing the more outputs
  switch (state_status) {
  case WAITING:
    adc_timer = 0;
    break;
  case ADC_SETTLING:
    adc_timer++;
    break;
  case PRESSED_ST:
    acknowledge_status = TOUCHSCREEN_PRESSED;
    break;
  }
  if (TOUCHSCREEN_DEBUG) {
    touchscreen_debug();
  }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status() { return acknowledge_status; }

// Acknowledge the touchscreen touch.  This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch() {
  if (acknowledge_status == TOUCHSCREEN_RELEASED) {
    acknowledge_status = TOUCHSCREEN_IDLE;
  }
}

// Get the (x,y) location of the last touchscreen touch
display_point_t touchscreen_get_location() { return screen_touch; }