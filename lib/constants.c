#include "enums.c"

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

#define COLOR_RESET "\x1b[0m"
#define COLOR_ROCK  "\x1b[37m"
#define COLOR_TREE  "\x1b[92m"
#define COLOR_GRASS_S  "\x1b[92m"
#define COLOR_GRASS_T  "\x1b[32m"
#define COLOR_WATER  "\x1b[94m"
#define COLOR_MART  "\x1b[91m"
#define COLOR_CENTER  "\x1b[93m"
#define COLOR_PATH  "\x1b[33m"
#define COLOR_GATE  "\x1b[33m"
#define COLOR_BORDER  "\x1b[37m"

#define PLAYER  '@'
#define HIKER   'h'
#define RIVAL   'r'
#define PACER   'p'
#define WANDERER    'w'
#define SENTRY  's'
#define EXPLORER    'e'
#define SWIMMER 'm'


const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, GATE, PATH, MART, CENTER, BORDER};
const char* TERRAIN_COLORS[] = {COLOR_ROCK, COLOR_TREE, COLOR_GRASS_T, COLOR_WATER, COLOR_GRASS_S, COLOR_GATE, COLOR_PATH, COLOR_MART, COLOR_CENTER, COLOR_BORDER};
const char ALL_TRAINERS[] = {PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER, SWIMMER};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};

/*
 * rock, tree, tgrass, water, sgrass, gate, path, mart, center, border
 * player, hiker, rival, pacer, wanderer, sentry, explorer, swimmer
 */
const int STRESS[num_types_tra][num_types_ter] = {
    {D_MAX, D_MAX, 20, D_MAX, 10, 10, 10, 10, 10, D_MAX},
    {15, 15, 15, D_MAX, 10, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, 20, D_MAX, 10, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, 20, D_MAX, 15, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, D_MAX, 7, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX}
};