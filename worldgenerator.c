#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lib/queue.c"


#define WORLD_SIZE 401
#define BOUNDS_X 80
#define BOUNDS_Y 21
#define ROCK '%'
#define TREE '^'
#define GRASS_S '.'
#define GRASS_T ':'
#define WATER '~'
#define PATH '#'


typedef struct map {
    char terrain[BOUNDS_Y][BOUNDS_X];
    int alt[BOUNDS_Y][BOUNDS_X];
    int px, py;
    int n, s, e, w;
} Map;

typedef struct point {
    int x, y;
} Point;


const int TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};
const Point center = {.x = 200, .y = 200};

//sorted in {y, x}
int EXITS[4][2];
Map *world[WORLD_SIZE][WORLD_SIZE];
Point curPos;


int print_map(Map *m)
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%c", m->terrain[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int check_map(Map *m) {
    printf("terrain: %ld x %ld\n", sizeof(m->terrain[0])/sizeof(char), sizeof(m->terrain)/sizeof(m->terrain[0])/sizeof(char));
    printf("n:%d s:%d e:%d w:%d\n", m->n, m->s, m->e, m->w);

    return 0;
}

int minDistance(int dist[], int sptSet[])
{
    int min = 99999, min_index, v;
    // printf("finding min\n");
    for (v = 0; v < BOUNDS_X; v++) {
        if (sptSet[v] == 0 && dist[v] <= min) {
            min = dist[v], min_index = v;
        }
    }
    
    printf("Min Distance: %d\n", min_index);
    return min_index;
}

int printSolution(int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < BOUNDS_X; i++)
        printf("%d \t\t %d\n", i, dist[i]);
        
    return 0;
}

int manhattan(Point p, Point q)
{
    int ret;
    // printf("(%d, %d) and (%d, %d)\n", p.x, p.y, q.x, q.y);
    ret = abs(p.x - q.x) + abs(p.y - q.y);
    // printf("ret:%d\n", ret);
    return ret;
}

int dijkstra(Map *m, Point p) {
    int i, j, k;

    for (i = 0; i < BOUNDS_Y - 1; i++) {
        for (j = 0; j < BOUNDS_X - 1; j++) {
            m->alt[i][j] = 99999;
        }
    }

    m->alt[p.y][p.x] = 0;

    i = 0, j = 3;

    while (j < (BOUNDS_X/2) - 1) {

    }

    return 0;
}


int make_boundary(Map *m)
{
    int i;

    int x[4] = {m->n, m->s, m->e, m->w};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        m->terrain[0][i] = ROCK;
        m->terrain[BOUNDS_Y-1][i] = ROCK;
        m->alt[0][i] = 99;
        m->alt[BOUNDS_Y-1][i] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        m->terrain[i][0] = ROCK;
        m->terrain[i][BOUNDS_X-1] = ROCK;
        m->alt[i][0] = 99;
        m->alt[i][BOUNDS_X-1] = 99;
    }

    //set paths
    if (curPos.x > 2) {
        m->alt[x[3]][0] = 25;
        m->terrain[x[3]][0] = PATH;
        m->w = x[3];
    }
    if (curPos.x < WORLD_SIZE - 1) {
        m->alt[x[2]][BOUNDS_X-1] = 25;
        m->terrain[x[2]][BOUNDS_X-1] = PATH;
        m->e = x[2]-1;
    }
    if (curPos.y < WORLD_SIZE - 1) {
        m->alt[BOUNDS_Y-1][x[1]] = 25;
        m->terrain[BOUNDS_Y-1][x[1]] = PATH;
        m->s = x[1];
    }
    if (curPos.y > 1) {
        m->alt[0][x[0]] = 25;
        m->terrain[0][x[0]] = PATH;
        m->n = x[0];
    }
    // print_map(m);
    
    return 0;
}

