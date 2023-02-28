#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <string.h>

#include "lib/queue.h"
#include "lib/heap.h"


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


//enums
typedef enum terrain_e {
    RCK,
    TRE,
    GT,
    WTR,
    GS,
    GTE,
    PTH,
    MRT,
    CTR,
    BDR,
    num_types_ter
} Terrain_e;

typedef enum trainer_e {
    PLAY,
    HIKR,
    RIVL,
    PACR,
    WAND,
    SENT,
    EXPL,
    SWIM,
    num_types_tra
} Trainer_e;

typedef enum direction_e{
    NONE,
    NE,
    N,
    NW,
    W,
    E,
    SW,
    S,
    SE,
    num_dir
} Dir_e;

//structs
typedef struct map {
    Terrain_e terrain[BOUNDS_Y][BOUNDS_X];
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
    Point pos;
    int chr;
    Dir_e dir;
    Point (*move)(Entity *, const Point p);
} Entity;

typedef struct trainer {
    Entity e;
} Trainer;

//specific examples
typedef Trainer PC;
typedef Trainer Hiker;
typedef Trainer Rival;

//movment prototypes
int move_player(Entity self);
int move_hiker(Entity self);
int move_rival(Entity self);
int move_pacer(Entity self);
int move_wanderer(Entity self);
int move_sentry(Entity self);
int move_explorer(Entity self);
int move_swimmer(Entity self);

const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, GATE, PATH, MART, CENTER, BORDER};
const char* ALL_COLORS[] = {COLOR_ROCK, COLOR_TREE, COLOR_GRASS_T, COLOR_WATER, COLOR_GRASS_S, COLOR_GATE, COLOR_PATH, COLOR_MART, COLOR_CENTER, COLOR_BORDER};
const char ALL_TRAINERS[] = {PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER, SWIMMER};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};
const int (*movement[]) (Entity e) = {move_player, move_hiker, move_rival, move_pacer, move_wanderer, move_sentry, move_explorer, move_swimmer};
/*
 * rock, tree, tgrass, water, sgrass, gate, path, mart, center
 * player, hiker, rival, pacer, wanderer, sentry, explorer, swimmer
 */
const int STRESS[num_types_tra][9] = {
    {D_MAX, D_MAX, 20, D_MAX, 10, 10, 10, 10, 10},
    {15, 15, 15, D_MAX, 10, D_MAX, 10, 50, 50},
    {D_MAX, D_MAX, 20, D_MAX, 10, D_MAX, 10, 50, 50},
    {D_MAX, D_MAX, 20, D_MAX, 15, D_MAX, 10, 50, 50},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, 7, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX}
};
const Point center = {.x = 200, .y = 200};

//sorted in {y, x}
// int EXITS[4][2];
Map *world[WORLD_SIZE][WORLD_SIZE];
Point curPos;
Terrain_e *startTer;
Map *trails[num_types_tra];


//printers
int print_display(char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%s", map[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int print_map_alt(Map *m) 
{
    int i , j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%d ", m->alt[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int print_map_terrain(Map *m) 
{
    int i , j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%d ", m->terrain[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int print_cost_map(Map *m) 
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (m->alt[i][j] < D_MAX) {
                printf("%02d ", m->alt[i][j] % 100);
            } else {
                printf("   ");
            }
            
        }
        printf("\n");
    }

    return 0;
}

int print_entity(Entity *e) 
{
    int i;

    printf("Entity: {\n");
    printf("\tChar: %c\n", e->chr);
    printf("\tPos: (%d, %d),\n", e->pos.x, e->pos.y);
    printf("\tCurrent direction: %d\n", e->dir);
    printf("}\n");

    return 0;
}

int print_trainer(Trainer *t)
{
    print_entity(t->e);
}

//coppiers
int copy_map_terrain(Map *from, Map *to) {
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            to->terrain[i][j] = from->terrain[i][j];
        }
    }

    return 0;
}

int copy_map_alt(Map *from, Map *to) {
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            to->alt[i][j] = from->alt[i][j];
        }
    }

    return 0;
}

int copy_map(Map *from, Map *to) {
    copy_map_alt(from, to);
    copy_map_terrain(from, to);

    return 0;
}

