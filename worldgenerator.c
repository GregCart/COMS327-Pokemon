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

#define COLOR_RESET "\x1b[0m"
#define COLOR_ROCK  "\x1b[37m"
#define COLOR_TREE  "\x1b[92m"
#define COLOR_GRASS_S  "\x1b[92m"
#define COLOR_GRASS_T  "\x1b[32m"
#define COLOR_WATER  "\x1b[94m"
#define COLOR_MART  "\x1b[91m"
#define COLOR_CENTER  "\x1b[93m"
#define COLOR_PATH  "\x1b[33m"

#define PLAYER  '@'
#define RIVAL   'R'
#define HIKER   'H'
#define SWIMMER 'M'
#define WANDERER    'W'


typedef enum terrain_e {
    R,
    T,
    GT,
    W,
    GS,
    P,
    M,
    C,
    num_types_ter
} Terrain_e;

typedef enum trainer_e {
    PLAY,
    RIVL,
    HIKR,
    SWIM,
    WAND,
    num_types_tra
} Trainer_e;

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
    Trainer_e chr;
    //rock, tree, tgrass, water, sgrass, gate, path, mart, center
    int stress[9];
} Entity;

typedef struct trainer {
    Entity e;
} Trainer;

typedef struct player {
    Trainer *t;
    Map *trail;
} PC;

const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, PATH, MART, CENTER};
const char* ALL_COLORS[] = {COLOR_ROCK, COLOR_TREE, COLOR_GRASS_T, COLOR_WATER, COLOR_GRASS_S, COLOR_PATH, COLOR_MART, COLOR_CENTER};
const char ALL_TRAINERS[] = {PLAYER, RIVAL, HIKER, SWIMMER, WANDERER};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};
const int STRESS[] = {20, 50, 15, 40, 10};
const Point center = {.x = 200, .y = 200};

//sorted in {y, x}
// int EXITS[4][2];
Map *world[WORLD_SIZE][WORLD_SIZE];
Point curPos;


int print_map(char map[BOUNDS_Y][BOUNDS_X][10])
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

int print_cost_map(Map *m) 
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (m->alt[i][j] < 999999) {
                printf("%02d ", m->alt[i][j] % 100);
            } else {
                printf("   ");
            }
            
        }
        printf("\n");
    }

    return 0;
}

int check_map(Map *m) {
    printf("terrain: %ld x %ld\n", sizeof(m->terrain[0])/sizeof(Terrain_e), sizeof(m->terrain)/sizeof(m->terrain[0])/sizeof(char));
    printf("n:%d s:%d e:%d w:%d\n", m->n, m->s, m->e, m->w);

    return 0;
}

static int32_t path_cmp(const void *key, const void *with) {
  return ((Path *) key)->cost - ((Path *) with)->cost;
}

int manhattan(Point p, Point q)
{
    int ret;
    // printf("(%d, %d) and (%d, %d)\n", p.x, p.y, q.x, q.y);
    ret = abs(p.x - q.x) + abs(p.y - q.y);
    // printf("ret:%d\n", ret);
    return ret;
}

int get_stress(Map *m, Entity *e, Point p) {
    int ret;

    switch (ALL_TERRAIN[m->terrain[p.y][p.x]]) {
        case ROCK:
            if (m->alt[p.y][p.x] < 99) {
                ret = e->stress[0];
            } else {
                ret = D_MAX;
            }
            break;
        case TREE:
            ret = e->stress[1];
            break;
        case GRASS_T:
            ret = e->stress[2];
            break;
        case WATER:
            ret = e->stress[3];
            break;
        case GRASS_S:
            ret = e->stress[4];
            break;
        case PATH:
            if (p.x == 0 || p.x == BOUNDS_X - 1 || p.y == 0 || p.y == BOUNDS_Y - 1) {
                ret = e->stress[5];
            } else {
                ret = e->stress[6];
            }
            break;
        case MART:
            ret = e->stress[7];
            break;
        case CENTER:
            ret = e->stress[8];
            break;
        default:
            ret = D_MAX;
            break;
    }

    return ret;
}

