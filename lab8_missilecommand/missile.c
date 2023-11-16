#include "missile.h"
#include "config.h"
#include "display.h"

#include <math.h>
#include <stdlib.h>

#define SQRUARE_ROOT 2
// Math for the starting locations of player missiles
#define FIRST_THIRD (DISPLAY_WIDTH / 3)
#define SECOND_THIRD (DISPLAY_WIDTH * 2 / 3)
#define FIRST_BASE (DISPLAY_WIDTH / 6)
#define SECOND_BASE ((DISPLAY_WIDTH / 3) + (DISPLAY_WIDTH / 6))
#define THIRD_BASE ((DISPLAY_WIDTH * 2 / 3) + (DISPLAY_WIDTH / 6))

// Math for starting location of enemy missiles
#define ENEMY_MISSILE_Y_ZONE DISPLAY_HEIGHT / 5

// Math for missile speeds
#define MISSILE_SPEED                                                          \
  ((missile->type == MISSILE_TYPE_PLAYER)                                      \
       ? CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK                               \
       : CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK)

// Missile colors
#define MISSILE_COLOR                                                          \
  (missile->type == MISSILE_TYPE_PLAYER                                        \
       ? CONFIG_COLOR_PLAYER                                                   \
       : (missile->type == MISSILE_TYPE_ENEMY ? CONFIG_COLOR_ENEMY             \
                                              : CONFIG_COLOR_PLANE))

typedef enum {
  MISSILE_STATE_DEAD,
  MISSILE_STATE_EXPLODING_GROWING,
  MISSILE_STATE_EXPLODING_SHRINKING,
  MISSILE_STATE_FLYING
} missile_state_t;

////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function,
// our missile will have different initializers depending on the
// missile type.

// All the things that every missile will need
void missile_init_all(missile_t *missile) {
  missile->length = 0;
  missile->explode_me = false;
  missile->total_length =
      sqrt(pow((missile->x_dest - missile->x_origin), SQRUARE_ROOT) +
           pow((missile->y_dest - missile->y_origin), SQRUARE_ROOT));
  missile->x_current = missile->x_dest;
  missile->y_current = missile->y_dest;
  missile->impacted = false;
  // Stuff they didn't tell me to do
  missile->radius = 0;
}

// Initialize the missile as a dead missile.  This is useful at
// the start of the game to ensure that player and plane missiles
// aren't moving before they should.
void missile_init_dead(missile_t *missile) {
  missile->currentState = MISSILE_STATE_DEAD;
  missile_init_all(missile);
}

// Initialize the missile as an enemy missile.  This will
// randomly choose the origin and destination of the missile. The
// origin should be somewhere near the top of the screen, and the
// destination should be the very bottom of the screen.
void missile_init_enemy(missile_t *missile) {
  missile->type = MISSILE_TYPE_ENEMY;
  missile->x_origin = rand() % DISPLAY_WIDTH;
  missile->y_origin = rand() % ENEMY_MISSILE_Y_ZONE;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;
  missile->currentState = MISSILE_STATE_FLYING;
  missile_init_all(missile);
}

// Initialize the missile as a player missile.  This function
// takes an (x, y) destination of the missile (where the user
// touched on the touchscreen).  The origin should be the closest
// "firing location" to the destination (there are three firing
// locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {
  missile->type = MISSILE_TYPE_PLAYER;
  missile->x_dest = x_dest;
  missile->y_dest = y_dest;
  missile->y_origin = DISPLAY_HEIGHT;
  // Making the start the nearest base to the the touched point
  if (x_dest <= FIRST_THIRD) {
    missile->x_origin = FIRST_BASE;
  } else if (x_dest <= SECOND_THIRD) {
    missile->x_origin = SECOND_BASE;
  } else {
    missile->x_origin = THIRD_BASE;
  }
  missile->currentState = MISSILE_STATE_FLYING;

  missile_init_all(missile);
}

// Initialize the missile as a plane missile.  This function
// takes an (x, y) location of the plane which will be used as
// the origin.  The destination can be randomly chosed along the
// bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {
  missile->type = MISSILE_TYPE_PLANE;
  missile->x_origin = plane_x;
  missile->y_origin = plane_y;
  missile->x_dest = rand() % DISPLAY_WIDTH;
  missile->y_dest = DISPLAY_HEIGHT;
  missile->currentState = MISSILE_STATE_FLYING;
  missile_init_all(missile);
}

