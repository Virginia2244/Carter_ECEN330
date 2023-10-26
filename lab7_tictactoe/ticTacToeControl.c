#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToe.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"

#include <stdint.h>
#include <stdio.h>

#define WAIT_TICKS 60
#define WELCOME_MESSAGE "WELCOME!"
#define TEXT_SIZE 6
#define TEXT_LOCATION_X 20
#define TEXT_LOCATION_Y 100
#define INIT_MOVE_X 2
#define INIT_MOVE_Y 2

// States for my State Machine
typedef enum {
  INIT,
  MENUE,
  WAIT,
  X_TURN,
  O_TURN,
  CHECK_X,
  CHECK_O,
  END_GAME
} game_controller_t;

static game_controller_t game_control;
static game_controller_t last_state;
static uint8_t wait_time;
static tictactoe_location_t o_move;
static tictactoe_location_t x_move;
static tictactoe_board_t board;
static bool player_is_x;
static bool first_blood;

// Checking to make sure the spot I want to play is free
bool check_touch_free(tictactoe_location_t *touch_point) {
  // Only run if the touchscreen has been touched
  if (touchscreen_get_status()) {
    *touch_point =
        ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());
    // If the square that has been touched is empty acknoledge the touch and
    // return true
    if (board.squares[touch_point->row][touch_point->column] ==
        MINIMAX_EMPTY_SQUARE) {
      touchscreen_ack_touch();
      return true;
    } else {
      touchscreen_ack_touch();
      return false;
    }
  }
  return false;
}

// If it is the first move of the game use the hardcoded move, otherwise use
// minimax to determine the move
tictactoe_location_t ticTacToeControlComputeNextMove() {
  // Only use the hardcoded move if it is the first move of the game
  if (first_blood) {
    tictactoe_location_t point;
    point.column = INIT_MOVE_X;
    point.row = INIT_MOVE_Y;
    return point;
  } else {
    return minimax_computeNextMove(&board, true);
  }
}

// Clear the screen to prepare for the next game
void clear_screen() {
  tictactoe_location_t move;
  // Itterating over rows
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    move.row = i;
    // Itterating over columns
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      move.column = j;
      // Erase the X or the O depending on which one is currently drawn
      if (board.squares[i][j] == MINIMAX_O_SQUARE) {
        ticTacToeDisplay_drawO(move, true);
      } else if (board.squares[i][j] == MINIMAX_X_SQUARE) {
        ticTacToeDisplay_drawX(move, true);
      }
      board.squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Prints the board state formatted nice
void printControlBoard(tictactoe_board_t *Board) {
  printf("\nBoard State:\n");
  // Itterate through rows
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    printf("----+---+----\n");
    // Itterate through collums
    for (uint8_t j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      // Prints x o or ' ' when it should
      printf("| %c ", Board->squares[i][j] == MINIMAX_X_SQUARE   ? 'x'
                      : Board->squares[i][j] == MINIMAX_O_SQUARE ? 'o'
                                                                 : ' ');
    }
    printf("|\n");
  }
  printf("----+---+----\n");
}

