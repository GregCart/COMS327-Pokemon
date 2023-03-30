#ifndef UTILS_H
#define UTILS_H


#include "queue.h"
#include "maps.h"


//globals
extern int numTrainers;
extern Map *trails[num_types_tra];

//constants
extern int (*movement[]) (Entity *e, const Map *m, const char map[BOUNDS_Y][BOUNDS_X][10]);
static const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
static const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, GATE, PATH, MART, CENTER, BORDER};
static const char ALL_TRAINERS[] = {PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER, SWIMMER};
static const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};


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
Entity* find_entity_pos(Trainer **t, const Point p) ;

//movement functions
int move_player(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_hiker(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_rival(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_pacer(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_wanderer(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_sentry(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_explorer(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);
int move_swimmer(Entity *self, const Map *wrld, const char map[BOUNDS_Y][BOUNDS_X][10]);

//distance finders
int manhattan(const Point p, const Point q);
int dijkstra(const Map *m, const Map *w, const Point p, const Entity *e);

//HP's
int print_display(const char map[BOUNDS_Y][BOUNDS_X][10]);
int color_display(const Map *m, const PC *player, Trainer **ts);
int print_entity(const Entity *e);

//new screen manipulation
int enter_building(const Terrain_e t);
int initiate_battle(const Entity *trainer, const PC *player);
int display_trainer_list(const Map *m, const Entity *player, const int y, const int x);

//misc
bool check_battle(const Map *wrld, const Entity *e, const PC *player);

#endif