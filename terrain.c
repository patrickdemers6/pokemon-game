/**
 * Generate 80x21 terrains.
 * Created by Patrick Demers.
 */

#include<stdio.h>
#include <stdlib.h>

#include "util.h"
#include "terrain.h"
#include "travel.h"
#include "npc.h"

#define pos(x, y) (map->map[y][x])

typedef enum directions {
	NORTH_SOUTH,
	EAST_WEST
} direction_t;

typedef struct {
	int x;
	int y;
	terrain_type_t ter;
} Region;

void add_regions(map_t *map);

void add_ter_path(direction_t ter_path_direction, map_t *map);

void add_two_by_two_block(terrain_type_t type, map_t *map, int manhattan_distance);

void place_pc();

void init_terrain(map_t *map) {
	int row, col;

	map->n = USE_RANDOM;
	map->e = USE_RANDOM;
	map->s = USE_RANDOM;
	map->w = USE_RANDOM;

	for (row = 0; row < HEIGHT; row++)
		for (col = 0; col < WIDTH; col++)
			pos(col, row) = ter_empty;
}

/**
 * Create a new terrain.
 */
void populate_terrain(map_t *map, coordinate_t *map_position) {
	int x, y;

	add_ter_path(NORTH_SOUTH, map);
	add_ter_path(EAST_WEST, map);

	int manhattan_distance = abs(map_position->x - START_POINT) + abs(map_position->y - START_POINT);
	// add a poke center
	add_two_by_two_block(ter_center, map, manhattan_distance);
	// add a poke mart
	add_two_by_two_block(ter_mart, map, manhattan_distance);

	// adding rocks and boulders
	int counter = 0;
	int goal = rand() % 10 + 1;
	while (counter < goal) {
		x = rand() % (WIDTH - 2) + 1;
		y = rand() % (HEIGHT - 2) + 1;
		if (pos(x, y) == ter_empty) {
			pos(x, y) = ter_boulder;
			counter++;
		}
	}

	add_regions(map);

	place_pc(map);
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
 * Determine if the program should move the ter_path in a random way.
 */
int should_move_randomly(direction_t ter_path_direction, int position, int last_random_position, int *random_value) {
	int sensitivity = 10;
	int ter_path_length = WIDTH;
	if (ter_path_direction == NORTH_SOUTH) {
		sensitivity = 15;
		ter_path_length = HEIGHT;
	}

	*random_value = rand() % 100;

	return (((*random_value < sensitivity || *random_value > 100 - sensitivity) &&
	         position - last_random_position > 7 && position > 2 && position < ter_path_length - 5) ||
	        position == ter_path_length - 5);
}

/**
 * Fill a ter_path between two points.
 */
void fill_ter_path_between(int current_position, int original_position, int movement_direction,
                           direction_t ter_path_direction, int position_across_board, map_t *map) {
	int j;
	if (ter_path_direction == NORTH_SOUTH) {
		for (j = original_position; j != current_position; j += movement_direction) {
			pos(j, position_across_board) = ter_path;
			pos(j, position_across_board + 1) = ter_path;
		}
		return;
	}
	for (j = original_position; j != current_position; j += movement_direction) {
		pos(position_across_board, j) = ter_path;
		pos(position_across_board + 1, j) = ter_path;
	}
}

/**
 * Add a ter_path to the board
 * @param ter_path_direction direction the ter_path is moving (north to south or east to west)
 * @param board the board to add the ter_path to
 * @param start_position the position to start the ter_path at
 * @param end_position the position to end the ter_path at
 */
void add_ter_path(direction_t ter_path_direction, map_t *map) {
	int ter_path_length, opposite_length, current_position, target_position, position_last_value_change;
	int i, movement_direction, original_position;

	if (ter_path_direction == EAST_WEST) {
		ter_path_length = WIDTH;
		opposite_length = HEIGHT;
	} else {
		ter_path_length = HEIGHT;
		opposite_length = WIDTH;
	}

	if (ter_path_direction == EAST_WEST) {
		if (map->w == USE_RANDOM)
			map->w = rand() % (opposite_length - 4) + 2;
		current_position = map->w;

		if (map->e == USE_RANDOM)
			map->e = rand() % (opposite_length - 4) + 2;
		target_position = map->e;
	} else {
		if (map->n == USE_RANDOM)
			map->n = rand() % (opposite_length - 4) + 2;
		current_position = map->n;

		if (map->s == USE_RANDOM)
			map->s = rand() % (opposite_length - 4) + 2;
		target_position = map->s;
	}

	position_last_value_change = 0;

	for (i = 0; i < ter_path_length; i++) {
		int random_value;
		if (should_move_randomly(ter_path_direction, i, position_last_value_change, &random_value)) {
			original_position = current_position;
			// if 5 away from end of ter_path, go to proper intersect
			if (i == ter_path_length - 5) {
				movement_direction = target_position - current_position > 0 ? 1 : -1;
				current_position = target_position;
			} else {
				movement_direction = random_value > 50 ? 1 : -1;

				if (near_edge(ter_path_direction, current_position))
					movement_direction *= -1;

				current_position += movement_direction * (random_value % 7);
				current_position = min(current_position, opposite_length - EDGE_TOLERANCE);
				current_position = max(current_position, EDGE_TOLERANCE);

			}

			position_last_value_change = i;

			fill_ter_path_between(current_position, original_position, movement_direction, ter_path_direction, i, map);

		}

		// add ter_path at this i value
		if (ter_path_direction == NORTH_SOUTH) {
			if (pos(0, i) == ter_empty) pos(0, i) = ter_boulder;
			if (pos(WIDTH - 1, i) == ter_empty) pos(WIDTH - 1, i) = ter_boulder;
			pos(current_position, i) = ter_path;
			if (i > 0 && i < HEIGHT - 1) {
				if (current_position + 1 != HEIGHT - 1)
					pos(current_position + 1, i) = ter_path;
				if (current_position - 1 > 0)
					pos(current_position - 1, i) = ter_path;
			}
		} else {
			if (pos(i, 0) == ter_empty) pos(i, 0) = ter_boulder;
			if (pos(i, HEIGHT - 1) == ter_empty) pos(i, HEIGHT - 1) = ter_boulder;
			pos(i, current_position) = ter_path;
			if (i != 0 && i < WIDTH - 1) {
				if (current_position + 1 != WIDTH - 1)
					pos(i, current_position + 1) = ter_path;
				if (current_position - 1 != 0)
					pos(i, current_position - 1) = ter_path;
			}
		}

		// get away from the edge
		if (i != 0 && i < ter_path_length - 5) {
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
void add_regions(map_t *map) {
	int i, j, k;
	Region regions[10];
	for (i = 0; i < 10; i++) {
		do {
			regions[i].x = rand() % (WIDTH - 6) + 3;
			regions[i].y = rand() % (HEIGHT - 4) + 2;
		} while (pos(regions[i].x, regions[i].y) != ter_empty);
		if (i < 4) {
			regions[i].ter = ter_grass;
		} else if (i < 8) {
			regions[i].ter = ter_clearing;
		} else {
			regions[i].ter = ter_forest;
		}
		pos(regions[i].x, regions[i].y) = regions[i].ter;
	}
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			if (pos(j, i) != ter_empty)
				continue;
			int least = 99999999;
			terrain_type_t least_char = ter_empty;
			for (k = 0; k < 10; k++) {
				int diff = abs(regions[k].x - j) + abs(regions[k].y - i) * 2;
				if (diff < least) {
					least = diff;
					least_char = regions[k].ter;
				}
			}
			if (least > 20)
				pos(j, i) = ter_mountain;
			else
				pos(j, i) = least_char;
		}
	}
}

void print_cost_grid(map_t *map, npc_characters_t character) {

	dijkstra_path(map, character);

}

void distance_grids(map_t *map) {
	printf("Hunter:\n");
	print_cost_grid(map, hiker);

	printf("\nRival:\n");
	print_cost_grid(map, rival);
	printf("\n");
}

void print_board(map_t *map, coordinate_t *terrain_position) {
	int row, column;

	distance_grids(map);

	for (row = 0; row < HEIGHT; row++) {
		for (column = 0; column < WIDTH; column++) {
			if ((terrain_position->x == 0 && column == 0) ||
			    (terrain_position->x == MAP_DIMENSION - 1 && column == WIDTH - 1) ||
			    (terrain_position->y == 0 && row == HEIGHT - 1) ||
			    (terrain_position->y == MAP_DIMENSION - 1 && row == 0)) {
				pos(column, row) = ter_boulder;
			}

			if (COLOR_OUTPUT) {
				switch (pos(column, row)) {
					case ter_clearing:
						printf(ANSI_COLOR_RED ".");
						break;
					case ter_grass:
						printf(ANSI_COLOR_MAGENTA ":");
						break;
					case ter_boulder:
					case ter_mountain:
						printf(ANSI_COLOR_YELLOW "%%");
						break;
					case ter_path:
						printf(ANSI_COLOR_WHITE "#");
						break;
					case ter_tree:
					case ter_forest:
						printf(ANSI_COLOR_GREEN "^");
						break;
					case ter_center:
						printf(ANSI_COLOR_CYAN "C");
						break;
					case ter_mart:
						printf(ANSI_COLOR_CYAN "M");
						break;
					case ter_pc:
						printf(ANSI_COLOR_RESET "@");
						break;
					default:
						printf(ANSI_COLOR_CYAN "!");
						break;
				}
			}
		}
		printf("\n" ANSI_COLOR_RESET);
	}
}

/**
 * Add a two by two building to the board.
 * @param type Letter that should be used to represent the building
 * @param board
 */
void add_two_by_two_block(terrain_type_t type, map_t *map, int manhattan_distance) {
	int x, y, probability, valid_position_found;

	// determine if two by two block should be placed
	if (manhattan_distance >= 200)
		probability = 5;
	else
		probability = ((-45 * manhattan_distance / 200) + 50);
	if (rand() % 100 > probability)
		return;

	x = rand() % (WIDTH - 4) + 2;
	y = rand() % (HEIGHT - 4) + 2;

	valid_position_found = 0;
	while (!valid_position_found) {
		for (; y < HEIGHT - 1 && !valid_position_found; y++) {
			for (; x < WIDTH - 1; x++) {
				if (pos(x, y) == ter_path && pos(x, y + 1) == ter_path && pos(x + 1, y) == ter_empty &&
				    pos(x + 2, y) == ter_empty && pos(x + 1, y + 1) == ter_empty &&
				    pos(x + 2, y + 1) == ter_empty) {
					pos(x + 1, y) = type;
					pos(x + 2, y) = type;
					pos(x + 1, y + 1) = type;
					pos(x + 2, y + 1) = type;
					valid_position_found = 1;
					break;
				}
				if (pos(x, y) == ter_path && pos(x, y + 1) == ter_path && pos(x - 1, y) == ter_empty &&
				    pos(x - 2, y) == ter_empty && pos(x - 1, y + 1) == ter_empty &&
				    pos(x - 2, y + 1) == ter_empty) {
					pos(x - 1, y) = type;
					pos(x - 2, y) = type;
					pos(x - 1, y + 1) = type;
					pos(x - 2, y + 1) = type;
					valid_position_found = 1;
					break;
				}
				if (pos(x, y) == ter_path && pos(x + 1, y) == ter_path && pos(x, y - 1) == ter_empty &&
				    pos(x + 1, y - 1) == ter_empty && pos(x, y - 2) == ter_empty &&
					pos(x + 1, y - 2) == ter_empty) {
					pos(x, y - 1) = type;
					pos(x + 1, y -1) = type;
					pos(x, y - 2) = type;
					pos(x + 1, y - 2) = type;
					valid_position_found = 1;
					break;
				}
				if (pos(x, y) == ter_path && pos(x + 1, y) == ter_path && pos(x, y + 1) == ter_empty &&
				    pos(x + 1, y + 1) == ter_empty && pos(x, y + 2) == ter_empty &&
				    pos(x + 1, y + 2) == ter_empty) {
					pos(x, y + 1) = type;
					pos(x + 1, y + 1) = type;
					pos(x, y + 2) = type;
					pos(x + 1, y + 2) = type;
					valid_position_found = 1;
					break;
				}
			}
			if (!valid_position_found) {
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

void place_pc(map_t *map) {
	while (1) {
		map->pc.x = rand() % (WIDTH - 2) + 1;
		map->pc.y = rand() % (HEIGHT - 2) + 1;

		if (map->map[map->pc.y][map->pc.x] == ter_path) {
			map->map[map->pc.y][map->pc.x] = ter_pc;
			break;
		}
	}
}