//comperators
static int32_t path_cmp(const void *key, const void *with) 
{
  return ((Path *) key)->cost - ((Path *) with)->cost;
}

static int32_t entity_cmp(const void *key, const void *with) 
{
  return ((Entity *) key)->dir - ((Entity *) with)->dir;
}

//distance finders
int manhattan(Point p, Point q)
{
    int ret;
    // printf("(%d, %d) and (%d, %d)\n", p.x, p.y, q.x, q.y);
    ret = abs(p.x - q.x) + abs(p.y - q.y);
    // printf("ret:%d\n", ret);
    return ret;
}

static int dijkstra(Map *m, Point p, Entity *e)
{
    // printf("Start Dijk's\n");
    static Path path[BOUNDS_Y][BOUNDS_X], *pth;
    static int init = 0;
    heap_t h;
    int x, y, c;
    Map *w = world[curPos.y][curPos.x];
    Point pnt;

    if (!init) {
        for (y = 0; y < BOUNDS_Y; y++) {
            for (x = 0; x < BOUNDS_X; x++) {
                path[y][x].pos[0] = y;
                path[y][x].pos[1] = x;
            }
        }
        init = 1;
    }

    // printf("Init paths\n");
    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            path[y][x].cost = D_MAX;
        }
    }

    path[p.y][p.x].cost = 0;

    heap_init(&h, path_cmp, NULL);

    // print_map_terrain(m);

    // printf("fill heap\n");
    for (y = 1; y < BOUNDS_Y - 1; y++) {
        for (x = 1; x < BOUNDS_X - 1; x++) {
            // if (m->terrain[y][x] == MRT || m->terrain[y][x] == CTR)
            //     printf("x:%d, y:%d, str:%d  ", x, y, STRESS[e.chr][m->terrain[y][x]]);
            if (find_stress(w, e, (Point){.x=x, .y=y}) != D_MAX) {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
                // printf("In\t\t");
            }
        }
        // printf("\n");
    }
    // printf("\n");

    // printf("loop start\n");
    c = 0;
    while ((pth = heap_remove_min(&h))) {
        pth->hn = NULL;
        // printf("get:%d, find:%d\n", get_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}), find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}));

        // printf("check above\n");//above
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check TL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //TL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check TR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //TR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check left\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //left
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check right\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //right
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check bottom\n");
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //bottom
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check BL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //BL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check BR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]}))))) {    //BR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, (Point) {.x=pth->pos[1], .y=pth->pos[0]});
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }

    }
    // printf("#less: %d\n", c);

    // printf("fill cost map\n");
    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            // printf("X: %d, y:%d, cost:%d\n", x, y, path[y][x].cost);
            m->alt[y][x] = path[y][x].cost;
        }
    }

    return 0;
}

//checkers
int valid_pos_list(Trainer_e e, Terrain_e t, int i)
{
    // printf("e:%d, t:%d\n", e, t);
    // printf("\tstrss:%d\n", STRESS[e][t]);
    if (t == GTE || t == BDR || t == MRT || t == CTR) {
        return 1;
    }

    switch (e){
        case PLAY:
        case RIVL:
        case HIKR:
        case SWIM:
        case EXPL:
            return (STRESS[e][t] != D_MAX);
            break;
        case PACR:
        case WAND:
            return (t == startTer[i] && t != WTR);
            break;
        case SENT:
            return 0;
            break;
        default:
            return 1;
    }
    return 1;
}

int valid_pos(Trainer_e e, Terrain_e t)
{
    // printf("e:%d, t:%d\n", e, t);
    // printf("\tstrss:%d\n", STRESS[e][t]);
    if (t == GTE || t == BDR || t == MRT || t == CTR) {
        return 1;
    }
    return (STRESS[e][t] != D_MAX);
}

int check_map(Map *m) 
{
    printf("terrain: %ld x %ld\n", sizeof(m->terrain[0])/sizeof(Terrain_e), sizeof(m->terrain)/sizeof(m->terrain[0])/sizeof(char));
    printf("n:%d s:%d e:%d w:%d\n", m->n, m->s, m->e, m->w);

    return 0;
}

//getters
int find_stress(Map *m, Entity *e, Point p)
{
    return STRESS[e->chr][m->terrain[p.y][p.x]];
}

