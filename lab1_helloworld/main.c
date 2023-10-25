/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdio.h>

#include "display.h"

#define TEXT_SIZE 2
#define CURSOR_X 10
#define CURSOR_Y (DISPLAY_HEIGHT / 2)

// Circle magic numbers
#define RADIUS 32
#define CIRCLE_Y DISPLAY_HEIGHT / 2
#define DRAW_CIRCLE_X DISPLAY_WIDTH / 4
#define FILL_CIRCLE_X DISPLAY_WIDTH * 3 / 4

// Triangle magic numbers
#define T_OFFSET 16
#define FILL_TRIANGLE_X_1 DISPLAY_WIDTH / 2 + T_OFFSET * 2
#define FILL_TRIANGLE_Y_1 DISPLAY_HEIGHT / 4 - T_OFFSET * 3
#define FILL_TRIANGLE_X_2 DISPLAY_WIDTH / 2 - T_OFFSET * 2
#define FILL_TRIANGLE_Y_2 DISPLAY_HEIGHT / 4 - T_OFFSET * 3
#define FILL_TRIANGLE_X_3 DISPLAY_WIDTH / 2
#define FILL_TRIANGLE_Y_3 DISPLAY_HEIGHT / 4 + 16
#define DRAW_TRIANGLE_X_1 DISPLAY_WIDTH / 2 + T_OFFSET * 2
#define DRAW_TRIANGLE_Y_1 DISPLAY_HEIGHT * 3 / 4 + T_OFFSET * 3
#define DRAW_TRIANGLE_X_2 DISPLAY_WIDTH / 2 - T_OFFSET * 2
#define DRAW_TRIANGLE_Y_2 DISPLAY_HEIGHT * 3 / 4 + T_OFFSET * 3
#define DRAW_TRIANGLE_X_3 DISPLAY_WIDTH / 2
#define DRAW_TRIANGLE_Y_3 DISPLAY_HEIGHT * 3 / 4 - 16

// Print out "hello world" on both the console and the LCD screen.
int main() {
  // Initialize display driver, and fill scren with black
  display_init();
  display_fillScreen(DISPLAY_BLACK); // Blank the screen.

  // Set the cursor location and print to the LCD
  display_setCursor(CURSOR_X, CURSOR_Y);

  // Drawing the x on the middle of the screen
  display_drawLine(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_GREEN);
  display_drawLine(0, DISPLAY_HEIGHT, DISPLAY_WIDTH, 0, DISPLAY_GREEN);

  // Drawing the circles
  display_drawCircle(DRAW_CIRCLE_X, CIRCLE_Y, RADIUS, DISPLAY_RED);
  display_fillCircle(FILL_CIRCLE_X, CIRCLE_Y, RADIUS, DISPLAY_RED);

  // Drawing the triangles

  display_fillTriangle(FILL_TRIANGLE_X_1, FILL_TRIANGLE_Y_1, FILL_TRIANGLE_X_2,
                       FILL_TRIANGLE_Y_2, FILL_TRIANGLE_X_3, FILL_TRIANGLE_Y_3,
                       DISPLAY_YELLOW);
  display_drawTriangle(DRAW_TRIANGLE_X_1, DRAW_TRIANGLE_Y_1, DRAW_TRIANGLE_X_2,
                       DRAW_TRIANGLE_Y_2, DRAW_TRIANGLE_X_3, DRAW_TRIANGLE_Y_3,
                       DISPLAY_YELLOW);
  return 0;
}
