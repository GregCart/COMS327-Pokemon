#include "lib/heap.h"

#include "constants.c"


//structs
typedef struct map {
    Terrain_e terrain[BOUNDS_Y][BOUNDS_X];
    Trainer **trainers;
    int alt[BOUNDS_Y][BOUNDS_X];
    int n, s, e, w;
} Map;

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
    int nextTime;
    int chr;
    int order;
    Dir_e dir;
    int (*move)(Entity *);
} Entity;

typedef struct trainer {
    Entity e;
} Trainer;

//specific examples
typedef Trainer PC;
typedef Trainer Hiker;
typedef Trainer Rival;
