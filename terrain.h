
#ifndef ASSIGNMENT_1_02_TERRAIN_H
#define ASSIGNMENT_1_02_TERRAIN_H

#include <stdint.h>

#define HEIGHT 21
#define WIDTH 80
#define USE_RANDOM -1
#define COLOR_OUTPUT 1
#define EDGE_TOLERANCE 3


#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_WHITE    "\033[0;37m"


typedef struct coordinate {
	int x;
	int y;
} coordinate_t;

typedef enum terrain_type {
	ter_boulder,
	ter_tree,
	ter_path,
	ter_mart,
	ter_center,
	ter_grass,
	ter_clearing,
	ter_mountain,
	ter_forest,
	ter_empty,
	ter_pc
} terrain_type_t;

typedef struct pc {
	int x;
	int y;
} pc_t;

typedef struct map {
	terrain_type_t map[HEIGHT][WIDTH];
	int n, e, s, w;
	pc_t pc;
} map_t;

void init_terrain(map_t *map);

void print_board(map_t *map, coordinate_t *terrain_position);

void populate_terrain(map_t *map, coordinate_t *map_position);

#endif //ASSIGNMENT_1_02_TERRAIN_H
