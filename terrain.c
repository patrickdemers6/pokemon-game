#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ROCK '%'
#define TREE '^'
#define PATH '#'
#define WALL '%'
#define LONG_GRASS ':'
#define SHORT_GRASS '.'
#define SPACE ' '
#define HEIGHT 21
#define WIDTH 80
#define USE_RANDOM -1
#define COLOR_OUTPUT 1
#define EDGE_TOLERANCE 3

// colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_WHITE    "\033[0;37m"

typedef struct {
	int x;
	int y;
	char character;
} Region;

typedef enum directions {
	NORTH_SOUTH,
	EAST_WEST
} direction_t;

void add_regions(char board[HEIGHT][WIDTH]);

void create_board(char board[HEIGHT][WIDTH]);

void create_board_restricted(char board[HEIGHT][WIDTH], int start_x, int end_x, int start_y, int end_y);

void add_path(direction_t path_direction, char board[HEIGHT][WIDTH], int start_position, int end_position);

void print_board(char board[HEIGHT][WIDTH]);

int min(int a, int b);

int max(int a, int b);

void add_two_by_two_block(char letter, char board[HEIGHT][WIDTH]);

/**
 * Generate random terrain.
 */
int main(int argc, char *argv[]) {
	// create empty board filled with spaces
	char board[HEIGHT][WIDTH];
	memset(board, SPACE, WIDTH * HEIGHT);

	// determining seed
	printf("seed: ");
	if (argc == 2) {
		srand(atoi(argv[1]));
		printf("%d\n", atoi(argv[1]));
	} else {
		time_t t = time(0);
		srand(t);
		printf("%lu\n", t);
	}

	// allows for setting beginning/end values
	if (argc == 5)
		create_board_restricted(board, atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	else
		create_board(board);

	print_board(board);
	return 0;
}

/**
 * Create a board using random start/end positions.
 * @param board
 */
void create_board(char board[HEIGHT][WIDTH]) {
	create_board_restricted(board, USE_RANDOM, USE_RANDOM, USE_RANDOM, USE_RANDOM);
}

/**
 * Create a board using the specified start/end positions.
 */
void create_board_restricted(char board[HEIGHT][WIDTH], int start_x, int end_x, int start_y, int end_y) {
	int x, y;

	add_path(NORTH_SOUTH, board, start_x, end_x);
	add_path(EAST_WEST, board, start_y, end_y);

	// add a poke center
	add_two_by_two_block('C', board);
	// add a poke mart
	add_two_by_two_block('M', board);

	// adding rocks and boulders
	int counter = 0;
	int goal = rand() % 10 + 1;
	while (counter < goal) {
		x = rand() % (WIDTH - 2) + 1;
		y = rand() % (HEIGHT - 2) + 1;
		if (board[y][x] == SPACE) {
			board[y][x] = ROCK;
			counter++;
		}
	}

	add_regions(board);
}

/**
 * If the current position is within EDGE_TOLERANCE of an edge
 */
int near_edge(direction_t direction, int position) {
	if (direction == EAST_WEST)
		return position > HEIGHT - EDGE_TOLERANCE || position < EDGE_TOLERANCE;
	return position > WIDTH - EDGE_TOLERANCE || position < EDGE_TOLERANCE;
}

/**
 * Determine if the program should move the path in a random way.
 */
int should_move_randomly(direction_t path_direction, int position, int last_random_position, int *random_value) {
	int sensitivity = 10;
	int path_length = WIDTH;
	if (path_direction == NORTH_SOUTH) {
		sensitivity = 15;
		path_length = HEIGHT;
	}

	*random_value = rand() % 100;

	return (((*random_value < sensitivity || *random_value > 100 - sensitivity) && position - last_random_position > 7 && position > 2 && position < path_length - 5) || position == path_length - 5);
}

/**
 * Fill a path between two points.
 */
void fill_path_between(int current_position, int original_position, int movement_direction, direction_t path_direction, int position_across_board, char board[HEIGHT][WIDTH]) {
	int j;
	if (path_direction == NORTH_SOUTH) {
		for (j = original_position; j != current_position; j += movement_direction) {
			board[position_across_board][j] = PATH;
			board[position_across_board + 1][j] = PATH;
		}
		return;
	}
	for (j = original_position; j != current_position; j += movement_direction) {
		board[j][position_across_board] = PATH;
		board[j][position_across_board + 1] = PATH;
	}
}

/**
 * Add a path to the board
 * @param path_direction direction the path is moving (north to south or east to west)
 * @param board the board to add the path to
 * @param start_position the position to start the path at
 * @param end_position the position to end the path at
 */
void add_path(direction_t path_direction, char board[HEIGHT][WIDTH], int start_position, int end_position) {
	int path_length, opposite_length, current_position, target_position, position_last_value_change;
	int i, movement_direction, original_position;

	if (path_direction == EAST_WEST) {
		path_length = WIDTH;
		opposite_length = HEIGHT;
	}
	else {
		path_length = HEIGHT;
		opposite_length = WIDTH;
	}

	current_position = start_position;
	if (start_position == USE_RANDOM)
		current_position = rand() % (opposite_length - 4) + 2;

	target_position = end_position;
	if (end_position == USE_RANDOM)
		target_position = rand() % (opposite_length - 4) + 2;

	position_last_value_change = 0;

	for (i = 0; i < path_length; i++) {

		int random_value;
		if (should_move_randomly(path_direction, i, position_last_value_change, &random_value)) {
			original_position = current_position;
			// if 5 away from end of path, go to proper intersect
			if (i == path_length - 5) {
				movement_direction = target_position - current_position > 0 ? 1 : -1;
				current_position = target_position;
			} else {
				movement_direction = random_value > 50 ? 1 : -1;

				if (near_edge(path_direction, current_position))
					movement_direction *= -1;

				current_position += movement_direction * (random_value % 7);
				current_position = min(current_position, opposite_length - EDGE_TOLERANCE);
				current_position = max(current_position, EDGE_TOLERANCE);

			}

			position_last_value_change = i;

			fill_path_between(current_position, original_position, movement_direction, path_direction, i, board);

		}

		// add path at this i value
		if (path_direction == NORTH_SOUTH) {
			if (board[i][0] == SPACE) board[i][0] = WALL;
			if (board[i][WIDTH - 1] == SPACE) board[i][WIDTH - 1] = WALL;
			board[i][current_position] = PATH;
			if (i > 0 && i < HEIGHT - 1) {
				if (current_position + 1 != HEIGHT - 1)
					board[i][current_position + 1] = PATH;
				if (current_position - 1 > 0)
					board[i][current_position - 1] = PATH;
			}
		}
		else {
			if (board[0][i] == SPACE) board[0][i] = WALL;
			if (board[HEIGHT - 1][i] == SPACE) board[HEIGHT - 1][i] = WALL;
			board[current_position][i] = PATH;
			if (i != 0 && i < WIDTH - 1) {
				if (current_position + 1 != WIDTH - 1)
					board[current_position + 1][i] = PATH;
				if (current_position - 1 != 0)
					board[current_position - 1][i] = PATH;
			}
		}

		// get away from the edge
		if (i != 0 && i < path_length - 5) {
			if (current_position < EDGE_TOLERANCE)
				current_position = EDGE_TOLERANCE;
			else if (current_position > opposite_length - EDGE_TOLERANCE)
				current_position = opposite_length - EDGE_TOLERANCE;
		}
	}
}

/**
 * Add regions of terrain to the board.
 * @param board
 */
void add_regions(char board[HEIGHT][WIDTH]) {
	int i, j, k;
	Region regions[10];
	for (i = 0; i < 10; i++) {
		do {
			regions[i].x = rand() % (WIDTH - 6) + 3;
			regions[i].y = rand() % (HEIGHT - 4) + 2;
		} while (board[regions[i].y][regions[i].x] != SPACE);
		if (i < 4) {
			regions[i].character = LONG_GRASS;
		} else if (i < 8) {
			regions[i].character = SHORT_GRASS;
		} else {
			regions[i].character = TREE;
		}
		board[regions[i].y][regions[i].x] = regions[i].character;
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (board[i][j] != SPACE)
				continue;
			int least = 99999999;
			char least_char = SPACE;
			for (k = 0; k < 10; k++) {
				int diff = abs(regions[k].x - j) + abs(regions[k].y - i) * 2;
				if (diff < least) {
					least = diff;
					least_char = regions[k].character;
				}
			}
			if (least > 25)
				board[i][j] = ROCK;
			else
				board[i][j] = least_char;
		}
	}
}

