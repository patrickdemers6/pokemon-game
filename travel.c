/**
 * Travel between terrains on a 399x399 map.
 * Created by Patrick Demers.
 */

#include <stdlib.h>
#include <stdio.h>
#include "travel.h"
#include "terrain.h"
#include "util.h"

void print_terrain(map_t *map[MAP_DIMENSION][MAP_DIMENSION], coordinate_t *new_position);

void generate_map(map_t *map[MAP_DIMENSION][MAP_DIMENSION]) {
	char input;
	int backup_x, backup_y;

	coordinate_t *map_position = malloc(sizeof(struct coordinate));
	map_position->x = START_POINT;
	map_position->y = START_POINT;

	do {
		print_terrain(map, map_position);
		printf("(%d, %d)\n\n>", map_position->x - START_POINT, map_position->y - START_POINT);

		// get new input and handle eof
		if (scanf(" %c", &input) == EOF)
			input = 'q';

		switch(input) {
			case 'n':
				map_position->y = min(map_position->y + 1, MAP_DIMENSION - 1);
				break;
			case 's':
				map_position->y = max(map_position->y - 1, 0);
				break;
			case 'e':
				map_position->x = min(map_position->x + 1, MAP_DIMENSION - 1);
				break;
			case 'w':
				map_position->x = max(map_position->x - 1, 0);
				break;
			case 'f':
				backup_x = map_position->x;
				backup_y = map_position->y;

				scanf(" %d %d", &(map_position->x), &(map_position->y));
				map_position->x += START_POINT;
				map_position->y += START_POINT;
				if (map_position->x >= MAP_DIMENSION || map_position->y >= MAP_DIMENSION
				|| map_position->x < 0 || map_position->y < 0) {
					printf("Invalid position entered. Please try again.\n");
					map_position->x = backup_x;
					map_position->y = backup_y;
				}
				break;
			case 'q':
				printf("Thanks for playing!\n\n");
				break;
			default:
				printf("Invalid command entered. Please try again.\n");
				break;
		}
	} while (input != 'q');

	free(map_position);
	map_position = NULL;
}

void print_terrain(map_t *map[MAP_DIMENSION][MAP_DIMENSION], coordinate_t *new_position) {
	// this map has been visited. Regenerate it
	if (map[new_position->y][new_position->x] != NULL) {
		print_board(map[new_position->y][new_position->x], new_position);
		return;
	}

	map[new_position->y][new_position->x] = malloc(sizeof(map_t));
	init_terrain(map[new_position->y][new_position->x]);

	// determine existing end points that need to be observed
	map_t *north = NULL;
	map_t *east = NULL;
	map_t *south = NULL;
	map_t *west = NULL;

	// find neighboring boards. Leave NULL if off board
	if (new_position->y != MAP_DIMENSION - 1)
		north = map[new_position->y + 1][new_position->x];
	if (new_position->x != MAP_DIMENSION - 1)
		east = map[new_position->y][new_position->x + 1];
	if (new_position->y != 0)
		south = map[new_position->y - 1][new_position->x];
	if (new_position->x != 0)
		west = map[new_position->y][new_position->x - 1];

	// if board has been visited (seed not zero), use the corresponding exit point
	if (north != NULL)
		map[new_position->y][new_position->x]->n = north->s;
	if (east != NULL)
		map[new_position->y][new_position->x]->e = east->w;
	if (south != NULL)
		map[new_position->y][new_position->x]->s = south->n;
	if (west != NULL)
		map[new_position->y][new_position->x]->w = west->e;

	populate_terrain(map[new_position->y][new_position->x], new_position);
	print_board(map[new_position->y][new_position->x], new_position);

}
