#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lib/queue.c"

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

int print_map(int map[BOUNDS_Y][BOUNDS_X][2], int player[2])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%c", map[i][j][0]);
        }
        printf("\n");
    }

    return 0;
}

int make_boundary(int map[BOUNDS_Y][BOUNDS_X][2])
{
    int i;

    int x[4] = {(rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_X - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1, (rand() % (BOUNDS_Y - 3)) + 1};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        map[0][i][0] = ROCK;
        map[BOUNDS_Y-1][i][0] = ROCK;
        map[0][i][1] = 99;
        map[BOUNDS_Y-1][i][1] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        map[i][0][0] = ROCK;
        map[i][BOUNDS_X-1][0] = ROCK;
        map[i][0][1] = 99;
        map[i][BOUNDS_X-1][1] = 99;
    }

    //set paths
    map[x[3]][0][1] = 25;
    map[x[3]][0][0] = PATH;
    EXITS[3][0] = x[3];
    EXITS[3][1] = 0;
    map[x[2]][BOUNDS_X-1][1] = 25;
    map[x[2]][BOUNDS_X-1][0] = PATH;
    EXITS[2][0] = x[2] - 1;
    EXITS[2][1] = BOUNDS_X-1;
    map[BOUNDS_Y-1][x[0]][1] = 25;
    map[BOUNDS_Y-1][x[0]][0] = PATH;
    EXITS[0][0] = BOUNDS_Y-1;
    EXITS[0][1] = x[0];
    map[0][x[1]][1] = 25;
    map[0][x[1]][0] = PATH;
    EXITS[1][0] = 0;
    EXITS[1][1] = x[1];

    return 0;
}

int add_terrain(int map[BOUNDS_Y][BOUNDS_X][2], int x, int y, char c)
{
    char *chr = (char*) &map[y][x][0];
    int *alt = (int*) &map[y][x][1];

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

int spread_seed(int map[BOUNDS_Y][BOUNDS_X][2], queue_t *qx, queue_t *qy)
{
    // printf("Begin spreading\n");
    int i, x, y;
    char chr;

    queue_dequeue(qx, &x);
    queue_dequeue(qy, &y);

    while (!queue_is_empty(qx)) {
        queue_dequeue(qx, &x);
        queue_dequeue(qy, &y);

        chr = map[y][x][0];
        // alt = map[x][y][1];
        // print_map(map, 0);
        // printf("Length: %d, Char: %c\n", queue_length(qx), chr);

        for (i = -1; i < 2; i++) {
            if (i != 0) {
                if (map[y][x+i][1] < 50 && map[y][x+i][0] == ' ') {
                    queue_enqueue(qx, x+i);
                    queue_enqueue(qy, y);
                    add_terrain(map, x+i, y, chr);
                }
            }
            if (map[y+1][x+i][1] < 50 && map[y+1][x+i][0] == ' ') {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y+1);
                add_terrain(map, x+i, y+1, chr);
            }
            if (map[y-1][x+i][1] < 50 && map[y-1][x+i][0] == ' ') {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y-1);
                add_terrain(map, x+i, y-1, chr);
            }
        }
        
    }
    // printf("End spreading\n");
    return 0;
}

int fill_map(int map[BOUNDS_Y][BOUNDS_X][2])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (map[i][j][0] == ' ') {
                map[i][j][0] = '.';
                map[i][j][1] = 25;
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

int pathfind(int map[BOUNDS_Y][BOUNDS_X][2], int src)
{
// printf("blazing trail\n");
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
            if (!sptSet[j] && u < BOUNDS_Y && map[u][j][1] && dist[u] != 99999 && dist[u] + map[u][j][1] < dist[j]) {
                dist[j] = dist[u] + map[u][j][1];
                // printf("dist[j]: %d\n", dist[j]);
            }
        }
    }
    printf("done blazing\n");

    printSolution(dist, BOUNDS_X);

    return 0;
}

