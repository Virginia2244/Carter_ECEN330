#include "minimax.h"
#include <stdint.h>
#include <stdio.h>

static int8_t depth;
static tictactoe_location_t location;

// Prints the board state formatted nice
void printBoard(tictactoe_board_t *Board) {
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

// The recursive funciton that looks at every possible itteration of the
// board state and sees what the score is going to be
int8_t minimax_recusive(tictactoe_board_t *board, bool is_Xs_turn) {
  // Updates depth
  depth++;
  // Init variables
  int8_t bestScore =
      is_Xs_turn ? MINIMAX_O_WINNING_SCORE : MINIMAX_X_WINNING_SCORE;
  tictactoe_location_t localLocation;
  int8_t score = 0;

  // Itterate through rows
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Itterate through collums
    for (uint8_t j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      // Avoid the ones that are full
      if (board->squares[i][j] != MINIMAX_EMPTY_SQUARE) {
        continue;
      }
      // Try a move
      board->squares[i][j] = is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

      // Get the score
      score = minimax_computeBoardScore(board, is_Xs_turn);
      // If the score is -1 then recurse
      if (score == MINIMAX_NOT_ENDGAME) {
        score = minimax_recusive(board, !is_Xs_turn);
      } else {
        score = score + (score ? (score > 0 ? -depth : depth) : 0);
      }
      // Get the correct score depending on if it is X's or O's turn
      if (is_Xs_turn) {
        // Replace bestScore if score is greater and updates the location
        if (bestScore < score) {
          bestScore = score;
          localLocation.column = j;
          localLocation.row = i;
        }
      } else {
        // Replace bestScore if score is less than and updates the location
        if (bestScore > score) {
          bestScore = score;
          localLocation.column = j;
          localLocation.row = i;
        }
      }
      // Undo the move on the board
      board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
    }
  }
  // Update the global location
  location.column = localLocation.column;
  location.row = localLocation.row;
  // Updates depth
  depth--;
  return bestScore; // + (bestScore ? (bestScore > 0 ? -1 : 1) : 0);
}

// This routine is not recursive but will invoke the recursive minimax
// function. You will call this function from the controlling state machine
// that you will implement in a later milestone. It computes the row and
// column of the next move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this
// function as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  depth = -1;
  // printBoard(board);
  // printf("Final Score: %i\n", minimax_recusive(board, is_Xs_turn));
  minimax_recusive(board, is_Xs_turn);
  return location;
}

// Checks the rows of the tick tac toe board for a win state
bool checkRow(tictactoe_board_t *board, bool is_Xs_turn, uint8_t row) {
  tictactoe_square_state_t check =
      is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
  // Itterates through the collums
  for (int i = 0; i < TICTACTOE_BOARD_COLUMNS; i++) {
    // Checks to see if each value is the right one
    if (board->squares[row][i] != check) {
      return false;
    }
  }
  return true;
}

// Checks the columns of the tick tac toe board for a win state
bool checkColumn(tictactoe_board_t *board, bool is_Xs_turn, uint8_t col) {
  tictactoe_square_state_t check =
      is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
  // Itterates through the rows
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Checks to see if each value is the right one
    if (board->squares[i][col] != check) {
      return false;
    }
  }
  return true;
}

// Checks the diagnals for a win state
bool checkDiagnals(tictactoe_board_t *board, bool is_Xs_turn) {
  tictactoe_square_state_t check =
      is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
  bool test1 = 1;
  // Checks the \ diagnal
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Checks to see if each value is the right one
    if (board->squares[i][i] != check) {
      test1 = 0;
    }
  }
  // If the previous test passed then the board is clear
  if (test1) {
    return true;
  }
  // Chekcs the / diagnal
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Checks to see if each value is the right one
    if (board->squares[i][TICTACTOE_BOARD_ROWS - i - 1] != check) {
      return false;
    }
  }
  return true;
}

// Looks at the entire board to see if there is an empty space
bool boardFull(tictactoe_board_t *board) {
  // Itterate through rows
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Itterate through collums
    for (uint8_t j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      // Checks each item to see if it is empty
      if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) {
        return false;
      }
    }
  }
  return true;
}

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {
  bool win = 0;
  // check Rows and collums
  for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    if (checkRow(board, is_Xs_turn, i) || checkColumn(board, is_Xs_turn, i)) {
      win = 1;
    }
  }
  // check diagnals
  if (checkDiagnals(board, is_Xs_turn)) {
    win = 1;
  }

  // If the game has been won return the score, if not see if it's a draw
  if (win) {
    return (is_Xs_turn ? MINIMAX_X_WINNING_SCORE : MINIMAX_O_WINNING_SCORE);
  } else if (boardFull(board)) {
    return MINIMAX_DRAW_SCORE;
  } else {
    return MINIMAX_NOT_ENDGAME;
  }
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {
  // Itterate through rows
  for (uint8_t i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
    // Itterate through collums
    for (uint8_t j = 0; j < TICTACTOE_BOARD_COLUMNS; j++) {
      board->squares[i][j] == MINIMAX_EMPTY_SQUARE;
    }
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  return score != MINIMAX_NOT_ENDGAME;
}