#include "gpioTest.h"
#include "buttons.h"
#include "display.h"
#include "leds.h"
#include "switches.h"

#include <unistd.h>

// Defining constants
#define BUTTONS_ON 15
#define SWITCHES_ON 15

// Screen Definitions
#define BTN0_X BTN_W * 3
#define BTN0_Y 0
#define BTN1_X BTN_W * 2
#define BTN1_Y 0
#define BTN2_X BTN_W * 1
#define BTN2_Y 0
#define BTN3_X BTN_W * 0
#define BTN3_Y 0
#define BTN_W DISPLAY_WIDTH / 4
#define BTN_H DISPLAY_HEIGHT / 2

// Text Definitions
#define TEXT_SIZE 2

// Setting cursors
#define CURS_Y DISPLAY_HEIGHT / 5
#define CURS0_X BTN0_X + BTN_W / 5
#define CURS1_X BTN1_X + BTN_W / 5
#define CURS2_X BTN2_X + BTN_W / 5
#define CURS3_X BTN3_X + BTN_W / 5

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void gpioTest_buttons() {
  // Initialization of buttons, screen, and variables
  buttons_init();
  display_init();
  uint8_t buttons = 0;
  uint8_t display_map = 0;

  // Blank the screen.
  display_fillScreen(DISPLAY_BLACK);

  // Setting variables for the text
  display_setTextSize(TEXT_SIZE);
  display_setTextWrap(0);
  display_setTextColor(DISPLAY_DARK_MAGENTA);

  // Loops until all buttons are pressed, it shows a colorful box with the
  // button label when each button is pressed
  while (1) {
    // Checking which buttons have been pressed
    buttons = buttons_read();

    // Checking if BTN0 has changed
    if ((BUTTONS_BTN0_MASK & buttons) != (BUTTONS_BTN0_MASK & display_map)) {
      if (BUTTONS_BTN0_MASK & buttons) {
        display_fillRect(BTN0_X, BTN0_Y, BTN_W, BTN_H, DISPLAY_BLUE);
        display_setCursor(CURS0_X, CURS_Y);
        display_print("BTN0");

        // Turning the BTN0 bit on display_map on
        display_map = display_map | BUTTONS_BTN0_MASK;
      } else {
        // Removing the BTN0 box
        display_fillRect(BTN0_X, BTN0_Y, BTN_W, BTN_H, DISPLAY_BLACK);

        // Turning the BTN0 bit on display_map off
        display_map = display_map & ~BUTTONS_BTN0_MASK;
      }
    }
    // Checking if BTN1 has changed
    if ((BUTTONS_BTN1_MASK & buttons) != (BUTTONS_BTN1_MASK & display_map)) {
      if (BUTTONS_BTN1_MASK & buttons) {
        display_fillRect(BTN1_X, BTN1_Y, BTN_W, BTN_H, DISPLAY_MAGENTA);
        display_setCursor(CURS1_X, CURS_Y);
        display_print("BTN1");

        // Turning the BTN1 bit on display_map on
        display_map = display_map | BUTTONS_BTN1_MASK;
      } else {
        // Removing the BTN1 box
        display_fillRect(BTN1_X, BTN1_Y, BTN_W, BTN_H, DISPLAY_BLACK);

        // Turning the BTN1 bit on display_map off
        display_map = display_map & ~BUTTONS_BTN1_MASK;
      }
    }
    // Checking if BTN2 has changed
    if ((BUTTONS_BTN2_MASK & buttons) != (BUTTONS_BTN2_MASK & display_map)) {
      if (BUTTONS_BTN2_MASK & buttons) {
        display_fillRect(BTN2_X, BTN2_Y, BTN_W, BTN_H, DISPLAY_CYAN);
        display_setCursor(CURS2_X, CURS_Y);
        display_print("BTN2");

        // Turning the BTN2 bit on display_map on
        display_map = display_map | BUTTONS_BTN2_MASK;
      } else {
        // Removing the BTN2 box
        display_fillRect(BTN2_X, BTN2_Y, BTN_W, BTN_H, DISPLAY_BLACK);

        // Turning the BTN2 bit on display_map off
        display_map = display_map & ~BUTTONS_BTN2_MASK;
      }
    }
    // Checking if BTN3 has changed
    if ((BUTTONS_BTN3_MASK & buttons) != (BUTTONS_BTN3_MASK & display_map)) {
      if (BUTTONS_BTN3_MASK & buttons) {
        display_fillRect(BTN3_X, BTN3_Y, BTN_W, BTN_H, DISPLAY_WHITE);
        display_setCursor(CURS3_X, CURS_Y);
        display_print("BTN3");

        // Turning the BTN3 bit on display_map on
        display_map = display_map | BUTTONS_BTN3_MASK;
      } else {
        // Removing the BTN3 box
        display_fillRect(BTN3_X, BTN3_Y, BTN_W, BTN_H, DISPLAY_BLACK);

        // Turning the BTN3 bit on display_map off
        display_map = display_map & ~BUTTONS_BTN3_MASK;
      }
    }

    // Exiting the loop if all buttons are pressed
    if (buttons == BUTTONS_ON) {
      // Blank the screen.
      display_fillScreen(DISPLAY_BLACK);
      break;
    }
  }
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void gpioTest_switches() {
  // Initializaion of switches, LEDs, and variables
  switches_init();
  leds_init();
  uint8_t switches = 0;

  // When an switch is on turn on the corisponding LED
  while (1) {
    // Getting the state of the switches
    switches = switches_read();
    // Lighting up the correct LEDs
    leds_write(switches);
    // Exiting the funciton when all of the switches are onn
    if (switches == SWITCHES_ON) {
      // Turning LEDs off when exiting
      leds_write(0);
      break;
    }
  }
}