Point get_next_position(Point p, Dir_e d)
{
    switch (d) {
        case NW:
            return (Point) {.x = p.x - 1, .y = p.y - 1};
        case N:
            return (Point) {.x = p.x, .y = p.y - 1};
        case NE:
            return (Point) {.x = p.x + 1, .y = p.y - 1};
        case W:
            return (Point) {.x = p.x - 1, .y = p.y};
        case E:
            return (Point) {.x = p.x + 1, .y = p.y};
        case SW:
            return (Point) {.x = p.x - 1, .y = p.y + 1};
        case S:
            return (Point) {.x = p.x, .y = p.y + 1};
        case SE:
            return (Point) {.x = p.x + 1, .y = p.y + 1};
        default:
            fprintf(stderr, "Error getting next position from (%d, %d) in direction %d", p.x, p.y, d);
            return (Point) {-1, -1};
    }
}

Dir_e get_lower_alt(Point p, Map *m)
{
    int i, cost;
    Point q;

    for (i = 1; i < num_dir; i++) {
        if (m->alt[q.x][q.y] && m->alt[p.x][p.y]) {
            q = get_next_position(p, i);
            if (m->alt[q.x][q.y] <= m->alt[p.x][p.y]) {
                return i;
            }
        }
    }

    return -1;
}

//map creations
int make_boundary(Map *m)
{
    int i;

    int x[4] = {m->n, m->s, m->e, m->w};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        m->terrain[0][i] = BDR;
        m->terrain[BOUNDS_Y-1][i] = BDR;
        m->alt[0][i] = 99;
        m->alt[BOUNDS_Y-1][i] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        m->terrain[i][0] = BDR;
        m->terrain[i][BOUNDS_X-1] = BDR;
        m->alt[i][0] = 99;
        m->alt[i][BOUNDS_X-1] = 99;
    }

    //set paths
    if (curPos.x > 2) {
        m->alt[x[3]][0] = 25;
        m->terrain[x[3]][0] = GTE;
        m->w = x[3];
    }
    if (curPos.x < WORLD_SIZE - 1) {
        m->alt[x[2]][BOUNDS_X-1] = 25;
        m->terrain[x[2]][BOUNDS_X-1] = GTE;
        m->e = x[2]-1;
    }
    if (curPos.y < WORLD_SIZE - 1) {
        m->alt[BOUNDS_Y-1][x[1]] = 25;
        m->terrain[BOUNDS_Y-1][x[1]] = GTE;
        m->s = x[1];
    }
    if (curPos.y > 1) {
        m->alt[0][x[0]] = 25;
        m->terrain[0][x[0]] = GTE;
        m->n = x[0];
    }
    // print_display(m);
    
    return 0;
}

int add_terrain(Map *m, int x, int y, Terrain_e c)
{
    Terrain_e *chr = &m->terrain[y][x];
    int *alt = &m->alt[y][x];

    if ((rand() % 137) != 0) {
        *chr = c;
        alt = alt - (rand() % 6) + 1;
    } else {
        alt = alt - ((rand() % 3) - 1) * (rand() % 3);
        if (*alt > 45) {
            *chr = ROCK;
        } else if (*alt > 43) {
            int tmp = (rand() % 3);
            if (tmp == 1) {
                *chr = TRE;
            } else if(tmp == 0) {
                *chr = RCK;
            } else {
                *chr = GS;
            }
        } else if (*alt > 30) {
            int tmp = (rand() % 4);
            if (tmp == 1) {
                *chr = TRE;
            } else if(tmp == 0) {
                *chr = RCK;
            } else if (tmp == 2) {
                *chr = GT;
            } else {
                *chr = GS;
            }
        } else if (*alt > 25) {
            int tmp = (rand() % 3);
            if (tmp == 1) {
                *chr = TRE;
            } else if (tmp == 2) {
                *chr = GT;
            } else {
                *chr = GS;
            }
        } else if (*alt > 18) {
            int tmp = (rand() % 2);
            if (tmp == 1) {
                *chr = GT;
            } else {
                *chr = GS;
            }
        } else if (*alt > 15) {
            int tmp = (rand() % 2);
            if (tmp == 1) {
                *chr = GT;
            } else {
                *chr = WTR;
            }
        } else {
            *chr = WTR;
        }
    }

    return 0;
}

