#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <ncurses>

#include "queue.h"
#include "structs.h"
#include "maps.h"
#include "trainers.h"
#include "entity.h"


//globals
int numTrainers;
static Map *trails[num_types_tra];

//constants
static const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
static const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, GATE, PATH, MART, CENTER, BORDER};
static const char ALL_TRAINERS[] = {PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER, SWIMMER};
static const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};
static const int (*movement[]) (const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);


//movement utility
Dir_e change_direction(const int x, Dir_e d);

//comperators
int32_t path_cmp(const void *key, const void *with);
int32_t entity_cmp(const void *key, const void *with);
int32_t point_cmp(const void *key, const void *with);

//!chess
bool valid_move_ter(const Terrain_e t);
bool valid_pos_trainer(const Trainer_e e, const Terrain_e t, const Terrain_e start);
bool valid_pos(const Trainer_e e, const Terrain_e t);
bool containes_trainer(const Point p, const char map[BOUNDS_Y][BOUNDS_X][10]);
Point check_surroundings_trainer(const Point p, const char map[BOUNDS_Y][BOUNDS_X][10]);

//getters
int find_stress(const Map *m, const Entity *e, const Point p);
Point get_next_position(const Point p, const Dir_e d);
Dir_e get_lower_alt(const Point p, const Map *m);

//movement functions
int move_player(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_hiker(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_rival(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_pacer(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_wanderer(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_sentry(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_explorer(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_swimmer(const Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);

//distance finders
int manhattan(const Point p, const Point q);
int dijkstra(const Map *m, const Map *w, const Point p, const Entity *e);

//HP's
int print_display(const char map[BOUNDS_Y][BOUNDS_X][10]);
int color_display(const Map *m, const PC *player, const Trainer **ts);
int print_entity(const Entity *e);

//new screen manipulation
int enter_building(const Terrain_e t);
int initiate_battle(const Entity *trainer, const PC *player);
int display_trainer_list(const Map *m, const Entity *player, const int y, const int x);

//misc
bool check_battle(const Map *wrld, const Entity *e, const PC *player);

#endif