int add_terrain(Map *m, int x, int y, char c)
{
    char *chr = &m->terrain[y][x];
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
                *chr = TREE;
            } else if(tmp == 0) {
                *chr = ROCK;
            } else {
                *chr = GRASS_S;
            }
        } else if (*alt > 30) {
            int tmp = (rand() % 4);
            if (tmp == 1) {
                *chr = TREE;
            } else if(tmp == 0) {
                *chr = ROCK;
            } else if (tmp == 2) {
                *chr = GRASS_T;
            } else {
                *chr = GRASS_S;
            }
        } else if (*alt > 25) {
            int tmp = (rand() % 3);
            if (tmp == 1) {
                *chr = TREE;
            } else if (tmp == 2) {
                *chr = GRASS_T;
            } else {
                *chr = GRASS_S;
            }
        } else if (*alt > 18) {
            int tmp = (rand() % 2);
            if (tmp == 1) {
                *chr = GRASS_T;
            } else {
                *chr = GRASS_S;
            }
        } else if (*alt > 15) {
            int tmp = (rand() % 2);
            if (tmp == 1) {
                *chr = GRASS_T;
            } else {
                *chr = WATER;
            }
        } else {
            *chr = WATER;
        }
    }

    return 0;
}

int spread_seed(Map *m, queue_t *qx, queue_t *qy)
{
    // printf("Begin spreading\n");
    int i, x, y;
    char chr;

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
                if (m->alt[y][x+i] < 50 && m->terrain[y][x+i] == ' ') {
                    queue_enqueue(qx, x+i);
                    queue_enqueue(qy, y);
                    add_terrain(m, x+i, y, chr);
                }
            }
            if (m->alt[y+1][x+i] < 50 && m->terrain[y+1][x+i] == ' ') {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y+1);
                add_terrain(m, x+i, y+1, chr);
            }
            if (m->alt[y-1][x+i] < 50 && m->terrain[y-1][x+i] == ' ') {
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
            if (m->terrain[i][j] == ' ') {
                m->terrain[i][j] = '.';
                m->alt[i][j] = 25;
            }
        }
    }

    return 0;
}

int pathfind(Map *m, int src)
{
    int i, j, k, u;
    int dist[BOUNDS_X], sptSet[BOUNDS_X];

    for (i = 0; i < BOUNDS_X; i++) {
        dist[i] = 99999;
        sptSet[i] = 0;
    }
    
    dist[src] = 0;  

    // printf("entering loop\n");
    for (k = 0; k < (BOUNDS_X - 1); k++) {
        u = minDistance(dist, sptSet);
        // printf("u: %d\n", u);
        sptSet[u] = 1;
        for (j = 0; j < (BOUNDS_X-1); j++) {
            printf("%d, %d\n", u, j);
            if (!sptSet[j] && u < BOUNDS_Y && m->alt[u][j] && dist[u] != 99999 && dist[u] + m->alt[u][j] < dist[j]) {
                dist[j] = dist[u] + m->alt[u][j];
                // printf("dist[j]: %d\n", dist[j]);
            }
        }
    }

    printSolution(dist, BOUNDS_X);

    return 0;
}

