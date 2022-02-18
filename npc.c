//
// Created by patrick on 2/5/22.
//
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include "npc.h"
#include "terrain.h"
#include "heap.h"

#define weightpair(pair) (weight[pair[dim_y]][pair[dim_x]])
#define weightxy(x, y) (weight[y][x])
static int32_t cost_cmp(const void *key, const void *with) {
	return ((path_t *) key)->cost - ((path_t *) with)->cost;
}
int dijkstra_path(map_t *m, npc_characters_t character)
{
	static path_t map_positions[HEIGHT][WIDTH], *p;
	uint32_t weight[HEIGHT][WIDTH];
	pair_t from;
	from[dim_y] = m->pc.y;
	from[dim_x] = m->pc.x;

	heap_t h;
	int x, y;

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			map_positions[y][x].pos[dim_y] = y;
			map_positions[y][x].pos[dim_x] = x;

			if (character == rival) {
				switch (m->map[y][x]) {
					case ter_path:
					case ter_clearing:
						weightxy(x, y) = 10;
						break;
					case ter_grass:
						weightxy(x, y) = 20;
						break;
					case ter_pc:
						weightxy(x, y) = 10;
						break;
					default:
						weightxy(x, y) = INT_MAX;
				}
			}
			else if (character == hiker) {
				switch (m->map[y][x]) {
					case ter_path:
					case ter_clearing:
						weightxy(x, y) = 10;
						break;
					case ter_grass:
					case ter_mountain:
					case ter_forest:
						weightxy(x, y) = 15;
						break;
					case ter_pc:
						weightxy(x, y) = 10;
						break;
					default:
						weightxy(x, y) = INT_MAX;
				}
			}
		}
	}

	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			map_positions[y][x].cost = INT_MAX;
		}
	}

	map_positions[from[dim_y]][from[dim_x]].cost = 0;

	heap_init(&h, cost_cmp, NULL);

	for (y = 1; y < HEIGHT - 1; y++) {
		for (x = 1; x < WIDTH - 1; x++) {
			if (weightxy(x, y) != INT_MAX)
				map_positions[y][x].hn = heap_insert(&h, &map_positions[y][x]);
		}
	}

	while ((p = heap_remove_min(&h))) {
		p->hn = NULL;

		if ((map_positions[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
		    (map_positions[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] - 1]
			[p->pos[dim_x]    ].hn);
		}
		if ((map_positions[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
		    (map_positions[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y]    ]
			[p->pos[dim_x] - 1].hn);
		}
		if ((map_positions[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
		    (map_positions[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
		     ((p->cost + weightpair(p->pos)) ))) {
			map_positions[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y]    ]
			[p->pos[dim_x] + 1].hn);
		}
		if ((map_positions[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
		    (map_positions[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] + 1]
			[p->pos[dim_x]    ].hn);
		}
		if ((map_positions[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
		    (map_positions[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] + 1]
			[p->pos[dim_x] + 1].hn);
		}
		if ((map_positions[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
		    (map_positions[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] + 1]
			[p->pos[dim_x] - 1].hn);
		}
		if ((map_positions[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
		    (map_positions[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] - 1]
			[p->pos[dim_x] + 1].hn);
		}
		if ((map_positions[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
		    (map_positions[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
		     ((p->cost + weightpair(p->pos))))) {
			map_positions[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
					((p->cost + weightpair(p->pos)));
			heap_decrease_key_no_replace(&h, map_positions[p->pos[dim_y] - 1]
			[p->pos[dim_x] - 1].hn);
		}
	}

	for (y = 1; y < HEIGHT - 1; y++) {
		for (x = 1; x < WIDTH - 1; x++) {
			printf("%02d ", map_positions[y][x].cost % 100);
		}
		printf("\n");
	}

	return -1;
}