void print_board(char board[HEIGHT][WIDTH]) {
	int row, column;
	for (row = 0; row < HEIGHT; row++) {
		for (column = 0; column < WIDTH; column++) {
			if (COLOR_OUTPUT)
				switch (board[row][column]) {
					case SHORT_GRASS:
						printf(ANSI_COLOR_RED);
						break;
					case LONG_GRASS:
						printf(ANSI_COLOR_MAGENTA);
						break;
					case WALL:
						printf(ANSI_COLOR_YELLOW);
						break;
					case PATH:
						printf(ANSI_COLOR_WHITE);
						break;
					case TREE:
						printf(ANSI_COLOR_GREEN);
						break;
					default:
						printf(ANSI_COLOR_CYAN);
						break;
				}
			printf("%c", board[row][column]);
		}
		printf("\n");
		printf(ANSI_COLOR_RESET);
	}
}

/**
 * Return the lesser of two numbers.
 */
int min(int a, int b) {
	return a < b ? a : b;
}

/**
 * Return the greater of two numbers.
 */
int max(int a, int b) {
	return a > b ? a : b;
}

/**
 * Add a two by two building to the board.
 * @param letter Letter that should be used to represent the building
 * @param board
 */
void add_two_by_two_block(char letter, char board[HEIGHT][WIDTH]) {
	int x, y;
	x = rand() % (WIDTH - 4) + 2;
	y = rand() % (HEIGHT - 4) + 2;
	int found = 0;
	while (!found) {
		for (; y < HEIGHT - 1 && !found; y++) {
			for (; x < WIDTH - 1; x++) {
				if (board[y][x] == PATH && board[y + 1][x] == PATH && board[y][x + 1] == SPACE &&
				    board[y][x + 2] == SPACE && board[y + 1][x + 1] == SPACE && board[y + 1][x + 2] == SPACE) {
					board[y][x + 1] = letter;
					board[y][x + 2] = letter;
					board[y + 1][x + 1] = letter;
					board[y + 1][x + 2] = letter;
					found = 1;
					break;
				}
				if (board[y][x] == PATH && board[y + 1][x] == PATH && board[y][x - 1] == SPACE &&
				    board[y][x - 2] == SPACE && board[y + 1][x - 1] == SPACE && board[y + 1][x - 2] == SPACE) {
					board[y][x - 1] = letter;
					board[y][x - 2] = letter;
					board[y + 1][x - 1] = letter;
					board[y + 1][x - 2] = letter;
					found = 1;
					break;
				}
				if (board[y][x] == PATH && board[y][x + 1] == PATH && board[y - 1][x] == SPACE &&
				    board[y - 1][x + 1] == SPACE && board[y - 2][x] == SPACE && board[y - 2][x + 1] == SPACE) {
					board[y - 1][x] = letter;
					board[y - 1][x + 1] = letter;
					board[y - 2][x] = letter;
					board[y - 2][x + 1] = letter;
					found = 1;
					break;
				}
				if (board[y][x] == PATH && board[y][x + 1] == PATH && board[y + 1][x] == SPACE &&
				    board[y + 1][x + 1] == SPACE && board[y + 2][x] == SPACE && board[y + 2][x + 1] == SPACE) {
					board[y + 1][x] = letter;
					board[y + 1][x + 1] = letter;
					board[y + 2][x] = letter;
					board[y + 2][x + 1] = letter;
					found = 1;
					break;
				}
			}
			if (!found) {
				x = 1;
				y++;
				if (y > HEIGHT - 1)
					y = 1;
			}
		}
		x = 1;
		y = 1;
	}
}