int trailblaze(int map[BOUNDS_Y][BOUNDS_X][2])
{
    int i, j, k, l, m, u = BOUNDS_X/2 + rand() % 20;

    //left
    for (i = 0; i < u; i++) {
        // printf("%d, %d\n", EXITS[2][0], EXITS[2][1]+i);
        map[EXITS[3][0]][EXITS[3][1]+i][0] = '#';
        map[EXITS[3][0]][EXITS[3][1]+i][1] = 25;
    }
    l = i - 1;
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", EXITS[3][0], EXITS[3][1]+i);
        map[EXITS[2][0]][EXITS[2][1]+i][0] = '#';
        map[EXITS[2][0]][EXITS[2][1]+i][1] = 25;
        i++;
    }
    //connect left and right
    j = (EXITS[3][0] > EXITS[2][0]) ? EXITS[2][0] : EXITS[3][0];
    k = (EXITS[3][0] > EXITS[2][0]) ? EXITS[3][0] : EXITS[2][0];
    i = 0;
    while (j + i <= k) {
        // printf("%d, %d, %d\n", j, l, k);
        map[j+i][l][0] = '#';
        map[j+i][l][1] = 25;
        i++;
    }
    
    //top
    m = 1;
    // printf("%d, %d\n", EXITS[1][0] + m, EXITS[1][1]);
    while (map[EXITS[1][0] + m][EXITS[1][1]][0] != '#') {
        // printf("%d, %d\n", EXITS[1][0] + m, m);
        map[EXITS[1][0]+m][EXITS[1][1]][0] = '#';
        map[EXITS[1][0]+m][EXITS[1][1]][1] = 25;
        m++;
    }

    //bottom
    m = 1;
    // printf("%d, %d\n", EXITS[0][0] - m, EXITS[0][1]);
    while (map[EXITS[0][0] - m][EXITS[0][1]][0] != '#') {
        // printf("%d, %d\n", EXITS[0][0] -     m, m);
        map[EXITS[0][0]-m][EXITS[0][1]][0] = '#';
        map[EXITS[0][0]-m][EXITS[0][1]][1] = 25;
        m++;
    }

    //Add PokemartMart
    map[j+2][l-1][0] = 'M';
    map[j+2][l-1][1] = 99;
    map[j+3][l-2][0] = 'M';
    map[j+3][l-2][1] = 99;
    map[j+2][l-2][0] = 'M';
    map[j+2][l-2][1] = 99;
    map[j+3][l-1][0] = 'M';
    map[j+3][l-1][1] = 99;
    //road
    if(j >= 16) j -= 2;
    map[j+2][l-3][0] = '#';
    map[j+2][l-3][1] = 25;
    map[j+3][l-3][0] = '#';
    map[j+3][l-3][1] = 25;
    map[j+4][l-3][0] = '#';
    map[j+4][l-3][1] = 25;
    map[j+4][l-1][0] = '#';
    map[j+4][l-1][1] = 25;
    map[j+4][l-2][0] = '#';
    map[j+4][l-2][1] = 25;
    map[j+4][l][0] = '#';
    map[j+4][l][1] = 25;
    map[j+5][l][0] = '#';
    map[j+5][l][1] = 25;
    map[j+3][l][0] = '#';
    map[j+3][l][1] = 25;
    map[j+2][l][0] = '#';
    map[j+2][l][1] = 25;
    map[j+6][l][0] = '#';
    map[j+6][l][1] = 25;
    map[j+5][l-3][0] = '#';
    map[j+5][l-3][1] = 25;
    map[j+6][l-3][0] = '#';
    map[j+6][l-3][1] = 25;
    //Pokecenter
    map[j+5][l-1][0] = 'C';
    map[j+5][l-1][1] = 99;
    map[j+6][l-2][0] = 'C';
    map[j+6][l-2][1] = 99;
    map[j+5][l-2][0] = 'C';
    map[j+5][l-2][1] = 99;
    map[j+6][l-1][0] = 'C';
    map[j+6][l-1][1] = 99;

    return 0;
}

int create_map(int map[BOUNDS_Y][BOUNDS_X][2])
{
    queue_t *qx, *qy;

    qx = qy = malloc(sizeof(queue_t));
    queue_init(qx);
    queue_init(qy);


    if (!make_boundary(map)) {
        int i, r;
        int x[2];



        //seed
        r = rand() % 2;
        // printf("Start Seed: %d\n", r);
        
        //tallgrass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            map[x[1]][x[0]][0] = GRASS_T;
            map[x[1]][x[0]][1] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        r = rand() % 2;
        //short grass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            map[x[1]][x[0]][0] = GRASS_S;
            map[x[1]][x[0]][1] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        //water
        x[0] = (rand() % (BOUNDS_X - 5)) + 3;
        x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
        map[x[1]][x[0]][0] = WATER;
        map[x[1]][x[0]][1] = (rand() % 18);
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
            map[x[1]][x[0]][0] = TERRAIN[a];
            map[x[1]][x[0]][1] = ALTITUDE[a][0];
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        // print_map(map, 0);
        spread_seed(map, qx, qy);
        fill_map(map);
        // print_map(map, 0);
        // pathfind(map, 0);
        trailblaze(map);
        print_map(map, 0);
    } else {
        printf("There was an error generating the map");
        return 1;
    }

    queue_destroy(qx);
    queue_destroy(qy);

    free(qx);
    free(qy);

    return 0;
}

int main(int argc, char *argv[])
{
    int map[BOUNDS_Y][BOUNDS_X][2];
    int player[2];
    int i, j;

    srand(time(NULL));

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            map[i][j][0] = ' ';
        }
    }

    if (!create_map(map)) {
        print_map(map, player);
    }

    return 0;
}