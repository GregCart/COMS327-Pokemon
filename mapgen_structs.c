#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include "lib/queue.h"
#include "lib/heap.h"


#define BOUNDS_X 80
#define BOUNDS_Y 21
#define ROCK '%'
#define TREE '^'
#define GRASS_S '.'
#define GRASS_T ':'
#define WATER '~'
#define PATH '#'

const int TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};

typedef struct map {
    int terrain[BOUNDS_Y][BOUNDS_X];
    int px, py;
    int n, s, e, w;
} map_t;

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

int print_map(map_t *map)
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%c", map->terrain[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int make_boundary(map_t *m, map_t *alt)
{
    int i;

    int x[4] = {(rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        m->terrain[0][i] = ROCK;
        m->terrain[BOUNDS_Y-1][i] = ROCK;
        alt->terrain[0][i] = 99;
        alt->terrain[BOUNDS_Y-1][i] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        m->terrain[i][0] = ROCK;
        m->terrain[i][BOUNDS_X-1] = ROCK;
        alt->terrain[i][0] = 99;
        alt->terrain[i][BOUNDS_X-1] = 99;
    }

    //set paths
    alt->terrain[x[3]][0] = 25;
    m->terrain[x[3]][0] = PATH;
    m->w = x[3];
    alt->terrain[x[2]][BOUNDS_X-1] = 25;
    m->terrain[x[2]][BOUNDS_X-1] = PATH;
    m->e = x[2];
    alt->terrain[BOUNDS_Y-1][x[0]] = 25;
    m->terrain[BOUNDS_Y-1][x[0]] = PATH;
    m->s = x[0];
    alt->terrain[0][x[1]] = 25;
    m->terrain[0][x[1]] = PATH;
    m->n = x[1];

    return 0;
}

int main(int argc, char *argv[])
{
    map_t *map;
    int i, j;

    srand(time(NULL));

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            m->ter[i][j] = ' ';
        }
    }

    if (!create_map(map)) {
        print_map(map);
    }

    return 0;
}