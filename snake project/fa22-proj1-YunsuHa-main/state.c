#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  game_state_t *default_state = malloc(sizeof(game_state_t));
  default_state->num_rows = 18;
  default_state->num_snakes = 1;

  snake_t *default_snake = malloc(sizeof(snake_t));
  default_snake->tail_row = 2;
  default_snake->tail_col = 2;
  default_snake->head_row = 2;
  default_snake->head_col = 4;
  default_snake->live = true;
  default_state->snakes = default_snake;
 
  char** board_elements;
  board_elements = malloc(sizeof(*board_elements) * 18);
  for (int i = 0 ; i < 18; i++) {
	  board_elements[i] = malloc(sizeof(**board_elements) * 21);
  }

  strcpy(board_elements[0], "####################");
  strcpy(board_elements[1], "#                  #");
  strcpy(board_elements[2], "# d>D    *         #");
  for (int i = 3; i < 17;i++) {
	  strcpy(board_elements[i], board_elements[1]);
  }
  strcpy(board_elements[17], board_elements[0]); 
  default_state->board = board_elements;
  return default_state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function
  free(state->snakes);
  for (int i = 0; i < state->num_rows; i++) {
	free((state->board)[i]);
  }
  free(state->board);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
  for (int i = 0; i < state->num_rows; i++) {
	 fprintf(fp, "%s\n", state->board[i]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (strchr("wasd", c) != NULL) {
	return true;
  } else {
	  return false;
  }
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (strchr("WASDX", c) != NULL) {
  	return true;
  } else {
	  return false;
  }
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (strchr("wasd^<v>WASDx", c) != NULL) {
	return true;
  } else {
	  return false;
  }
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
	  return 'w';
  } else if (c == '<') {
	  return 'a';
  } else if (c == 'v') {
	  return 's';
  } else if (c == '>') {
	  return 'd';
  } else {
	  return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
	  return '^';
  } else if (c == 'A') {
	  return '<';
  } else if (c == 'S') {
	  return 'v';
  } else if (c == 'D') {
	  return '>';
  } else {
	  return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
	  return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
	  return cur_row - 1;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
	  return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
	  return cur_col - 1;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t curr_snake = state->snakes[snum];
  unsigned int curr_row = curr_snake.head_row;
  unsigned int curr_col = curr_snake.head_col;
  char head_pos = get_board_at(state, curr_row, curr_col);
  return get_board_at(state, get_next_row(curr_row, head_pos), get_next_col(curr_col, head_pos));
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t curr_snake = state->snakes[snum];
  unsigned int curr_row = curr_snake.head_row;
  unsigned int curr_col = curr_snake.head_col;
  char head_pos = get_board_at(state, curr_row, curr_col);
  set_board_at(state, get_next_row(curr_row, head_pos), get_next_col(curr_col, head_pos), head_pos);
  set_board_at(state, curr_row, curr_col, head_to_body(head_pos));

  state->snakes[snum].head_row = get_next_row(curr_row, head_pos);
  state->snakes[snum].head_col = get_next_col(curr_col, head_pos);
  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t curr_snake = state->snakes[snum];
  unsigned int curr_row = curr_snake.tail_row;
  unsigned int curr_col = curr_snake.tail_col;
  char tail_pos = get_board_at(state, curr_row, curr_col);
  unsigned int next_row = get_next_row(curr_row, tail_pos);
  unsigned int next_col = get_next_col(curr_col, tail_pos);
  char body = get_board_at(state, next_row, next_col);
  set_board_at(state, next_row, next_col, body_to_tail(body));
  set_board_at(state, curr_row, curr_col, ' ');

  state->snakes[snum].tail_row = next_row;
  state->snakes[snum].tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
	snake_t curr_snake = state->snakes[i];
	unsigned int curr_row = curr_snake.head_row;
	unsigned int curr_col = curr_snake.head_col;
	char next_pos = next_square(state, i);
	if (next_pos == '#' || is_snake(next_pos)) {
		state->snakes[i].live = false;
		set_board_at(state, curr_row, curr_col, 'x');
	} else if (next_pos == 	'*') {
		update_head(state, i);
		add_food(state);
	} else {
		update_head(state, i);
		update_tail(state,i);
	}
  }	
  return;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
  game_state_t *curr_state = malloc(sizeof(game_state_t));
  FILE *fptr;
  unsigned int row = 0;
  unsigned int col = 0;
  int c;
  char** board_elements;
  board_elements = malloc(sizeof(*board_elements));
  board_elements[0] = malloc(sizeof(char));
  
  fptr = fopen(filename, "r");
  if (fptr == NULL) {
	  return NULL;
  }
  
  while (!feof(fptr)) {
       c = fgetc(fptr);
       if((char) c == '\n') {
	       board_elements[row][col] = '\0';
	       row++;
	       board_elements = realloc(board_elements, sizeof(*board_elements) * (row + 1));
	       board_elements[row] = malloc(sizeof(char));
	       col = 0;
       } else {
	    board_elements[row][col] = (char) c;
	    col++;
	    board_elements[row] = realloc(board_elements[row], sizeof(char) * (col + 1));
       }
  }
  board_elements = realloc(board_elements, sizeof(*board_elements) * row);
  fclose(fptr);
  curr_state->num_rows = row;
  curr_state->board = board_elements;
  
  return curr_state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int row = 0;
  row = state->snakes[snum].tail_row;
  unsigned int col = 0;
  col = state->snakes[snum].tail_col;
  char c = get_board_at(state, row, col);
  while (!is_head(c) && is_snake(c)) {
	//c = get_board_at(state, row, col);
	row = get_next_row(row, c);
	col = get_next_col(col, c);
	c = get_board_at(state, row, col);
  }
  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;
  return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function
  unsigned int snake_num = 0;
  state->snakes = malloc(sizeof(snake_t));
  for (unsigned int r = 0; r < state->num_rows; r++) {
	  for (unsigned int c = 0; c < strlen(state->board[r]); c++) {
		  if (is_tail(get_board_at(state, r, c))) {
			//snake_list[snake_num] = malloc(sizeof(snake_t));
			state->snakes = realloc(state->snakes, sizeof(snake_t) * (snake_num + 1));
			state->snakes[snake_num].tail_row = r;
			state->snakes[snake_num].tail_col = c;
			state->snakes[snake_num].live = true;
			find_head(state, snake_num);
			++snake_num;
		  }
	  }
  }
  state->num_snakes = snake_num;
  return state;
}
