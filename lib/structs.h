#ifndef STRUCTS_H
#define STRUCTS_H


#include "heap.h"
#include "constants.h"


//structs
typedef struct entity Entity;
typedef struct map Map;

typedef struct point {
    int x, y;
} Point;

typedef struct path {
    heap_node_t *hn;    
    int pos[2];
    int from[2];
    int cost;
} Path;

typedef struct entity {
    heap_node_t *hn;
    Point pos;
    Terrain_e start;
    int defeted;
    int nextTime;
    int chr;
    int order;
    Dir_e dir;
    int (*do_move)(Entity *, Map *m, char map[BOUNDS_Y][BOUNDS_X][10]);
} Entity;

typedef struct trainer {
    Entity e;
} Trainer;

typedef struct map {
    Terrain_e terrain[BOUNDS_Y][BOUNDS_X];
    Trainer **trainers;
    int alt[BOUNDS_Y][BOUNDS_X];
    int n, s, e, w;
} Map;

//specific examples
typedef Trainer PC;
typedef Trainer Hiker;
typedef Trainer Rival;

#endif