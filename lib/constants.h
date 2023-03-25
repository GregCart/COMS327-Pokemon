#ifndef CONSTANTS_H
#define CONSTANTS_H


#include "enums.h"


#define WORLD_SIZE 401
#define D_MAX 1000000
#define BOUNDS_X 80
#define BOUNDS_Y 21

#define ROCK    '%'
#define TREE    '^'
#define GRASS_S '.'
#define GRASS_T ':'
#define WATER   '~'
#define MART    'M'
#define CENTER  'C'
#define PATH    '#'
#define GATE    '#'
#define BORDER    '%'

// #define COLOR_RESET "\x1b[0m"
// #define COLOR_ROCK  "\x1b[37m"
// #define COLOR_TREE  "\x1b[92m"
// #define COLOR_GRASS_S  "\x1b[92m"
// #define COLOR_GRASS_T  "\x1b[32m"
// #define COLOR_WATER  "\x1b[94m"
// #define COLOR_MART  "\x1b[91m"
// #define COLOR_CENTER  "\x1b[93m"
// #define COLOR_PATH  "\x1b[33m"
// #define COLOR_GATE  "\x1b[33m"
// #define COLOR_BORDER  "\x1b[37m"

#define COLOR_RESET COLOR_WHITE
#define COLOR_ROCK  COLOR_WHITE
#define COLOR_TREE  COLOR_GREEN
#define COLOR_GRASS_S  COLOR_GREEN
#define COLOR_GRASS_T  COLOR_GREEN
#define COLOR_WATER  COLOR_BLUE
#define COLOR_MART  COLOR_MAGENTA
#define COLOR_CENTER  COLOR_RED
#define COLOR_PATH  COLOR_YELLOW
#define COLOR_GATE  COLOR_CYAN
#define COLOR_BORDER  COLOR_WHITE

#define PLAYER  '@'
#define HIKER   'h'
#define RIVAL   'r'
#define PACER   'p'
#define WANDERER    'w'
#define SENTRY  's'
#define EXPLORER    'e'
#define SWIMMER 'm'


extern const char TERRAIN[];
extern const char ALL_TERRAIN[];
extern const int TERRAIN_COLORS[];
extern const char ALL_TRAINERS[];
extern const int ALTITUDE[][2];

/*
 * rock, tree, tgrass, water, sgrass, gate, path, mart, center, border
 * player, hiker, rival, pacer, wanderer, sentry, explorer, swimmer
 */
extern const int STRESS[num_types_tra][num_types_ter];

#endif