//
// Created by patrick on 2/5/22.
//

#ifndef ASSIGNMENT_1_03_NPC_H
#define ASSIGNMENT_1_03_NPC_H

#include <stdlib.h>
#include <limits.h>

#include "terrain.h"
#include "heap.h"

typedef enum dim {
	dim_x,
	dim_y,
	num_dims
} dim_t;

typedef struct path {
	heap_node_t *hn;
	uint8_t pos[2];
	int cost;
} path_t;
typedef uint8_t pair_t[num_dims];

typedef enum npc_characters {
	hiker,
	rival
} npc_characters_t;

int dijkstra_path(map_t *m, npc_characters_t character);
#endif //ASSIGNMENT_1_03_NPC_H