// Prints the current state
void print_states() {
  printf("\n\nCurrent State: ");
  // Prints the name of the state depending on the state
  switch (game_control) {
  case INIT:
    printf("INIT");
    break;
  case MENUE:
    printf("MENUE");
    break;
  case WAIT:
    printf("WAIT");
    break;
  case X_TURN:
    printf("X_TURN");
    break;
  case O_TURN:
    printf("O_TURN");
    break;
  case CHECK_O:
    printf("CHECK_O");
    break;
  case CHECK_X:
    printf("CHECK_X");
    break;
  case END_GAME:
    printf("END_GAME");
    break;
  }
  printf("\t");
  printf("\n");
  // Print the move the computer is using
  if (player_is_x) {
    // Only do it if it is the right time
    if (game_control == CHECK_O) {
      printf("O's move: (%i,%i)", o_move.row, o_move.column);
    }
  } else {
    // Only do it if it is the right time
    if (game_control == CHECK_X) {
      printf("X's move: (%i,%i)", x_move.row, x_move.column);
    }
  }
}

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick() {

  // Mealy outputs and state managment
  switch (game_control) {
  case INIT:
    game_control = MENUE;
    player_is_x = true;
    display_setTextSize(TEXT_SIZE);
    display_setCursor(TEXT_LOCATION_X, TEXT_LOCATION_Y);
    display_setTextColor(DISPLAY_WHITE);
    display_println(WELCOME_MESSAGE);
    break;
  case MENUE:
    // Display the menue for 3 seconds before moving on
    if (wait_time >= WAIT_TICKS) {
      display_setCursor(TEXT_LOCATION_X, TEXT_LOCATION_Y);
      display_setTextColor(DISPLAY_DARK_BLUE);
      display_println(WELCOME_MESSAGE);
      ticTacToeDisplay_init();
      wait_time = 0;
      game_control = WAIT;
    }
    break;
  case WAIT:
    // Wait for the player to play, but if they don't for 3 seconds then let the
    // computer play (whoever plays first is X)
    if (touchscreen_get_status()) {
      game_control = X_TURN;
      player_is_x = true;
      wait_time = 0;
    } else if (wait_time >= WAIT_TICKS) {
      game_control = X_TURN;
      player_is_x = false;
      wait_time = 0;
    }
    break;
  case X_TURN:
    // If the player is X then do the player stuff, otherwise let the computer
    // decide what to play
    if (player_is_x) {
      // Wait for the player to make a valid move, draw it to the screen, then
      // make the move and move on
      if (check_touch_free(&x_move)) {
        ticTacToeDisplay_drawX(x_move, false);
        board.squares[x_move.row][x_move.column] = MINIMAX_X_SQUARE;
        touchscreen_ack_touch();
        game_control = CHECK_X;
        first_blood = false;
      }
    } else {
      x_move = ticTacToeControlComputeNextMove();
      ticTacToeDisplay_drawX(x_move, false);
      board.squares[x_move.row][x_move.column] = MINIMAX_X_SQUARE;
      game_control = CHECK_X;
      first_blood = false;
    }
    break;
  case O_TURN:
    // If the player is O then do the player stuff, otherwise let the computer
    // decide what to play
    if (player_is_x) {
      o_move = minimax_computeNextMove(&board, true);
      ticTacToeDisplay_drawO(o_move, false);
      board.squares[o_move.row][o_move.column] = MINIMAX_O_SQUARE;
      game_control = CHECK_O;
    } else {
      // Wait for the player to make a valid move, draw it to the screen, then
      // make the move and move on
      if (check_touch_free(&o_move)) {
        ticTacToeDisplay_drawO(o_move, false);
        board.squares[o_move.row][o_move.column] = MINIMAX_O_SQUARE;
        touchscreen_ack_touch();
        game_control = CHECK_O;
      }
    }
    break;
  case CHECK_X:
    // Check to see if the game is over or not
    if (minimax_computeBoardScore(&board, true) != MINIMAX_NOT_ENDGAME) {
      game_control = END_GAME;
    } else {
      game_control = O_TURN;
    }
    break;
  case CHECK_O:
    // Check to see if the game is over or not
    if (minimax_computeBoardScore(&board, false) != MINIMAX_NOT_ENDGAME) {
      game_control = END_GAME;
    } else {
      game_control = X_TURN;
    }
    break;
  case END_GAME:
    // If the right button is pressed then restart the game
    if (buttons_read() & 1) {
      game_control = WAIT;
      wait_time = 0;
      clear_screen();
      touchscreen_ack_touch();
    }
    break;
  }

  // More outputs
  switch (game_control) {
  case INIT:
    break;
  case MENUE:
    wait_time++;
    break;
  case WAIT:
    first_blood = true;
    wait_time++;
    break;
  case X_TURN:
    break;
  case O_TURN:
    break;
  case CHECK_O:
    break;
  case CHECK_X:
    break;
  case END_GAME:
    break;
  }
  // If the state has changed print the debug statements
  if (last_state != game_control) {
    last_state = game_control;
    print_states();
    printControlBoard(&board);
  }
}

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s) {
  game_control = INIT;
  last_state = END_GAME;
  wait_time = 0;

  // Initalizing board to 0
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Still initalizing board to 0
    for (int j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      board.squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
  buttons_init();
  // Initializing the screen
  display_init();
  display_fillScreen(DISPLAY_DARK_BLUE);
}