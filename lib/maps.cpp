#include "maps.h"

#include <ncurses.h>

#include "utils-misc.h"
#include "trainers.h"


//Bobs
Map::Map(Terrain_e **t, int **a, int *g)
{
    memcpy(this->terrain, t, sizeof(this->terrain));
    memcpy(this->alt, a, sizeof(this->alt));
    memcpy(this->gates, g, sizeof(this->gates));
}

//HP's
int Map::print_map_alt() const 
{
    int i , j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            mvprintw(0, 0, "Printing (%d, %d): %d\n", j, i, this->alt[i][j]);
            mvprintw(45 + i, j * 2, "%d|", this->alt[i][j]);
        }
        printw("\n");
    }

    return 0;
}

int Map::print_map_terrain() const
{
    int i , j;

    mvprintw(22, 0, " \t");
    for (j = 0; j < BOUNDS_X; j++) {
        printw("%d ", j % 10);
    }
    printw("\n\n");

    for (i = 0; i < BOUNDS_Y; i++) {
        printw("%d\t", i);
        for (j = 0; j < BOUNDS_X; j++) {
            printw("%d ", this->terrain[i][j]);
        }
        printw("\n");
    }

    return 0;
}

int Map::print_map() const
{
    return (!print_map_terrain() && !print_map_alt());
}

int Map::print_cost_map() const 
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (this->alt[i][j] < D_MAX) {
                mvprintw(BOUNDS_Y + 4 + i, j*3, "%02d", this->alt[i][j] % 100);
            } else {
                mvprintw(BOUNDS_Y + 4 + i, j, "   ");
            }
            
        }
        printw("\n");
    }

    return 0;
}

//-cats
Terrain_e **Map::copy_map_terrain() const
 {
    int i, j;
    static Terrain_e **ret = (Terrain_e **) malloc(sizeof(this->terrain));

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            ret[i][j] = this->terrain[i][j];
        }
    }

    return ret;
}

int **Map::copy_map_alt() const
{
    int i, j;
    static int **ret = (int **) malloc(sizeof(this->alt));


    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            ret[i][j] = this->alt[i][j];
        }
    }

    return ret;
}

Map *Map::copy_map() const
 {
    int **alt = copy_map_alt();
    Terrain_e **terrain = copy_map_terrain();
    int *gates = (int *) malloc(sizeof(this->gates));
    gates = (int *) memcpy(gates, this->gates, sizeof(this->gates));


    return new Map(terrain, alt, gates);
}

//checkers
int Map::check_map() const 
{
    printw("terrain: %ld x %ld\n", sizeof(this->terrain[0])/sizeof(Terrain_e), sizeof(this->terrain)/sizeof(this->terrain[0])/sizeof(char));
    printw("n:%d s:%d e:%d w:%d\n", this->gates[0], this->gates[1], this->gates[3], this->gates[4]);

    return 0;
}

//map creations
int Map::make_boundary(Point curPos)
{
    int i;

    int x[4] = {this->gates[0], this->gates[1], this->gates[3], this->gates[4]};
    
    //bounds X
    for (i = 0; i < BOUNDS_X; i++) {
        this->terrain[0][i] = BDR;
        this->terrain[BOUNDS_Y-1][i] = BDR;
        this->alt[0][i] = 99;
        this->alt[BOUNDS_Y-1][i] = 99;
    }

    //bounds Y
    for (i = 0; i < BOUNDS_Y; i++) {
        this->terrain[i][0] = BDR;
        this->terrain[i][BOUNDS_X-1] = BDR;
        this->alt[i][0] = 99;
        this->alt[i][BOUNDS_X-1] = 99;
    }

    //set paths
    if (curPos.x > 2) {
        this->alt[x[3]][0] = 25;
        this->terrain[x[3]][0] = GTE;
        this->gates[4] = x[3];
    }
    if (curPos.x < WORLD_SIZE - 1) {
        this->alt[x[2]][BOUNDS_X-1] = 25;
        this->terrain[x[2]][BOUNDS_X-1] = GTE;
        this->gates[3] = x[2]-1;
    }
    if (curPos.y < WORLD_SIZE - 1) {
        this->alt[BOUNDS_Y-1][x[1]] = 25;
        this->terrain[BOUNDS_Y-1][x[1]] = GTE;
        this->gates[1] = x[1];
    }
    if (curPos.y > 1) {
        this->alt[0][x[0]] = 25;
        this->terrain[0][x[0]] = GTE;
        this->gates[0] = x[0];
    }
    // print_display(this);
    
    return 0;
}

