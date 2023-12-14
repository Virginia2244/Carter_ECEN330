#ifndef NOTES
#define NOTES

#include <stdint.h>

#define PLAY_LINE ((DISPLAY_HEIGHT * 4) / 5)
#define NOTE_RADIUS 10

typedef enum { NOTE_DEAD, NOTE_MOVING } node_state_t;

/* This struct contains all information about a missile */
typedef struct {
  int16_t y_current;
  uint8_t position;
  node_state_t state;
} note_t;

// Initialize the plane state machine
// Pass in a pointer to the node struct
void note_init(note_t *note);

// State machine tick function
void note_tick(note_t *note);

// End the notes
void note_played(note_t *note);

#endif