#include <stdio.h>
#include "travel.h"
#include<stdlib.h>
#include<time.h>

int main(int argc, char *argv[]) {
	int row, col;

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

	map_t *map[MAP_DIMENSION][MAP_DIMENSION];
	for (row = 0; row < MAP_DIMENSION; row++)
		for (col = 0; col < MAP_DIMENSION; col++)
			map[row][col] = NULL;

	generate_map(map);

	for (row = 0; row < MAP_DIMENSION; row++)
		for (col = 0; col < MAP_DIMENSION; col++)
			free(map[row][col]);

	return 0;
}
