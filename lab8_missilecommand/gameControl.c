#include "gameControl.h"
#include "config.h"
#include "missile.h"
#include "touchscreen.h"

#include <stdio.h>

static missile_t missiles[CONFIG_MAX_TOTAL_MISSILES];
static missile_t *enemy_missiles = &(missiles[0]);
static missile_t *player_missiles = &(missiles[CONFIG_MAX_ENEMY_MISSILES]);
static missile_t *plane_missiles =
    &(missiles[CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLAYER_MISSILES]);
static bool tick_first_half = false;
// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {
  // Blank the screen.
  display_fillScreen(DISPLAY_BLACK);

  // Initialize player missiles
  for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
    missile_init_dead(&player_missiles[i]);

  // Initialize enemy missiles
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
    missile_init_dead(&enemy_missiles[i]);

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
      missile_init_enemy(&enemy_missiles[i]);
    }
  }

  // Handling touches on the touchscreen
  if (touchscreen_get_status() != TOUCHSCREEN_IDLE) {
    // Itterating through all player missiles
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
      // Checking to see if there is a missile that isn't dead. Either way
      // acknoledge the touch
      if (missile_is_dead(&player_missiles[i])) {
        missile_init_player(&player_missiles[i], touchscreen_get_location().x,
                            touchscreen_get_location().y);
        touchscreen_ack_touch();
        break;
      }
    }
  }

  // Checking to see if there is a collision
  for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
    // Don't bother checking if the missile isn't exploding
    if (missile_is_dead(&player_missiles[i]) ||
        missile_is_flying(&player_missiles[i]))
      continue;
    // Looping though all the enemy missiles
    for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {
      // If the missile is dead don't bother
      if (missile_is_dead(&enemy_missiles[i]))
        continue;
      int16_t delta_x =
          player_missiles[i].x_current - enemy_missiles[j].x_current;
      int16_t delta_y =
          player_missiles[i].y_current - enemy_missiles[j].y_current;
      // Only explode if the missile is within the radius of the explosion
      if (((delta_x * delta_x) + (delta_y * delta_y)) <
          (player_missiles[i].radius * player_missiles[i].radius)) {
        missile_trigger_explosion(&enemy_missiles[j]);
      }
    }
  }

  // Tick all missiles, half each time
  if (tick_first_half)
    // Tick the fist half
    for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES / 2; i++)
      missile_tick(&missiles[i]);
  else
    // Tick the second half
    for (uint16_t i = CONFIG_MAX_TOTAL_MISSILES / 2;
         i < CONFIG_MAX_TOTAL_MISSILES; i++)
      missile_tick(&missiles[i]);
  tick_first_half = !tick_first_half;
}