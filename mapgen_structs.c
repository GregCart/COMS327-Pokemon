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

const int TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};

//sorted in {y, x}
int EXITS[4][2];

typedef struct map {
    char terrain[BOUNDS_Y][BOUNDS_X];
    int alt[BOUNDS_Y][BOUNDS_X];
    int px, py;
    int n, s, e, w;
} Map;

//y, x
Map *world[WORLD_SIZE][WORLD_SIZE];
int curPos[2] = {200, 200};

int print_map(Map *map)
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%c", map->terrain[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int make_boundary(Map *m)
{
    int i;

    int x[4] = {(rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1};
    
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
    m->alt[x[3]][0] = 25;
    m->terrain[x[3]][0] = PATH;
    m->w = x[3];
    m->alt[x[2]][BOUNDS_X-1] = 25;
    m->terrain[x[2]][BOUNDS_X-1] = PATH;
    m->e = x[2];
    m->alt[BOUNDS_Y-1][x[0]] = 25;
    m->terrain[BOUNDS_Y-1][x[0]] = PATH;
    m->s = x[0];
    m->alt[0][x[1]] = 25;
    m->terrain[0][x[1]] = PATH;
    m->n = x[1];

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
    int i, j, k, l, b, u = BOUNDS_X/2 + rand() % 20;

    // printf("TRAILS\n");
    //left
    for (i = 0; i < u; i++) {
        // printf("%d, %d\n", EXITS[2][0], EXITS[2][1]+i);
        m->terrain[EXITS[3][0]][EXITS[3][1]+i] = '#';
        m->terrain[EXITS[3][0]][EXITS[3][1]+i] = 25;
    }
    l = i - 1;
    // printf("TRAILS\n");
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", EXITS[3][0], EXITS[3][1]+i);
        m->terrain[EXITS[2][0]][EXITS[2][1]+i] = '#';
        m->alt[EXITS[2][0]][EXITS[2][1]+i] = 25;
        i++;
    }
    //connect left and right
    j = (EXITS[3][0] > EXITS[2][0]) ? EXITS[2][0] : EXITS[3][0];
    k = (EXITS[3][0] > EXITS[2][0]) ? EXITS[3][0] : EXITS[2][0];
    i = 0;
    // printf("TRAILS\n");
    while (j + i <= k) {
        // printf("%d, %d, %d\n", j, l, k);
        m->terrain[j+i][l] = '#';
        m->alt[j+i][l] = 25;
        i++;
    }
    
    // printf("TRAILS\n");
    //top
    b = 1;
    // printf("%d, %d\n", EXITS[1][0] + b, EXITS[1][1]);
    while (m->terrain[EXITS[1][0] + b][EXITS[1][1]] != '#') {
        // printf("%d, %d\n", EXITS[1][0] + m, m);
        m->terrain[EXITS[1][0]+b][EXITS[1][1]] = '#';
        m->alt[EXITS[1][0]+b][EXITS[1][1]] = 25;
        b++;
    }

    // printf("TRAILS\n");
    //bottom
    b = 1;
    // printf("%d, %d\n", EXITS[0][0] - b, EXITS[0][1]);
    while (m->terrain[EXITS[0][0] - b][EXITS[0][1]] != '#') {
        printf("%d, %d\n", EXITS[0][0] - b, b);
        m->terrain[EXITS[0][0]-b][EXITS[0][1]] = '#';
        m->alt[EXITS[0][0]-b][EXITS[0][1]] = 25;
        b++;
    }

    printf("TRAILS\n");
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
    if(j >= 16) j -= 2;
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

    return 0;
}

