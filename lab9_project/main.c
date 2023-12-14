#include <stdio.h>

#include "buttons.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "notes.h"

#define TIMER 15
#define NUMNOTES 4

note_t notes_list[NUMNOTES];
int16_t score;
int16_t total;
int16_t period;

void printScore(bool clear) {
  display_setTextColor(clear ? DISPLAY_BLACK : DISPLAY_WHITE);
  display_setCursor(0, 0);
  display_print("Score: ");
  display_printDecimalInt(score);
  display_print("/");
  display_printDecimalInt(total);
}

void isr2() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);
  if (rand() % 3) {
    for (int i = 0; i < NUMNOTES; i++) {
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
  for (int i = 0; i < NUMNOTES; i++) {
    if ((notes_list[i].state == NOTE_MOVING)) {
      int16_t dist_from_line = notes_list[i].y_current - PLAY_LINE;
      if (buttons_read() == notes_list[i].position &&
          (dist_from_line < NOTE_RADIUS * 1.5) &&
          (dist_from_line > -(NOTE_RADIUS * 1.5))) {
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

int main() {
  // Initalization phase
  buttons_init();
  display_init();
  display_fillScreen(DISPLAY_BLACK);
  for (int i = 0; i < NUMNOTES; i++) {
    notes_list[i].state = NOTE_DEAD;
  }

  // Initialize timer interrupts
  interrupts_init();

  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  intervalTimer_initCountDown(INTERVAL_TIMER_0, 45.0E-3);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_0);

  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, isr2);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, .75);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_1);

  // Main game loop
  while (1)
    ;
}