int trailblaze(Map *m)
{
    int i, j, k, l, b, d, p, u = BOUNDS_X/2 + (rand() % 20);
    // printf("%d\n", u);

    // print_map(m);
    //left
    for (i = 0; i < u; i++) {
        // printf("%d, %d\n", m->w, m->w+i);
        m->terrain[m->w][1+i] = PATH;
        m->alt[m->w][1+i] = 25;
        // print_map(m);
    }
    // print_map(m);
    l = i - 1;
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", m->e, 0+i);
        m->terrain[m->e][BOUNDS_X-1+i] = PATH;
        m->alt[m->e][BOUNDS_X-1+i] = 25;
        i++;
    }
    // print_map(m);
    //connect left and right
    j = (m->w > m->e) ? m->e : m->w;
    k = (m->w > m->e) ? m->w : m->e;
    i = 0;
    // print_map(m);
    while (j + i <= k) {
        // printf("%d, %d, %d\n", j, l, k);
        m->terrain[j+i][l] = PATH;
        m->alt[j+i][l] = 25;
        i++;
    }
    
    // print_map(m);
    //top
    b = 1;
    // printf("%d, %d\n", 0 + b, m->n);
    while (m->terrain[0 + b][m->n] != '#') {
        // printf("%d, %d\n", 0 + m, m);
        m->terrain[0+b][m->n] = '#';
        m->alt[0+b][m->n] = 25;
        b++;
    }

    // print_map(m);
    //bottom
    b = 1;
    // printf("%d, %d\n", BOUNDS_Y-1 - b, m->s);
    while (m->terrain[BOUNDS_Y-1 - b][m->s] != '#') {
        // printf("%d, %d\n", BOUNDS_Y-1 - b, b);
        m->terrain[BOUNDS_Y-1-b][m->s] = '#';
        m->alt[BOUNDS_Y-1-b][m->s] = 25;
        b++;
    }
    // print_map(m);

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
        m->terrain[j+2][l-1] = 'M';
        m->alt[j+2][l-1] = 99;
        m->terrain[j+3][l-2] = 'M';
        m->alt[j+3][l-2] = 99;
        m->terrain[j+2][l-2] = 'M';
        m->alt[j+2][l-2] = 99;
        m->terrain[j+3][l-1] = 'M';
        m->alt[j+3][l-1] = 99;
        //road
        m->terrain[j+2][l-3] = '#';
        m->alt[j+2][l-3] = 25;
        m->terrain[j+3][l-3] = '#';
        m->alt[j+3][l-3] = 25;
        m->terrain[j+4][l-3] = '#';
        m->alt[j+4][l-3] = 25;
        m->terrain[j+4][l-1] = '#';
        m->alt[j+4][l-1] = 25;
        m->terrain[j+4][l-2] = '#';
        m->alt[j+4][l-2] = 25;
        m->terrain[j+4][l] = '#';
        m->alt[j+4][l] = 25;
        m->terrain[j+5][l] = '#';
        m->alt[j+5][l] = 25;
        m->terrain[j+3][l] = '#';
        m->alt[j+3][l] = 25;
        m->terrain[j+2][l] = '#';
        m->alt[j+2][l] = 25;
        m->terrain[j+6][l] = '#';
        m->alt[j+6][l] = 25;
        m->terrain[j+5][l-3] = '#';
        m->alt[j+5][l-3] = 25;
        m->terrain[j+6][l-3] = '#';
        m->alt[j+6][l-3] = 25;
        //Pokecenter
        m->terrain[j+5][l-1] = 'C';
        m->alt[j+5][l-1] = 99;
        m->terrain[j+6][l-2] = 'C';
        m->alt[j+6][l-2] = 99;
        m->terrain[j+5][l-2] = 'C';
        m->alt[j+5][l-2] = 99;
        m->terrain[j+6][l-1] = 'C';
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
            m->terrain[i][j] = ' ';
            // printf("%c", m->terrain[i][j]);
        }
        // printf("\n");
    }
    // print_map(m);


    // printf("trying boundary\n");
    if (!make_boundary(m)) {
        // print_map(m);
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
            m->terrain[x[1]][x[0]] = GRASS_T;
            m->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        r = rand() % 2;
        //short grass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            m->terrain[x[1]][x[0]] = GRASS_S;
            m->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        //water
        x[0] = (rand() % (BOUNDS_X - 5)) + 3;
        x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
        m->terrain[x[1]][x[0]] = WATER;
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
            m->terrain[x[1]][x[0]] = TERRAIN[a];
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


int main(int argc, char *argv[])
{
    // printf("start\n");
    int i, j, n, s, e, w;
    int count = 0, r;
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


    // printf("Begin Game\n");
    r = print_map(world[curPos.y][curPos.x]);
    while (r == 0) {
        char in[15];
        char c;
        int nx = 0, ny = 0;
        printf("Current Position: (%d, %d). Enter Move:  ", curPos.x - 200, curPos.y - 200);
        fgets(in, sizeof(in), stdin);
        // printf("%c %d %d\n", in[0], in[2], in[4]);
        sscanf(in, "%c %d %d", &c, &nx, &ny);
        // printf("c:%c x:%d y:%d\n", c, nx, ny);

        n = 1 + (rand() % (BOUNDS_X - 3));
        s = 1 + (rand() % (BOUNDS_X - 3));
        e = 1 + (rand() % (BOUNDS_Y - 3));
        w = 1 + (rand() % (BOUNDS_Y - 3));
        // printf("n: %d, s: %d, e: %d, w: %d\n", n, s, e, w);

        // printf("check moves\n");
        switch (c) {
            case 'n':
                curPos.y += 1;
                count++;
                break;
            case 's':
                curPos.y -= 1;
                count++;
                break;
            case 'e':
                curPos.x += 1;
                count++;
                break;
            case 'w':
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

        print_map(world[curPos.y][curPos.x]);
    }
    
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            free(world[i][j]);
        }
    }
    return 0;
}