static int dijkstra(Map *m, Point p, Entity *e)
{
    // printf("Start Dijk's\n");
    static Path path[BOUNDS_Y][BOUNDS_X], *pth;
    static int init = 0;
    heap_t h;
    int x, y;
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

    // printf("fill heap\n");
    for (y = 1; y < BOUNDS_Y - 1; y++) {
        for (x = 1; x < BOUNDS_X - 1; x++) {
            path[y][x].hn = heap_insert(&h, &path[y][x]);
        }
    }

    // printf("loop start\n");
    while ((pth = heap_remove_min(&h))) {
        pth->hn = NULL;

        // printf("check above\n");//above
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] - 1};
        if ((path[pth->pos[0] - 1][pth->pos[1]].hn) && w->alt[pth->pos[0] - 1][pth->pos[1]] < 50 
            && (path[pth->pos[0] - 1][pth->pos[1]].cost > ((pth->cost + get_stress(w, e, pnt))))) {
                path[pth->pos[0] - 1][pth->pos[1]].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] - 1][pth->pos[1]].from[0] = pth->pos[0];
                path[pth->pos[0] - 1][pth->pos[1]].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] - 1][pth->pos[1]].hn);
        }
        // printf("check TL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] - 1};
        if ((path[pth->pos[0] - 1][pth->pos[1] - 1].hn) && w->alt[pth->pos[0] - 1][pth->pos[1] - 1] < 50 
            && (path[pth->pos[0] - 1][pth->pos[1] - 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //TL
                path[pth->pos[0] - 1][pth->pos[1] - 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] - 1][pth->pos[1] - 1].from[0] = pth->pos[0];
                path[pth->pos[0] - 1][pth->pos[1] - 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] - 1][pth->pos[1] - 1].hn);
        }
        // printf("check TR\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] + 1};
        if ((path[pth->pos[0] - 1][pth->pos[1] + 1].hn) && w->alt[pth->pos[0] - 1][pth->pos[1] + 1] < 50
            && (path[pth->pos[0] - 1][pth->pos[1] + 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //TR
                path[pth->pos[0] - 1][pth->pos[1] + 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] - 1][pth->pos[1] + 1].from[0] = pth->pos[0];
                path[pth->pos[0] - 1][pth->pos[1] + 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] - 1][pth->pos[1] + 1].hn);
        }
        // printf("check left\n");
        if ((path[pth->pos[0]][pth->pos[1] - 1].hn) && w->alt[pth->pos[0]][pth->pos[1] - 1] < 50
            && (path[pth->pos[0]][pth->pos[1] - 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //left
                path[pth->pos[0]][pth->pos[1] - 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0]][pth->pos[1] - 1].from[0] = pth->pos[0];
                path[pth->pos[0]][pth->pos[1] - 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0]][pth->pos[1] - 1].hn);
        }
        // printf("check right\n");
        if ((path[pth->pos[0]][pth->pos[1] + 1].hn) && w->alt[pth->pos[0]][pth->pos[1] + 1] < 50
            && (path[pth->pos[0]][pth->pos[1] + 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //right
                path[pth->pos[0]][pth->pos[1] + 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0]][pth->pos[1] + 1].from[0] = pth->pos[0];
                path[pth->pos[0]][pth->pos[1] + 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0]][pth->pos[1] + 1].hn);
        }
        // printf("check bottom\n");
        if ((path[pth->pos[0] + 1][pth->pos[1]].hn) && w->alt[pth->pos[0] + 1][pth->pos[1]] < 50
            && (path[pth->pos[0] + 1][pth->pos[1]].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //bottom
                path[pth->pos[0] + 1][pth->pos[1]].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] + 1][pth->pos[1]].from[0] = pth->pos[0];
                path[pth->pos[0] + 1][pth->pos[1]].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] + 1][pth->pos[1]].hn);
        }
        // printf("check BL\n");
        if ((path[pth->pos[0] + 1][pth->pos[1] - 1].hn) && w->alt[pth->pos[0] + 1][pth->pos[1] - 1] < 50
            && (path[pth->pos[0] + 1][pth->pos[1] - 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //BL
                path[pth->pos[0] + 1][pth->pos[1] - 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] + 1][pth->pos[1] - 1].from[0] = pth->pos[0];
                path[pth->pos[0] + 1][pth->pos[1] - 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] + 1][pth->pos[1] - 1].hn);
        }
        // printf("check BR\n");
        if ((path[pth->pos[0] + 1][pth->pos[1] + 1].hn) && w->alt[pth->pos[0] + 1][pth->pos[1] + 1] < 50
            && (path[pth->pos[0] + 1][pth->pos[1] + 1].cost > ((pth->cost + get_stress(w, e, pnt))))) {    //BR
                path[pth->pos[0] + 1][pth->pos[1] + 1].cost = pth->cost + get_stress(w, e, pnt);
                //from stuff for drawing nodes, don't need for part 3
                path[pth->pos[0] + 1][pth->pos[1] + 1].from[0] = pth->pos[0];
                path[pth->pos[0] + 1][pth->pos[1] + 1].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pth->pos[0] + 1][pth->pos[1] + 1].hn);
        }

    }

    // printf("fill cost map\n");
    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            // printf("X: %d, y:%d\n", x, y);
            m->alt[y][x] = path[y][x].cost;
        }
    }

    return 0;
}