int spread_seed(Map *m, queue_t *qx, queue_t *qy)
{
    // printf("Begin spreading\n");
    int i, x, y;
    Terrain_e chr;

    queue_dequeue(qx, &x);
    queue_dequeue(qy, &y);

    while (!queue_is_empty(qx)) {
        queue_dequeue(qx, &x);
        queue_dequeue(qy, &y);

        chr = m->terrain[y][x];
        // alt = map[x][y][1];
        // print_display(map, 0);
        // printf("Length: %d, Char: %c\n", queue_length(qx), chr);

        for (i = -1; i < 2; i++) {
            if (i != 0) {
                if (m->alt[y][x+i] < 50 && m->terrain[y][x+i] == -1) {
                    queue_enqueue(qx, x+i);
                    queue_enqueue(qy, y);
                    add_terrain(m, x+i, y, chr);
                }
            }
            if (m->alt[y+1][x+i] < 50 && m->terrain[y+1][x+i] == -1) {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y+1);
                add_terrain(m, x+i, y+1, chr);
            }
            if (m->alt[y-1][x+i] < 50 && m->terrain[y-1][x+i] == -1) {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y-1);
                add_terrain(m, x+i, y-1, chr);
            }
        }
        
    }
    // printf("End spreading\n");
    return 0;
}

int fill_map(Map *m)
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (m->terrain[i][j] == -1) {
                m->terrain[i][j] = GS;
                m->alt[i][j] = 25;
            }
        }
    }

    return 0;
}

int trailblaze(Map *m)
{
    int i, j, k, l, b, d, p, u = BOUNDS_X/2 + (rand() % 20);

    //left
    for (i = 0; i < u; i++) {
        // printf("%d, %d\n", m->w, m->w+i);
        m->terrain[m->w][1+i] = PTH;
        m->alt[m->w][1+i] = 25;
    }
    l = i;
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", m->e, 0+i);
        m->terrain[m->e][BOUNDS_X-1+i] = PTH;
        m->alt[m->e][BOUNDS_X-1+i] = 25;
        i++;
    }
    //connect left and right
    j = (m->w > m->e) ? m->e : m->w;
    k = (m->w > m->e) ? m->w : m->e;
    i = 0;
    while (j + i <= k) {
        // printf("%d, %d, %d\n", j, l, k);
        m->terrain[j+i][l] = PTH;
        m->alt[j+i][l] = 25;
        i++;
    }
    
    //top
    b = 1;
    // printf("%d, %d\n", 0 + b, m->n);
    while (m->terrain[0 + b][m->n] != PTH) {
        // printf("%d, %d\n", 0 + m, m);
        m->terrain[0+b][m->n] = PTH;
        m->alt[0+b][m->n] = 25;
        b++;
    }

    //bottom
    b = 1;
    // printf("%d, %d\n", BOUNDS_Y-1 - b, m->s);
    while (m->terrain[BOUNDS_Y-1 - b][m->s] != PTH) {
        // printf("%d, %d\n", BOUNDS_Y-1 - b, b);
        m->terrain[BOUNDS_Y-1-b][m->s] = PTH;
        m->alt[BOUNDS_Y-1-b][m->s] = 25;
        b++;
    }
    
    //get probability
    d = manhattan(center, curPos);
    p = 100 - ((45 * d) / 200);
    if (d >= 200) {
        p = 5;
    }
    // printf("d:%d, p:%d\n", d, p);

    if ((rand() % 100) < p) {
        if(j >= 14) j -= 2;
        //Add PokemartMart
        m->terrain[j+2][l-1] = MRT;
        m->alt[j+2][l-1] = 99;
        m->terrain[j+3][l-2] = MRT;
        m->alt[j+3][l-2] = 99;
        m->terrain[j+2][l-2] = MRT;
        m->alt[j+2][l-2] = 99;
        m->terrain[j+3][l-1] = MRT;
        m->alt[j+3][l-1] = 99;
        //road
        m->terrain[j+2][l-3] = PTH;
        m->alt[j+2][l-3] = 25;
        m->terrain[j+3][l-3] = PTH;
        m->alt[j+3][l-3] = 25;
        m->terrain[j+4][l-3] = PTH;
        m->alt[j+4][l-3] = 25;
        m->terrain[j+4][l-1] = PTH;
        m->alt[j+4][l-1] = 25;
        m->terrain[j+4][l-2] = PTH;
        m->alt[j+4][l-2] = 25;
        m->terrain[j+4][l] = PTH;
        m->alt[j+4][l] = 25;
        m->terrain[j+5][l] = PTH;
        m->alt[j+5][l] = 25;
        m->terrain[j+3][l] = PTH;
        m->alt[j+3][l] = 25;
        m->terrain[j+2][l] = PTH;
        m->alt[j+2][l] = 25;
        m->terrain[j+6][l] = PTH;
        m->alt[j+6][l] = 25;
        m->terrain[j+5][l-3] = PTH;
        m->alt[j+5][l-3] = 25;
        m->terrain[j+6][l-3] = PTH;
        m->alt[j+6][l-3] = 25;
        //Pokecenter
        m->terrain[j+5][l-1] = CTR;
        m->alt[j+5][l-1] = 99;
        m->terrain[j+6][l-2] = CTR;
        m->alt[j+6][l-2] = 99;
        m->terrain[j+5][l-2] = CTR;
        m->alt[j+5][l-2] = 99;
        m->terrain[j+6][l-1] = CTR;
        m->alt[j+6][l-1] = 99;
    }

    return 0;
}