////////// State Machine TICK Function //////////
void missile_tick(missile_t *missile) {
  // State changes and mealy outputs
  switch (missile->currentState) {
  case MISSILE_STATE_DEAD:
    break;

  case MISSILE_STATE_EXPLODING_GROWING:
    // If the missile is at the max size then start shrinking it, otherwise
    // increase the radius
    if (missile->radius == CONFIG_EXPLOSION_MAX_RADIUS) {
      missile->currentState = MISSILE_STATE_EXPLODING_SHRINKING;
    }
    break;

  case MISSILE_STATE_EXPLODING_SHRINKING:
    // Calculate the new radius, if it is 0 then kill the missile
    if ((missile->radius - CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK) <= 0) {
      // Clear the old explosion
      display_fillCircle(missile->x_current, missile->y_current,
                         missile->radius, DISPLAY_BLACK);
      missile->radius = 0;
      missile->currentState = MISSILE_STATE_DEAD;
    }
    break;

  case MISSILE_STATE_FLYING:
    // If it is time to explode or kill the missile do so, otherwise
    if (missile->explode_me) {
      // Erase the line
      display_drawLine(missile->x_current, missile->y_current,
                       missile->x_origin, missile->y_origin, DISPLAY_BLACK);
      missile->currentState = MISSILE_STATE_EXPLODING_GROWING;
    } else if (missile->length == missile->total_length) {
      // Erase the line
      display_drawLine(missile->x_current, missile->y_current,
                       missile->x_origin, missile->y_origin, DISPLAY_BLACK);
      // If it is a player missile then explode, otherwise kill the missile
      missile->currentState = (missile->type == MISSILE_TYPE_PLAYER)
                                  ? MISSILE_STATE_EXPLODING_GROWING
                                  : MISSILE_STATE_DEAD;
      missile->impacted = true;
    }
    break;
  }

  // More outputs
  switch (missile->currentState) {
  case MISSILE_STATE_DEAD:
    break;

  case MISSILE_STATE_EXPLODING_GROWING:
    // increase the radius
    missile->radius =
        ((missile->radius + CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK) <=
         CONFIG_EXPLOSION_MAX_RADIUS)
            ? (missile->radius + CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK)
            : CONFIG_EXPLOSION_MAX_RADIUS;
    // Draw the explosion
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       MISSILE_COLOR);
    break;

  case MISSILE_STATE_EXPLODING_SHRINKING:
    // Clear the old explosion
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       DISPLAY_BLACK);
    // Decrease the radius
    missile->radius -= CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK;
    // Draw the new explosion
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       MISSILE_COLOR);
    break;

  case MISSILE_STATE_FLYING:
    // Erase the previous line
    display_drawLine(missile->x_current, missile->y_current, missile->x_origin,
                     missile->y_origin, DISPLAY_BLACK);
    // The missile length increases until it hits max length
    missile->length = (missile->length + MISSILE_SPEED < missile->total_length)
                          ? missile->length + MISSILE_SPEED
                          : missile->total_length;
    // Calculate what percent of the total distance the missile has traveled
    double percentage =
        ((double)(missile->length) / (double)missile->total_length);
    // Find the x and y positions of the missile
    missile->x_current =
        missile->x_origin + percentage * (missile->x_dest - missile->x_origin);
    missile->y_current =
        missile->y_origin + percentage * (missile->y_dest - missile->y_origin);
    // Draw the new line
    display_drawLine(missile->x_current, missile->y_current, missile->x_origin,
                     missile->y_origin, MISSILE_COLOR);
    break;
  }
}

// Return whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  return missile->currentState == MISSILE_STATE_DEAD;
}

// Return whether the given missile is exploding.  This is needed
// when detecting whether a missile hits another exploding
// missile.
bool missile_is_exploding(missile_t *missile) {
  return (missile->currentState == MISSILE_STATE_EXPLODING_GROWING) ||
         (missile->currentState == MISSILE_STATE_EXPLODING_SHRINKING);
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {
  return missile->currentState == MISSILE_STATE_FLYING;
}

// Used to indicate that a flying missile should be detonated.
// This occurs when an enemy or plane missile is located within
// an explosion zone.
void missile_trigger_explosion(missile_t *missile) {
  missile->explode_me = true;
}