#include "gameControl.h"
#include "config.h"
#include "missile.h"
#include "touchscreen.h"

#include <stdio.h>

static missile_t player_missiles[CONFIG_MAX_PLAYER_MISSILES];
static missile_t enemy_missiles[CONFIG_MAX_ENEMY_MISSILES];
static missile_t plane_missiles[CONFIG_MAX_PLANE_MISSILES];

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {
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
    if (missile_is_dead(&enemy_missiles[i])) {
      missile_init_enemy(&enemy_missiles[i]);
    }
  }

  if (touchscreen_get_status() != TOUCHSCREEN_IDLE) {
    for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
      if (missile_is_dead(&player_missiles[i])) {
        missile_init_player(&player_missiles[i], touchscreen_get_location().x,
                            touchscreen_get_location().y);
        touchscreen_ack_touch();
        break;
      }
    }
  }

  for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
    if (missile_is_dead(&player_missiles[i]) ||
        missile_is_flying(&player_missiles[i]))
      continue;
    for (uint16_t j = 0; j < CONFIG_MAX_ENEMY_MISSILES; j++) {
      if (missile_is_dead(&enemy_missiles[i]))
        continue;
      int16_t delta_x =
          player_missiles[i].x_current - enemy_missiles[j].x_current;
      int16_t delta_y =
          player_missiles[i].y_current - enemy_missiles[j].y_current;
      if (((delta_x * delta_x) + (delta_y * delta_y)) <
          (player_missiles[i].radius * player_missiles[i].radius)) {
        missile_trigger_explosion(&enemy_missiles[j]);
      }
    }
  }

  // Tick player missiles
  for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++)
    missile_tick(&player_missiles[i]);

  // Tick enemy missiles
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++)
    missile_tick(&enemy_missiles[i]);

  // Tick plane missiles
  for (uint16_t i = 0; i < CONFIG_MAX_PLANE_MISSILES; i++)
    missile_tick(&plane_missiles[i]);
}