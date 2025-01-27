#include "notes.h"
#include "display.h"

#include <stdio.h>

// Positions of notes
#define GREEN_POSITION ((DISPLAY_WIDTH * 2) / 7)
#define RED_POSITION ((DISPLAY_WIDTH * 3) / 7)
#define YELLOW_POSITION ((DISPLAY_WIDTH * 4) / 7)
#define BLUE_POSITION ((DISPLAY_WIDTH * 5) / 7)

#define MODULO 0b10000

#define GREEN_B 0b1000
#define RED_B 0b0100
#define YELLOW_B 0b0010
#define BLUE_B 0b0001

// Initialize the plane state machine
// Pass in a pointer to the node struct
void note_init(note_t *note) {
  note->y_current = 0;
  note->position = rand() % MODULO;
  if (!note->position) {
    note->position = 1;
  }
  note->state = NOTE_MOVING;
}

void draw_notes(note_t *note, bool clear) {
  // Draw green
  if (note->position & GREEN_B) {
    display_drawCircle(GREEN_POSITION, note->y_current, NOTE_RADIUS,
                       (clear ? DISPLAY_GREEN : DISPLAY_BLACK));
    display_drawCircle(GREEN_POSITION, note->y_current, NOTE_RADIUS - 1,
                       (clear ? DISPLAY_GREEN : DISPLAY_BLACK));
  }
  // Draw red
  if (note->position & RED_B) {
    display_drawCircle(RED_POSITION, note->y_current, NOTE_RADIUS,
                       (clear ? DISPLAY_RED : DISPLAY_BLACK));
    display_drawCircle(RED_POSITION, note->y_current, NOTE_RADIUS - 1,
                       (clear ? DISPLAY_RED : DISPLAY_BLACK));
  }
  // Draw yellow
  if (note->position & YELLOW_B) {
    display_drawCircle(YELLOW_POSITION, note->y_current, NOTE_RADIUS,
                       (clear ? DISPLAY_YELLOW : DISPLAY_BLACK));
    display_drawCircle(YELLOW_POSITION, note->y_current, NOTE_RADIUS - 1,
                       (clear ? DISPLAY_YELLOW : DISPLAY_BLACK));
  }
  // Draw blue
  if (note->position & BLUE_B) {
    display_drawCircle(BLUE_POSITION, note->y_current, NOTE_RADIUS,
                       (clear ? DISPLAY_BLUE : DISPLAY_BLACK));
    display_drawCircle(BLUE_POSITION, note->y_current, NOTE_RADIUS - 1,
                       (clear ? DISPLAY_BLUE : DISPLAY_BLACK));
  }
}

// State machine tick function
void note_tick(note_t *note) {
  // mealy
  switch (note->state) {
  case NOTE_MOVING:
    // If it is within 10 of the end then delete it
    if (note->y_current + NOTE_RADIUS >= DISPLAY_HEIGHT) {
      draw_notes(note, false);
      note->state = NOTE_DEAD;
    }
    break;
  case NOTE_DEAD:
    break;
  }

  // more
  switch (note->state) {
  case NOTE_MOVING:
    draw_notes(note, false);
    note->y_current += (NOTE_RADIUS / 2);
    draw_notes(note, true);
    break;
  case NOTE_DEAD:
    break;
  }
}

// End the notes
void note_played(note_t *note) {
  note->state = NOTE_DEAD;
  draw_notes(note, false);
}
