#include <stdlib.h>
#include <string.h>

#include "lib/queue.h"
#include "lib/heap.h"
#include "utils-misc.c"


//globals
//sorted in {y, x}


//HP's
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

    printf(" \t");
    for (j = 0; j < BOUNDS_X; j++) {
        printf("%d ", j % 10);
    }
    printf("\n\n");

    for (i = 0; i < BOUNDS_Y; i++) {
        printf("%d\t", i);
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%d ", m->terrain[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int print_map(Map *m)
{
    return (print_map_terrain(m) && print_map_alt(m));
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

//-cats
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

//checkers
int check_map(Map *m) 
{
    printf("terrain: %ld x %ld\n", sizeof(m->terrain[0])/sizeof(Terrain_e), sizeof(m->terrain)/sizeof(m->terrain[0])/sizeof(char));
    printf("n:%d s:%d e:%d w:%d\n", m->n, m->s, m->e, m->w);

    return 0;
}

//map creations
int make_boundary(Map *m, Point curPos)
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

int trailblaze(Map *m, Point curPos, Point center)
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

int create_map(Map *m, Point curPos, Point center)
{
    queue_t *qx, *qy;
    int i, j, n, s, e, w;

    n = 1 + (rand() % (BOUNDS_X - 2));
    s = 1 + (rand() % (BOUNDS_X - 2));
    e = 1 + (rand() % (BOUNDS_Y - 2));
    w = 1 + (rand() % (BOUNDS_Y - 2));

    // printf("n:%d s:%d e:%d w:%d\n", n, s, e, w);
    m->n = n;
    m->s = s;
    m->e = e;
    m->w = w;

    // printf("setting map\n");
    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            m->terrain[i][j] = -1;
            // printf("%c", m->terrain[i][j]);
        }
        // printf("\n");
    }


    // printf("trying boundary\n");
    if (!make_boundary(m, curPos)) {
        int i, r;
        int x[2];

        // printf("Making queues\n");
        qx = qy = malloc(sizeof(queue_t));
        queue_init(qx);
        queue_init(qy);

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
        // printf("Add Paths\n");
        // pathfind(m, 0);
        trailblaze(m, curPos, center);
    } else {
        fprintf(stderr, "There was an error generating the map\n");
        return 1;
    }

    // printf("Begin cleanup\n");
    queue_destroy(qx);
    queue_destroy(qy);
    
    return 0;
}
