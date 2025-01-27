#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_TOUCHSCREEN_TIMER_PERIOD 10.0E-3
#define CONFIG_GAME_TIMER_PERIOD 45.0E-3

#define CONFIG_MAX_ENEMY_MISSILES 7
#define CONFIG_MAX_PLAYER_MISSILES 4
#define CONFIG_MAX_PLANE_MISSILES 1
#define CONFIG_MAX_TOTAL_MISSILES                                              \
  (CONFIG_MAX_ENEMY_MISSILES + CONFIG_MAX_PLAYER_MISSILES +                    \
   CONFIG_MAX_PLANE_MISSILES)

// Speed of enemby missile
#define CONFIG_ENEMY_MISSILE_DISTANCE_PER_SECOND 35
#define CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK                                 \
  (CONFIG_ENEMY_MISSILE_DISTANCE_PER_SECOND * CONFIG_GAME_TIMER_PERIOD * 2)

// Speed of player missile
#define CONFIG_PLAYER_MISSILE_DISTANCE_PER_SECOND 350
#define CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK                                \
  (CONFIG_PLAYER_MISSILE_DISTANCE_PER_SECOND * CONFIG_GAME_TIMER_PERIOD * 2)

// How fast explosion radius increases/decreases per second
#define CONFIG_EXPLOSION_RADIUS_CHANGE_PER_SECOND 30
#define CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK                                \
  (CONFIG_EXPLOSION_RADIUS_CHANGE_PER_SECOND * CONFIG_GAME_TIMER_PERIOD * 2)

// Speed of plane
#define CONFIG_PLANE_DISTANCE_PER_SECOND 40
#define CONFIG_PLANE_DISTANCE_PER_TICK                                         \
  (CONFIG_PLANE_DISTANCE_PER_SECOND * CONFIG_GAME_TIMER_PERIOD)

#define CONFIG_COLOR_ENEMY DISPLAY_RED
#define CONFIG_COLOR_PLAYER DISPLAY_GREEN
#define CONFIG_COLOR_PLANE DISPLAY_BLUE

#define CONFIG_BACKGROUND_COLOR DISPLAY_BLACK

#define CONFIG_EXPLOSION_MAX_RADIUS 25

#endif /* CONFIG_H */
