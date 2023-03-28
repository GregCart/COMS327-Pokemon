#ifndef STRUCTS_H
#define STRUCTS_H


#include "heap.h"
#include "constants.h"


typedef struct point {
    int x, y;
    point(const int x, const int y) { this->x = x; this->y = y; }
    point() { this->x = 0; this->y = 0; };
} Point;

typedef struct path {
    heap_node_t *hn;    
    int pos[2];
    int from[2];
    int cost;
} Path;

#endif