int create_map(Map *m)
{
    queue_t *qx, *qy;
    int i, j;

    // printf("setting map\n");
    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            m->terrain[i][j] = -1;
            // printf("%c", m->terrain[i][j]);
        }
        // printf("\n");
    }


    // printf("trying boundary\n");
    if (!make_boundary(m)) {
        int i, r;
        int x[2];

        // printf("Making queues\n");
        qx = qy = malloc(sizeof(queue_t));
        queue_init(qx);
        queue_init(qy);

        // printf("%d %d\n", qx->length, qy->length);

        //seed
        r = rand() % 2;
        // printf("Start Seed: %d\n", r);
        
        //tallgrass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            m->terrain[x[1]][x[0]] = GT;
            m->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        r = rand() % 2;
        //short grass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            m->terrain[x[1]][x[0]] = GS;
            m->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        //water
        x[0] = (rand() % (BOUNDS_X - 5)) + 3;
        x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
        m->terrain[x[1]][x[0]] = WTR;
        m->alt[x[1]][x[0]] = (rand() % 18);
        queue_enqueue(qx, x[0]);
        queue_enqueue(qy, x[1]);

        r = rand() % 3;

        // printf("Random Terrain start\n");
        //get seed coords
        for (i = 0; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 4;
            int a = rand() % 5;
            // printf("x: %d, y: %d, %c\n", x[0], x[1], (char) TERRAIN[a]);
            m->terrain[x[1]][x[0]] = a;
            m->alt[x[1]][x[0]] = ALTITUDE[a][0];
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        // print_display(m);
        // printf("Begin spreading\n");
        spread_seed(m, qx, qy);
        // printf("Fill Map\n");
        fill_map(m);
        // print_display(m);
        // printf("Add Paths\n");
        // pathfind(m, 0);
        trailblaze(m);
        // print_display(m);
    } else {
        printf("There was an error generating the map");
        return 1;
    }

    // printf("Begin cleanup\n");
    queue_destroy(qx);
    queue_destroy(qy);

    return 0;
}

//initializers
Trainer* init_trainer(Trainer_e te, Point p) 
{
    int x, y;
    Trainer* t = malloc(sizeof(*t));

    if (t != NULL) {
        t->e.chr = te;
        t->e.pos = p;
        t->e.move = movement[te];
        t->e.dir = get_lower_alt(p, world[curPos.y][curPos.x]);
    }

    return t;
}


//map updates
int add_trainer(Trainer *t, char map[BOUNDS_Y][BOUNDS_X][10])
{
    strcpy(map[t->e.pos.y][t->e.pos.x], (char[2]) {ALL_TRAINERS[t->e.chr], '\0'});

    return 0;
}