int Map::add_terrain(int x, int y, Terrain_e c)
{
    Terrain_e *chr = &this->terrain[y][x];
    int *alt = &this->alt[y][x];

    if ((rand() % 137) != 0) {
        *chr = c;
        *alt = *alt - (rand() % 6) + 1;
    } else {
        *alt = *alt - ((rand() % 3) - 1) * (rand() % 3);
        if (*alt > 45) {
            *chr = RCK;
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

int Map::spread_seed(queue_t *qx, queue_t *qy)
{
    // printf("Begin spreading\n");
    int i, x, y;
    Terrain_e chr;

    // queue_dequeue(qx, &x);
    // queue_dequeue(qy, &y);

    while (!queue_is_empty(qx)) {
        queue_dequeue(qx, &x);
        queue_dequeue(qy, &y);

        chr = this->terrain[y][x];

        for (i = -1; i < 2; i++) {
            if (i != 0) {
                if (this->alt[y][x+i] < 50 && this->terrain[y][x+i] == -1) {
                    queue_enqueue(qx, x+i);
                    queue_enqueue(qy, y);
                    add_terrain(x+i, y, chr);
                }
            }
            if (this->alt[y+1][x+i] < 50 && this->terrain[y+1][x+i] == -1) {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y+1);
                add_terrain(x+i, y+1, chr);
            }
            if (this->alt[y-1][x+i] < 50 && this->terrain[y-1][x+i] == -1) {
                queue_enqueue(qx, x+i);
                queue_enqueue(qy, y-1);
                add_terrain(x+i, y-1, chr);
            }
        }
        
    }
    // printf("End spreading\n");
    return 0;
}

int Map::fill_map()
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            if (this->terrain[i][j] == -1) {
                this->terrain[i][j] = GS;
                this->alt[i][j] = 25;
            }
        }
    }

    return 0;
}

int Map::trailblaze(Point curPos, Point center)
{
    int i, j, k, l, b, d, p, u = BOUNDS_X/2 + (rand() % 20);

    //left
    for (i = 0; i < u; i++) {
        // printf("%d, %d\n", this->gates[4], this->gates[4]+i);
        this->terrain[this->gates[4]][1+i] = PTH;
        this->alt[this->gates[4]][1+i] = 25;
    }
    l = i;
    // right
    while (i < BOUNDS_X) {
        // printf("%d, %d\n", this->gates[3], 0+i);
        this->terrain[this->gates[3]][BOUNDS_X-1+i] = PTH;
        this->alt[this->gates[3]][BOUNDS_X-1+i] = 25;
        i++;
    }
    //connect left and right
    j = (this->gates[4] > this->gates[3]) ? this->gates[3] : this->gates[4];
    k = (this->gates[4] > this->gates[3]) ? this->gates[4] : this->gates[3];
    i = 0;
    while (j + i <= k) {
        // printf("%d, %d, %d\n", j, l, k);
        this->terrain[j+i][l] = PTH;
        this->alt[j+i][l] = 25;
        i++;
    }
    
    //top
    b = 1;
    // printf("%d, %d\n", 0 + b, this->gates[0]);
    while (this->terrain[0 + b][this->gates[0]] != PTH) {
        // printf("%d, %d\n", 0 + this, this);
        this->terrain[0+b][this->gates[0]] = PTH;
        this->alt[0+b][this->gates[0]] = 25;
        b++;
    }

    //bottom
    b = 1;
    // printf("%d, %d\n", BOUNDS_Y-1 - b, this->gates[1]);
    while (this->terrain[BOUNDS_Y-1 - b][this->gates[1]] != PTH) {
        // printf("%d, %d\n", BOUNDS_Y-1 - b, b);
        this->terrain[BOUNDS_Y-1-b][this->gates[1]] = PTH;
        this->alt[BOUNDS_Y-1-b][this->gates[1]] = 25;
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
        this->terrain[j+2][l-1] = MRT;
        this->alt[j+2][l-1] = 99;
        this->terrain[j+3][l-2] = MRT;
        this->alt[j+3][l-2] = 99;
        this->terrain[j+2][l-2] = MRT;
        this->alt[j+2][l-2] = 99;
        this->terrain[j+3][l-1] = MRT;
        this->alt[j+3][l-1] = 99;
        //road
        this->terrain[j+2][l-3] = PTH;
        this->alt[j+2][l-3] = 25;
        this->terrain[j+3][l-3] = PTH;
        this->alt[j+3][l-3] = 25;
        this->terrain[j+4][l-3] = PTH;
        this->alt[j+4][l-3] = 25;
        this->terrain[j+4][l-1] = PTH;
        this->alt[j+4][l-1] = 25;
        this->terrain[j+4][l-2] = PTH;
        this->alt[j+4][l-2] = 25;
        this->terrain[j+4][l] = PTH;
        this->alt[j+4][l] = 25;
        this->terrain[j+5][l] = PTH;
        this->alt[j+5][l] = 25;
        this->terrain[j+3][l] = PTH;
        this->alt[j+3][l] = 25;
        this->terrain[j+2][l] = PTH;
        this->alt[j+2][l] = 25;
        this->terrain[j+6][l] = PTH;
        this->alt[j+6][l] = 25;
        this->terrain[j+5][l-3] = PTH;
        this->alt[j+5][l-3] = 25;
        this->terrain[j+6][l-3] = PTH;
        this->alt[j+6][l-3] = 25;
        //Pokecenter
        this->terrain[j+5][l-1] = CTR;
        this->alt[j+5][l-1] = 99;
        this->terrain[j+6][l-2] = CTR;
        this->alt[j+6][l-2] = 99;
        this->terrain[j+5][l-2] = CTR;
        this->alt[j+5][l-2] = 99;
        this->terrain[j+6][l-1] = CTR;
        this->alt[j+6][l-1] = 99;
    }

    return 0;
}