int create_map(Map *m)
{
    queue_t *qx, *qy;


    if (!make_boundary(m)) {
        int i, r;
        int x[2];

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

        //get seed coords
        for (i = 0; i <= r; i++) {
            // printf("Random Terrain start\n");
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 4;
            int a = rand() % 5;
            // printf("x: %d, y: %d, %c\n", x[0], x[1], (char) TERRAIN[a]);
            m->terrain[x[1]][x[0]] = TERRAIN[a];
            m->alt[x[1]][x[0]] = ALTITUDE[a][0];
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        // print_map(m, 0);
        // printf("Begin spreading\n");
        spread_seed(m, qx, qy);
        // printf("Fill Map\n");
        fill_map(m);
        // print_map(m, 0);
        printf("Add Paths\n");
        // pathfind(m, 0);
        trailblaze(m);
        // print_map(m, 0);
    } else {
        printf("There was an error generating the map");
        return 1;
    }

    printf("Begin cleanup\n");
    queue_destroy(qx);
    queue_destroy(qy);

    return 0;
}


int main(int argc, char *argv[])
{
    printf("start\n");
    int i, j;
    // Map d, *prev;
    // int count = 0, n, s, e, w;
    // char in[6];

    printf("initialize World\n");
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    printf("setup Randoms\n");
    srand(time(NULL));

    // n = 1 + (rand() % BOUNDS_X - 2);
    // s = 1 + (rand() % BOUNDS_X - 2);
    // e = 1 + (rand() % BOUNDS_Y - 2);
    // w = 1 + (rand() % BOUNDS_Y - 2);

    printf("Create Starting Map\n");
    world[curPos[0]][curPos[1]] = malloc(sizeof(world[curPos[0]][curPos[1]]));
    create_map(world[curPos[0]][curPos[1]]);
    print_map(world[curPos[0]][curPos[1]]);
    
    // world[curPos[0]][curPos[1]] = &d;
    
    // while (1) {
    //     printf("Move: %d, Current Position: (%d, %d). Enter Move:\t", count, curPos[1] - 200, curPos[0] - 200);
    //     fgets(in, sizeof(in), stdin);
    //     printf("%c\n", in[0]);

    //     prev = world[curPos[0]][curPos[1]];

    //     printf("check moves\n");
    //     switch (in[0]) {
    //         case 'n':
    //             curPos[0] += 1;
    //             count++;
    //             n = 1 + (rand() % BOUNDS_X - 2);
    //             s = prev->n;
    //             e = 1 + (rand() % BOUNDS_Y - 2);
    //             w = 1 + (rand() % BOUNDS_Y - 2);
    //             break;
    //         case 's':
    //             curPos[0] -= 1;
    //             count++;
    //             n = prev->s;
    //             s = 1 + (rand() % BOUNDS_X - 2);
    //             e = 1 + (rand() % BOUNDS_Y - 2);
    //             w = 1 + (rand() % BOUNDS_Y - 2);
    //             break;
    //         case 'e':
    //             curPos[1] += 1;
    //             count++;
    //             n = 1 + (rand() % BOUNDS_X - 2);
    //             s = 1 + (rand() % BOUNDS_X - 2);
    //             e = 1 + (rand() % BOUNDS_Y - 2);
    //             w = prev->e;
    //             break;
    //         case 'w':
    //             curPos[1] -= 1;
    //             count++;
    //             n = 1 + (rand() % BOUNDS_X - 2);
    //             s = 1 + (rand() % BOUNDS_X - 2);
    //             e = prev->w;
    //             w = 1 + (rand() % BOUNDS_Y - 2);
    //             break;
    //         case 'f':
    //             curPos[0] = in[2];
    //             curPos[1] = in[4];
    //             count++;
    //             break;
    //         case 'q':
    //             return 0;
    //         default:
    //             printf("Command Not Valid\n");
    //             break;
    //     }

    //     if (!(curPos[0] % 401)) {
    //         curPos[0] -= 1;
    //     }
    //     if (!(curPos[1] % 401)) {
    //         curPos[1] -= 1;
    //     }
    //     printf("nullcheck spot\n");
    //     if (world[curPos[0]][curPos[1]] == NULL) {
    //         world[curPos[0]][curPos[1]] = malloc(sizeof(world[curPos[0]][curPos[1]]));
    //         new_map(world[curPos[0]][curPos[1]], n, s, e, w);
    //         print_map(world[curPos[0]][curPos[1]]);
    //     } else {
    //         print_map(world[curPos[0]][curPos[1]]);
    //     }
    // }
    
    return 0;
}