int make_boundary(Map *m)
{
    int i;

    int x[4] = {m->n, m->s, m->e, m->w};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        m->terrain[0][i] = R;
        m->terrain[BOUNDS_Y-1][i] = R;
        m->alt[0][i] = 99;
        m->alt[BOUNDS_Y-1][i] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        m->terrain[i][0] = R;
        m->terrain[i][BOUNDS_X-1] = R;
        m->alt[i][0] = 99;
        m->alt[i][BOUNDS_X-1] = 99;
    }

    //set paths
    if (curPos.x > 2) {
        m->alt[x[3]][0] = 25;
        m->terrain[x[3]][0] = P;
        m->w = x[3];
    }
    if (curPos.x < WORLD_SIZE - 1) {
        m->alt[x[2]][BOUNDS_X-1] = 25;
        m->terrain[x[2]][BOUNDS_X-1] = P;
        m->e = x[2]-1;
    }
    if (curPos.y < WORLD_SIZE - 1) {
        m->alt[BOUNDS_Y-1][x[1]] = 25;
        m->terrain[BOUNDS_Y-1][x[1]] = P;
        m->s = x[1];
    }
    if (curPos.y > 1) {
        m->alt[0][x[0]] = 25;
        m->terrain[0][x[0]] = P;
        m->n = x[0];
    }
    // print_map(m);
    
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
                *chr = T;
            } else if(tmp == 0) {
                *chr = R;
            } else {
                *chr = GS;
            }
        } else if (*alt > 30) {
            int tmp = (rand() % 4);
            if (tmp == 1) {
                *chr = T;
            } else if(tmp == 0) {
                *chr = R;
            } else if (tmp == 2) {
                *chr = GT;
            } else {
                *chr = GS;
            }
        } else if (*alt > 25) {
            int tmp = (rand() % 3);
            if (tmp == 1) {
                *chr = T;
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
                *chr = W;
            }
        } else {
            *chr = W;
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
        // print_map(map, 0);
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
        m->terrain[m->w][1+i] = P;
        m->alt[m->w][1+i] = 25;
    }
    l = i - 1;
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", m->e, 0+i);
        m->terrain[m->e][BOUNDS_X-1+i] = P;
        m->alt[m->e][BOUNDS_X-1+i] = 25;
        i++;
    }
    //connect left and right
    j = (m->w > m->e) ? m->e : m->w;
    k = (m->w > m->e) ? m->w : m->e;
    i = 0;
    while (j + i < k) {
        // printf("%d, %d, %d\n", j, l, k);
        m->terrain[j+i][l] = P;
        m->alt[j+i][l] = 25;
        i++;
    }
    
    //top
    b = 1;
    // printf("%d, %d\n", 0 + b, m->n);
    while (m->terrain[0 + b][m->n] != P) {
        // printf("%d, %d\n", 0 + m, m);
        m->terrain[0+b][m->n] = P;
        m->alt[0+b][m->n] = 25;
        b++;
    }

    //bottom
    b = 1;
    // printf("%d, %d\n", BOUNDS_Y-1 - b, m->s);
    while (m->terrain[BOUNDS_Y-1 - b][m->s] != P) {
        // printf("%d, %d\n", BOUNDS_Y-1 - b, b);
        m->terrain[BOUNDS_Y-1-b][m->s] = P;
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
        m->terrain[j+2][l-1] = M;
        m->alt[j+2][l-1] = 99;
        m->terrain[j+3][l-2] = M;
        m->alt[j+3][l-2] = 99;
        m->terrain[j+2][l-2] = M;
        m->alt[j+2][l-2] = 99;
        m->terrain[j+3][l-1] = M;
        m->alt[j+3][l-1] = 99;
        //road
        m->terrain[j+2][l-3] = P;
        m->alt[j+2][l-3] = 25;
        m->terrain[j+3][l-3] = P;
        m->alt[j+3][l-3] = 25;
        m->terrain[j+4][l-3] = P;
        m->alt[j+4][l-3] = 25;
        m->terrain[j+4][l-1] = P;
        m->alt[j+4][l-1] = 25;
        m->terrain[j+4][l-2] = P;
        m->alt[j+4][l-2] = 25;
        m->terrain[j+4][l] = P;
        m->alt[j+4][l] = 25;
        m->terrain[j+5][l] = P;
        m->alt[j+5][l] = 25;
        m->terrain[j+3][l] = P;
        m->alt[j+3][l] = 25;
        m->terrain[j+2][l] = P;
        m->alt[j+2][l] = 25;
        m->terrain[j+6][l] = P;
        m->alt[j+6][l] = 25;
        m->terrain[j+5][l-3] = P;
        m->alt[j+5][l-3] = 25;
        m->terrain[j+6][l-3] = P;
        m->alt[j+6][l-3] = 25;
        //Pokecenter
        m->terrain[j+5][l-1] = C;
        m->alt[j+5][l-1] = 99;
        m->terrain[j+6][l-2] = C;
        m->alt[j+6][l-2] = 99;
        m->terrain[j+5][l-2] = C;
        m->alt[j+5][l-2] = 99;
        m->terrain[j+6][l-1] = C;
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
        m->terrain[x[1]][x[0]] = W;
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

        // print_map(m);
        // printf("Begin spreading\n");
        spread_seed(m, qx, qy);
        // printf("Fill Map\n");
        fill_map(m);
        // print_map(m);
        // printf("Add Paths\n");
        // pathfind(m, 0);
        trailblaze(m);
        // print_map(m);
    } else {
        printf("There was an error generating the map");
        return 1;
    }

    // printf("Begin cleanup\n");
    queue_destroy(qx);
    queue_destroy(qy);

    return 0;
}

