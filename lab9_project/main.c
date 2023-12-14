#include <stdio.h>

#include "buttons.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "notes.h"

#define TIMER 15
#define NUMNOTES 4

#define ODDS_OF_EMPTY 4

#define DIST_TO_LINE 1.5

#define GAME_TICK 45.0E-3

#define NOTES_TICK .75

note_t notes_list[NUMNOTES];
int16_t score;
int16_t total;
int16_t period;

// Printing the score or erasing it depending on the mood
void printScore(bool clear) {
  display_setTextColor(clear ? DISPLAY_BLACK : DISPLAY_WHITE);
  display_setCursor(0, 0);
  display_print("Score: ");
  display_printDecimalInt(score);
  display_print("/");
  display_printDecimalInt(total);
}

// Initalize the notes
void isr2() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);
  // Skip one out of every 4 notes
  if (rand() % ODDS_OF_EMPTY) {
    // Looping through notes to see if any are dead
    for (int i = 0; i < NUMNOTES; i++) {
      // Initalizing them if they are dead
      if (notes_list[i].state == NOTE_DEAD) {
        note_init(&(notes_list[i]));
        printScore(true);
        total++;
        printScore(false);
        period = 0;
        break;
      }
    }
  }
}

// Interrupt Function for calling FSM ticks
void isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  // Looping though all the notes
  for (int i = 0; i < NUMNOTES; i++) {
    // Checking to make sure the notes are moving
    if ((notes_list[i].state == NOTE_MOVING)) {
      int16_t dist_from_line = notes_list[i].y_current - PLAY_LINE;
      // Checking too see if the right buttons are pressed at the right time
      if (buttons_read() == notes_list[i].position &&
          (dist_from_line < NOTE_RADIUS * DIST_TO_LINE) &&
          (dist_from_line > -(NOTE_RADIUS * DIST_TO_LINE))) {
        note_played(&(notes_list[i]));
        printScore(true);
        score++;
        printScore(false);
      }
      note_tick(&(notes_list[i]));
    }
  }
  display_drawLine(0, PLAY_LINE, DISPLAY_WIDTH, PLAY_LINE, DISPLAY_GRAY);
  period++;
}

// My main function
int main() {
  // Initalization phase
  buttons_init();
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  // Initalizing the notes_list
  for (int i = 0; i < NUMNOTES; i++) {
    notes_list[i].state = NOTE_DEAD;
  }

  // Initialize timer interrupts
  interrupts_init();
  // Regulat timer
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  intervalTimer_initCountDown(INTERVAL_TIMER_0, GAME_TICK);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);
  // Initalizing the notes when it is time
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, isr2);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, NOTES_TICK);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_1);

  // Main game loop
  while (1)
    ;
}