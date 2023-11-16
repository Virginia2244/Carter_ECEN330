#include "gameControl.h"
#include "config.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"

#include <stdio.h>

#define HALF_TOTAL_MISSILES (CONFIG_MAX_TOTAL_MISSILES / 2)
#define TEXT_OFFSET 10
#define IMPACTED_OFFSET (DISPLAY_WIDTH / 2)

static uint16_t shot;
static uint16_t impacted;
static missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];
static missile_t *enemy_missiles = &(missiles[0]);
static missile_t *player_missiles = &(missiles[CONFIG_MAX_ENEMY_MISSILES]);
static missile_t *plane_missiles =
    &(missiles[CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLAYER_MISSILES]);
static bool tick_first_half = false;

// Prints or clears the score
void printScore(bool clear) {
  display_setTextColor(clear ? CONFIG_BACKGROUND_COLOR : DISPLAY_WHITE);
  display_setCursor(TEXT_OFFSET, TEXT_OFFSET);
  display_print("Shot: ");
  display_printDecimalInt(shot);

  display_setCursor(IMPACTED_OFFSET, TEXT_OFFSET);
  display_print("Impacted: ");
  display_printDecimalInt(impacted);
}

// Checks to see if there is a collision
bool check_collision(int16_t delta_x, int16_t delta_y, double radius) {
  return ((delta_x * delta_x) + (delta_y * delta_y)) < (radius * radius);
}

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {
  // Blank the screen.
  display_fillScreen(DISPLAY_BLACK);
  // Set and draw the score
  shot = 0;
  impacted = 0;
  printScore(0);

  // Initialize player missiles
  for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
    missile_init_dead(&player_missiles[i]);

  // Initialize enemy missiles
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
    missile_init_dead(&enemy_missiles[i]);

#ifdef LAB8_M3
  // Initalize plane
  plane_init(
      &(missiles[CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLAYER_MISSILES]));
#endif

  // Initialize plane missiles
  for (uint16_t i = 0; i < CONFIG_MAX_PLANE_MISSILES; i++)
    missile_init_dead(&plane_missiles[i]);
}

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {
  // Check for dead enemy missiles and re-initialize
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    // Checking if it is dead, if so re-initialze
    if (missile_is_dead(&enemy_missiles[i])) {
      if (enemy_missiles[i].impacted) {
        printScore(1);
        impacted++;
      }
      missile_init_enemy(&enemy_missiles[i]);
    }
  }

  // Handling touches on the touchscreen
  if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
    // Itterating through all player missiles
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
      // Checking to see if there is a missile that isn't dead. Either way
      // acknoledge the touch
      if (missile_is_dead(&player_missiles[i])) {
        missile_init_player(&player_missiles[i], touchscreen_get_location().x,
                            touchscreen_get_location().y);
        printScore(1);
        shot++;
        break;
      }
    }
    touchscreen_ack_touch();
  }

  // Checking to see if there is a collision
  for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
    // Don't bother checking if the missile isn't exploding
    if (missile_is_dead(&missiles[i]) || missile_is_flying(&missiles[i]))
      continue;
    // Looping though all the enemy missiles
    for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {
      // If the anything other than flying don't bother
      if (!missile_is_flying(&enemy_missiles[j]))
        continue;
      // Only explode if the missile is within the radius of the explosion
      if (!enemy_missiles[j].impacted &&
          check_collision(missiles[i].x_current - enemy_missiles[j].x_current,
                          missiles[i].y_current - enemy_missiles[j].y_current,
                          missiles[i].radius)) {
        missile_trigger_explosion(&enemy_missiles[j]);
      }
    }

    // Checking the plane missile collision
    if (!plane_missiles[0].impacted &&
        check_collision(missiles[i].x_current - plane_missiles[0].x_current,
                        missiles[i].y_current - plane_missiles[0].y_current,
                        missiles[i].radius)) {
      missile_trigger_explosion(&plane_missiles[0]);
    }
#ifdef LAB8_M3
    // Checking the plane collision
    if (check_collision(plane_getXY().x - missiles[i].x_current,
                        plane_getXY().y - missiles[i].y_current,
                        missiles[i].radius)) {
      plane_explode();
    }
#endif
  }

  // Tick all missiles, half each time
  if (tick_first_half)
    // Tick the fist half
    for (uint16_t i = 0; i < HALF_TOTAL_MISSILES; i++)
      missile_tick(&missiles[i]);
  else
    // Tick the second half
    for (uint16_t i = HALF_TOTAL_MISSILES; i < CONFIG_MAX_TOTAL_MISSILES; i++)
      missile_tick(&missiles[i]);
  tick_first_half = !tick_first_half;

#ifdef LAB8_M3
  // Tick plane
  plane_tick();
#endif

  // Draw the score
  printScore(0);
}