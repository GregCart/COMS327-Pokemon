#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"
#include "structs.h"


//movement utility
Dir_e change_direction(int x, Dir_e d);

//comperators
int32_t path_cmp(const void *key, const void *with);
int32_t entity_cmp(const void *key, const void *with);

//!chess
int valid_move_ter(Terrain_e t);
int valid_pos_list(Trainer_e e, Terrain_e t, Terrain_e st);
int valid_pos(Trainer_e e, Terrain_e t);

//getters
int find_stress(Map *m, Entity *e, Point p);
Point get_next_position(Point p, Dir_e d);
Dir_e get_lower_alt(Point p, Map *m);

//distance finders
int manhattan(Point p, Point q);
int dijkstra(Map *m, Map *w, Point p, Entity *e);

//HP's
int print_display(char map[BOUNDS_Y][BOUNDS_X][10]);
int color_display(Map *m, Trainer **ts, int numTrainers);
int print_entity(Entity *e);

//new screen manipulation
int enter_building(Terrain_e t);
int initiate_battle(Entity *trainer, PC *player);

//misc
int check_battle(Map *wrld, Entity *e, PC *player);

#endif