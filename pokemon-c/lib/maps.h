#ifndef MAPS_H
#define MAPS_H


#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "heap.h"
#include "utils-misc.h"


//globals
//sorted in {y, x}


//HP's
int print_map_alt(Map *m);
int print_map_terrain(Map *m);
int print_map(Map *m);
int print_cost_map(Map *m);

//-cats
int copy_map_terrain(Map *from, Map *to);
int copy_map_alt(Map *from, Map *to);
int copy_map(Map *from, Map *to);

//checkers
int check_map(Map *m);

//map creations
int make_boundary(Map *m, Point curPos);
int add_terrain(Map *m, int x, int y, Terrain_e c);
int spread_seed(Map *m, queue_t *qx, queue_t *qy);
int fill_map(Map *m);
int trailblaze(Map *m, Point curPos, Point center);
int create_map(Map *m, Point curPos, Point center);

#endif