int add_trainer(Trainer *t, char map[BOUNDS_Y][BOUNDS_X][10])
{
    strcpy(map[t->e.pos.y][t->e.pos.x], (char[2]) {ALL_TRAINERS[t->e.chr], '\0'});

    return 0;
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

int main(int argc, char *argv[])
{

    // printf("start\n");
    int i, j, n, s, e, w;
    int count = 0, r;
    PC *player = malloc(sizeof(*player));
    Entity ent = {
        .chr = PLAY, 
        .pos = {.x = 5, .y = 5}, 
        .stress = {D_MAX, D_MAX, 20, D_MAX, 10, 10, 10, 10, 10}
    };
    char display[BOUNDS_Y][BOUNDS_X][10];
    player->t = malloc(sizeof(*player->t));
    player->trail = malloc(sizeof(player->trail));
    player->t->e = ent;
    curPos.x = 200;
    curPos.y = 200;

    // printf("initialize World\n");
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    // printf("setup Randoms\n");
    srand(time(NULL));

    
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
    // check_map(world[curPos.y][curPos.x]);
    map_chars(world[curPos.y][curPos.x], display);
    // display->terrain[PC->t->e.pos.y][PC->t->e.pos.x] = PC->t->e.chr;
    // printf("x:%d, y:%d\n", PC->t->e.pos.x, PC->t->e.pos.y);
    // printf("DO print\n");
    player->t->e.pos = (Point) {.x = world[curPos.y][curPos.x]->s, .y = BOUNDS_Y - 3};
    add_trainer(player->t, display);
    r = print_map(display);
    dijkstra(player->trail, player->t->e.pos, &player->t->e);
    print_cost_map(player->trail);
    while (r == 0) {
        char in[15];
        char c;
        int nx = 0, ny = 0;
        printf("Current Position: (%d, %d). Enter Move:  ", curPos.x - 200, curPos.y - 200);
        fgets(in, sizeof(in), stdin);
        // printf("%c %d %d\n", in[0], in[2], in[4]);
        sscanf(in, "%c %d %d", &c, &nx, &ny);
        // printf("c:%c x:%d y:%d\n", c, nx, ny);

        // printf("check moves\n");
        switch (c) {
            case 'n':
                curPos.y += 1;
                player->t->e.pos = (Point) {.x = world[curPos.y][curPos.x]->n, .y = BOUNDS_Y - 2 };
                count++;
                break;
            case 's':
                curPos.y -= 1;
                player->t->e.pos = (Point) {.x = world[curPos.y][curPos.x]->s, .y = 1 };
                count++;
                break;
            case 'e':
                curPos.x += 1;
                player->t->e.pos = (Point) {.x = BOUNDS_X - 2, .y = world[curPos.y][curPos.x]->e };
                count++;
                break;
            case 'w':
                curPos.x -= 1;
                player->t->e.pos = (Point) {.x = 1, .y = world[curPos.y][curPos.x]->w };
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
                if (c == '\0' || c == '\n') {
                    printf("\n");
                    break;
                }
                printf("Command Not Found\n");
                break;
        }

        // printf("valid check\n");
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

        // printf("Set Gates\n");
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

        // printf("nullcheck spot\n");
        if (world[curPos.y][curPos.x] == NULL) {
            world[curPos.y][curPos.x] = malloc(sizeof(*world[curPos.y][curPos.x]));
            world[curPos.y][curPos.x]->n = n;
            world[curPos.y][curPos.x]->s = s;
            world[curPos.y][curPos.x]->e = e;
            world[curPos.y][curPos.x]->w = w;
            // check_map(world[curPos.y][curPos.x]);
            r = create_map(world[curPos.y][curPos.x]);
        }
        
        add_trainer(player->t, display);

        print_map(display);
        dijkstra(player->trail, player->t->e.pos, &player->t->e);
        print_cost_map(player->trail);
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