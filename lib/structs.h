#ifndef STRUCTS_H
#define STRUCTS_H


#include "heap.h"
#include "constants.h"


typedef struct point {
    int x, y;
    Point(const int x, const int y) { this->x = x; this->y = y; }
    Point();
} Point;

typedef struct path {
    heap_node_t *hn;    
    int pos[2];
    int from[2];
    int cost;
} Path;

#endif