Trainer** add_trainers(const int num)
{
    int i = 0;
    Trainer **trainers = malloc(sizeof(Trainer) * num);

    if (num >= 2) {
        Hiker *h = init_trainer(HIKR, (Point) {.x = i + 1, .y = i + 1});
        Rival *r = init_trainer(RIVL, (Point) {.x = i + 1, .y = i + 1});
        trainers[0] = h;
        trainers[1] = r;
        i = 2;
    }

    while (i < num) {
        Trainer *t = init_trainer(1 + (rand() % (num_types_tra - 1)), (Point) {.x=1, .y=i});
        trainers[i] = t;
        i++;
    }

    return trainers;
}

int update_trails(PC *player, Trainer** t)
{
    int i;

    dijkstra(trails[PLAY], player->e.pos, &player->e);
    for (i = 1; i < num_types_tra; i++) {
        dijkstra(trails[i], player->e.pos, &t[i]->e);
    }
}

int map_chars(Map *from, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            strcpy(map[i][j], (char[2]) {ALL_TERRAIN[from->terrain[i][j]], '\0'});
            // printf("x:%d, y:%d, chr:%c\n", j, i, ALL_TERRAIN[from->terrain[i][j]]);
        }
    }

    return 0;
}


//movement utility
Dir_e change_direction(int x, Dir_e d)
{
    if (x == 0) {
        switch (d) {
            case NW:
                return SE;
            case N:
                return S;
            case NE:
                return SW;
            case W:
                return E;
            case E:
                return W;
            case SW:
                return NE;
            case S:
                return S;
            case SE:
                return NW;
            default:
                fprintf(stderr, "Error reversing direction %d", d);
                return NONE;
        }
    } else {
        return (x * rand()) % num_dir;
    }
}

//movement functions
int move_player(Entity self)
{
    return 0;
}