int Map::create_map(Point curPos, Point center)
{
    queue_t *qx, *qy;
    int i, j, n, s, e, w, ret;

    n = 1 + (rand() % (BOUNDS_X - 2));
    s = 1 + (rand() % (BOUNDS_X - 2));
    e = 1 + (rand() % (BOUNDS_Y - 2));
    w = 1 + (rand() % (BOUNDS_Y - 2));

    // printf("n:%d s:%d e:%d w:%d\n", n, s, e, w);
    this->gates[0] = n;
    this->gates[1] = s;
    this->gates[2] = e;
    this->gates[3] = w;

    // printf("setting map\n");
    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            this->terrain[i][j] = (Terrain_e) num_types_ter;
            this->alt[i][j] = 25;
        }
    }


    // printf("trying boundary\n");
    if (!(ret = make_boundary(curPos))) {
        int i, r;
        int x[2];

        // printf("Making queues\n");
        qx = (queue_t *) malloc(sizeof(*qx));
        qy = (queue_t *) malloc(sizeof(*qy));
        queue_init(qx);
        queue_init(qy);

        //seed
        r = rand() % 2;
        // printf("Start Seed: %d\n", r);
        
        //tallgrass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            this->terrain[x[1]][x[0]] = GT;
            this->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        r = rand() % 2;
        //short grass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            this->terrain[x[1]][x[0]] = GS;
            this->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        //water
        x[0] = (rand() % (BOUNDS_X - 5)) + 3;
        x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
        this->terrain[x[1]][x[0]] = WTR;
        this->alt[x[1]][x[0]] = (rand() % 18);
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
            this->terrain[x[1]][x[0]] = (Terrain_e) a;
            this->alt[x[1]][x[0]] = ALTITUDE[a][0];
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        // print_display(this);
        // printf("Begin spreading\n");
        ret = spread_seed(qx, qy) || ret;
        // printf("Fill Map\n");
        ret = fill_map() || ret;
        // printf("Add Paths\n");
        // pathfind(this, 0);
        ret = trailblaze(curPos, center) || ret;
        queue_destroy(qx);
        queue_destroy(qy);
    } else {
        mvprintw(0, 0, "There was an error generating the map border\n");
    }

    if (ret) {
        mvprintw(0, 0, "There was an error generating the map.\n");
    } else {
        mvprintw(0, 0, "Created map.\n");
    }

    return ret;
}

int Map::create_map(Point curPos, Point center, int gates[4])
{
    queue_t *qx, *qy;
    int i, j, ret = 0;

    // printf("n:%d s:%d e:%d w:%d\n", n, s, e, w);
    this->gates[0] = gates[0];
    this->gates[1] = gates[1];
    this->gates[2] = gates[2];
    this->gates[3] = gates[3];

    // printf("setting map\n");
    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            this->terrain[i][j] = (Terrain_e) num_types_ter;
            this->alt[i][j] = 25;
        }
    }


    // printf("trying boundary\n");
    if (!(ret = make_boundary(curPos))) {
        int i, r;
        int x[2];

        // printf("Making queues\n");
        qx = (queue_t *) malloc(sizeof(*qx));
        qy = (queue_t *) malloc(sizeof(*qy));
        queue_init(qx);
        queue_init(qy);

        //seed
        r = rand() % 2;
        // printf("Start Seed: %d\n", r);
        
        //tallgrass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            this->terrain[x[1]][x[0]] = GT;
            this->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        r = rand() % 2;
        //short grass
        for (i = -1; i <= r; i++) {
            x[0] = (rand() % (BOUNDS_X - 5)) + 3;
            x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
            this->terrain[x[1]][x[0]] = GS;
            this->alt[x[1]][x[0]] = (rand() % 10) + 25;
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        //water
        x[0] = (rand() % (BOUNDS_X - 5)) + 3;
        x[1] = (rand() % (BOUNDS_Y - 5)) + 3;
        this->terrain[x[1]][x[0]] = WTR;
        this->alt[x[1]][x[0]] = (rand() % 18);
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
            this->terrain[x[1]][x[0]] = (Terrain_e) a;
            this->alt[x[1]][x[0]] = ALTITUDE[a][0];
            queue_enqueue(qx, x[0]);
            queue_enqueue(qy, x[1]);
        }

        // print_display(this);
        // printf("Begin spreading\n");
        ret = spread_seed(qx, qy) || ret;
        // printf("Fill Map\n");
        ret = fill_map() || ret;
        // printf("Add Paths\n");
        // pathfind(this, 0);
        ret = trailblaze(curPos, center) || ret;
        queue_destroy(qx);
        queue_destroy(qy);
    } else {
        mvprintw(0, 0, "There was an error generating the map border\n");
    }

    if (ret) {
        mvprintw(0, 0, "There was an error generating the map.\n");
    } else {
        mvprintw(0, 0, "Created map.\n");
    }

    return ret;
}
