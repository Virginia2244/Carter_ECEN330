#include "plane.h"
#include "config.h"

#define PLANE_Y_START (DISPLAY_HEIGHT / 6)
#define PLANE_DIAMETER 25
#define PLANE_RADIUS (PLANE_DIAMETER * 3 / 4)
#define PLANE_HEIGHT 7
#define PLANE_RAND_TIME_DEAD (((rand() % 100) / 20) / CONFIG_GAME_TIMER_PERIOD)

typedef enum {
  PLANE_STATE_DEAD,
  PLANE_STATE_FLYING_LOADED,
  PLANE_STATE_FLYING_UNLOADED
} plane_state_t;

static plane_state_t plane;
static display_point_t plane_location;
static missile_t *global_plane_missile;
static uint16_t plane_launch_x_cords;
static uint16_t revive_time;
static uint16_t time_dead;

void move_plane() {
  plane_location.x -=
      CONFIG_PLANE_DISTANCE_PER_TICK; // TODO: impliment this better
}

void draw_plane(bool erase) {
  if (erase) {
    display_fillTriangle(
        plane_location.x, plane_location.y, plane_location.x + PLANE_DIAMETER,
        plane_location.y - PLANE_HEIGHT, plane_location.x + PLANE_DIAMETER,
        plane_location.y + PLANE_HEIGHT, CONFIG_BACKGROUND_COLOR);
  } else {
    display_fillTriangle(
        plane_location.x, plane_location.y, plane_location.x + PLANE_DIAMETER,
        plane_location.y - PLANE_HEIGHT, plane_location.x + PLANE_DIAMETER,
        plane_location.y + PLANE_HEIGHT, DISPLAY_WHITE);
    display_fillTriangle(
        plane_location.x + PLANE_RADIUS, plane_location.y,
        plane_location.x + PLANE_DIAMETER, plane_location.y - PLANE_HEIGHT + 1,
        plane_location.x + PLANE_DIAMETER, plane_location.y + PLANE_HEIGHT - 1,
        CONFIG_BACKGROUND_COLOR);
  }
}

// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {
  plane = PLANE_STATE_DEAD;
  plane_location.x = DISPLAY_WIDTH;
  plane_location.y = PLANE_Y_START;
  global_plane_missile = plane_missile;
  plane_launch_x_cords = rand() % DISPLAY_WIDTH;
  revive_time = PLANE_RAND_TIME_DEAD;
  time_dead = 0;
}

// State machine tick function
void plane_tick() {

  // States and mealy outputs
  switch (plane) {
  case PLANE_STATE_DEAD:
    if (time_dead >= revive_time) {
      plane = PLANE_STATE_FLYING_LOADED;
      plane_launch_x_cords = rand() % DISPLAY_WIDTH;
      revive_time = PLANE_RAND_TIME_DEAD;
      plane_location.x = DISPLAY_WIDTH;
      plane_location.y = PLANE_Y_START;
      time_dead = 0;
    }
    break;

  case PLANE_STATE_FLYING_LOADED:
    if ((plane_location.x <= plane_launch_x_cords) &&
        (missile_is_dead(global_plane_missile))) {
      missile_init_plane(global_plane_missile, plane_location.x,
                         plane_location.y);
      plane = PLANE_STATE_FLYING_UNLOADED;
    }
    break;

  case PLANE_STATE_FLYING_UNLOADED:
    if (plane_location.x <= 0) {
      draw_plane(1);
      plane = PLANE_STATE_DEAD;
    }
    break;
  }

  // More outputs
  switch (plane) {
  case PLANE_STATE_DEAD:
    time_dead++;
    break;

  case PLANE_STATE_FLYING_LOADED:
  case PLANE_STATE_FLYING_UNLOADED:
    draw_plane(1);
    move_plane();
    draw_plane(0);
    break;
  }
}

// Trigger the plane to expode
void plane_explode() {
  plane = PLANE_STATE_DEAD;
  draw_plane(1);
  plane_location.x = DISPLAY_WIDTH;
  plane_location.y = PLANE_Y_START;
}

// Get the XY location of the plane
display_point_t plane_getXY() { return plane_location; }