int move_hiker(Entity self)
{
    Map *m = trails[HIKR];
    Point q = get_next_position(self.pos, self.dir);
    Dir_e d;

    if (m->alt[q.y][q.x] <= m->alt[self.pos.y][self.pos.x] || rand() % 500 == 0) {
        self.pos = q;
    } else {
        d = get_lower_alt(self.pos, m);
        if (d > 0) {
            self.pos = get_next_position(self.pos, d);
            self.dir = d;
        } else {
            fprintf(stderr, "Hiker stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_rival(Entity self)
{
    Map *m = trails[RIVL];
    Point q = get_next_position(self.pos, self.dir);
    Dir_e d;

    if (m->alt[q.y][q.x] <= m->alt[self.pos.y][self.pos.x] || rand() % 500 == 0) {
        self.pos = q;
    } else {
        d = get_lower_alt(self.pos, m);
        if(d > 0) {
            self.pos = get_next_position(self.pos, d);
            self.dir = d;
        } else {
            fprintf(stderr, "Rival stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_pacer(Entity self)
{
    Map *m = world[curPos.y][curPos.x];
    Point q = get_next_position(self.pos, self.dir);

    if (m->terrain[self.pos.y][self.pos.x] == m->terrain[q.y][q.x]) {
        self.pos = q;
    } else {
        self.pos = get_next_position(self.pos, change_direction(0, self.dir));
    }

    return 0;
}

int move_wanderer(Entity self)
{
    Map *m = world[curPos.y][curPos.x];
    Point q = get_next_position(self.pos, self.dir);
    int i, c = 0;

    if (m->terrain[self.pos.y][self.pos.x] == m->terrain[q.y][q.x]) {
        self.pos = q;
    } else {
        for (i = 0; i < num_dir; i++) {
            q = get_next_position(self.pos, change_direction(rand(), self.dir));
            if (m->alt[q.x][q.y] && m->alt[self.pos.x][self.pos.y] && m->terrain[self.pos.y][self.pos.x] == m->terrain[q.y][q.x]) {
                self.pos = q;
                break;
            }
        }
    }

    return 0;
}

int move_sentry(Entity self)
{
    //no
    return 0;
}

int move_explorer(Entity self)
{
    Map *m = world[curPos.y][curPos.x];
    Point q = get_next_position(self.pos, self.dir);
    int i, c = 0;

    if (m->terrain[self.pos.y][self.pos.x] == m->terrain[q.y][q.x]) {
        self.pos = q;
        return 0;
    } else {
        for (i = 0; i < num_dir; i++) {
            q = get_next_position(self.pos, change_direction(rand(), self.dir));
            if (valid_pos(EXPL, m->terrain[q.y][q.x])) {
                self.pos = q;
                return 0;
            }
        }
    }

    fprintf(stderr, "Explorer stuck\n");
    return 1;
}

int move_swimmer(Entity self)
{
    Map *m = trails[SWIM];
    Point q = get_next_position(self.pos, self.dir);
    Dir_e d;

    if (m->alt[q.y][q.x] <= m->alt[self.pos.y][self.pos.x] || rand() % 10 == 0) {
        self.pos = q;
    } else {
        d = get_lower_alt(self.pos, m);
        if (d > 0) {
            self.pos = get_next_position(self.pos, d);
            self.dir = d;
        } else {
            fprintf(stderr, "Swimmer stuck\n");
            return 1;
        }
    }
}

int main(int argc, char *argv[])
{

    // printf("start\n");
    int i, j, n, s, e, w;
    int count = 0, r;
    int numTrainers = 10, moved;
    PC *player = init_trainer(PLAY, (Point) {.x=5, .y=5});
    Trainer **trainers;
    heap_t order;
    char display[BOUNDS_Y][BOUNDS_X][10];

    curPos.x = 200;
    curPos.y = 200;

    if (argc > 1 && argc != 3) {
        printf("Usage: %s --numtrainers <number of trainers to spawn <default: 10>\n", argv[0]);
        return 1;
    } else if (argc == 3 && strcmp(argv[1], "--numtrainers") == 0) {
        numTrainers = atoi(argv[2]);
    }

    // printf("initialize World\n");
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    // printf("setup Randoms\n");
    srand(time(NULL));

    heap_init(&order, entity_cmp, NULL);
    
    n = 1 + (rand() % (BOUNDS_X - 2));
    s = 1 + (rand() % (BOUNDS_X - 2));
    e = 1 + (rand() % (BOUNDS_Y - 2));
    w = 1 + (rand() % (BOUNDS_Y - 2));
    // printf("n:%d s:%d e:%d w:%d\n", n, s, e, w);

   
    world[curPos.y][curPos.x] = malloc(sizeof(*world[curPos.y][curPos.x]));
    world[curPos.y][curPos.x]->n = n;
    world[curPos.y][curPos.x]->s = s;
    world[curPos.y][curPos.x]->e = e;
    world[curPos.y][curPos.x]->w = w;
    // printf("Create Starting Map\n");
    r = create_map(world[curPos.y][curPos.x]);
    map_chars(world[curPos.y][curPos.x], display);
    player->e.pos = (Point) {.x = world[curPos.y][curPos.x]->s, .y = BOUNDS_Y - 2};
    add_trainer(player, display);

    startTer = malloc(sizeof(Terrain_e) * numTrainers);
    trainers = add_trainers(numTrainers);

    printf("NumTrainers: %d\n", numTrainers);
    for (i = 0; i < numTrainers; i++) {
        startTer[i] = world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x];
        while (!valid_pos(trainers[i]->e.chr, world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x], i)) {
            trainers[i]->e.pos = (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))};
            startTer[i] = world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x];
        }
        printf("Trainer char: %c, pos: (%d, %d)\n", ALL_TRAINERS[trainers[i]->e.chr], trainers[i]->e.pos.x, trainers[i]->e.pos.y);
        add_trainer(trainers[i], display);
    }
    update_trails(player, trainers);
    
    r = print_display(display);
    while (r == 0) {
        char in[15];
        char c;
        int nx = 0, ny = 0;
        // printf("Current Position: (%d, %d). Enter Move:  ", curPos.x - 200, curPos.y - 200);
        // fgets(in, sizeof(in), stdin);
        if (in != NULL) {
            sscanf(in, "%c %d %d", &c, &nx, &ny);
        }

        

        printf("check moves\n");
        switch (c) {
            case 'n':
                player->e.pos = (Point) {.x = world[curPos.y][curPos.x]->n, .y = BOUNDS_Y - 2 };
                curPos.y += 1;
                count++;
                break;
            case 's':
                player->e.pos = (Point) {.x = world[curPos.y][curPos.x]->s, .y = 1 };
                curPos.y -= 1;
                count++;
                break;
            case 'e':
                player->e.pos = (Point) {.x = BOUNDS_X - 2, .y = world[curPos.y][curPos.x]->e };
                curPos.x += 1;
                count++;
                break;
            case 'w':
                player->e.pos = (Point) {.x = 1, .y = world[curPos.y][curPos.x]->w };
                curPos.x -= 1;
                count++;
                break;
            case 'f':
                curPos.y = ny + 200;
                curPos.x = nx + 200;
                count++;
                break;
            case 'q':
                return 0;
            default:
                if (c == '\0' || c == '\n' || c == NULL) {
                    printf("\n");
                    break;
                }
                printf("Command Not Found\n");
                break;
        }

        printf("valid check\n");
        while (curPos.y >= WORLD_SIZE) {
            curPos.y -= 1;
        }
        while (curPos.x >= WORLD_SIZE) {
            curPos.x -= 1;
        }
        while (curPos.y < 0) {
            curPos.y += 1;
        }
        while (curPos.x < 0) {
            curPos.x += 1;
        }

        printf("Set Gates\n");
        n = 1 + (rand() % (BOUNDS_X - 3));
        s = 1 + (rand() % (BOUNDS_X - 3));
        e = 1 + (rand() % (BOUNDS_Y - 3));
        w = 1 + (rand() % (BOUNDS_Y - 3));
        // printf("n: %d, s: %d, e: %d, w: %d\n", n, s, e, w);

        // printf("%d\n", curPos.y);
        if (curPos.y > 0 && world[curPos.y - 1][curPos.x] != NULL) {
            // printf("N->s:%d, ", world[curPos.y - 1][curPos.x]->n);
            s = world[curPos.y - 1][curPos.x]->n;
        }
        // printf("-1");
        if (curPos.y < (WORLD_SIZE - 1) && world[curPos.y + 1][curPos.x] != NULL) {
            // printf("S->n:%d, ", world[curPos.y + 1][curPos.x]->s);
            n = world[curPos.y + 1][curPos.x]->s;
        }
        // printf("-2");
        // printf("%d\n", curPos.x);
        if (curPos.x < (WORLD_SIZE - 1) && world[curPos.y][curPos.x + 1] != NULL) {
            // printf("E->w:%d, ", world[curPos.y][curPos.x + 1]->w);
            e = world[curPos.y][curPos.x + 1]->w;
        }
        // printf("-3");
        if (curPos.x > 0 && world[curPos.y][curPos.x - 1] != NULL) {
            // printf("W->e:%d", world[curPos.y][curPos.x - 1]->e);
            w = world[curPos.y][curPos.x - 1]->e;
        }
        // printf("-4");
        // printf("\n");
        // printf("n: %d, s: %d, e: %d, w: %d\n", n, s, e, w);

        printf("nullcheck spot\n");
        if (world[curPos.y][curPos.x] == NULL) {
            world[curPos.y][curPos.x] = malloc(sizeof(*world[curPos.y][curPos.x]));
            world[curPos.y][curPos.x]->n = n;
            world[curPos.y][curPos.x]->s = s;
            world[curPos.y][curPos.x]->e = e;
            world[curPos.y][curPos.x]->w = w;
            // check_map(world[curPos.y][curPos.x]);
            r = create_map(world[curPos.y][curPos.x]);
        }

        map_chars(world[curPos.y][curPos.x], display);

        add_trainer(player, display);
        printf("NumTrainers: %d\n", numTrainers);
        for (i = 0; i < numTrainers; i++) {
            startTer[i] = world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x];
            while (!valid_pos(trainers[i]->e.chr, world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x], i)) {
                trainers[i]->e.pos = (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))};
                startTer[i] = world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x];
            }
            printf("Trainer char: %c, pos: (%d, %d)\n", ALL_TRAINERS[trainers[i]->e.chr], trainers[i]->e.pos.x, trainers[i]->e.pos.y);
            add_trainer(trainers[i], display);
        }

        if (moved) {
            update_trails(player, trainers);
        }

        print_display(display);
        moved = 0;
    }
    
    for (i = 0; i < numTrainers; i++) {
        free(trainers[i]);
    }
    
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            if (world[i][j] != NULL) {
                free(world[i][j]);
            }
        }
    }
    return 0;
}