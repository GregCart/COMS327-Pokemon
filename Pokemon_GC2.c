#include <time.h>

#include "lib/maps.h"
#include "lib/trainers.h"


//globals
Map *world[WORLD_SIZE][WORLD_SIZE];
Point curPos, center;


//display updates
int add_entity_trainer(Entity *e, char map[BOUNDS_Y][BOUNDS_X][10])
{
    strcpy(map[e->pos.y][e->pos.x], (char[2]) {ALL_TRAINERS[e->chr], '\0'});
    mvprintw(0, 0, "%c to (%d, %d)\n", ALL_TRAINERS[e->chr], e->pos.x, e->pos.y);


    return 0;
}

int add_trainer(Trainer *t, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int ret = 0;


    ret = add_entity_trainer(&t->e, map);


    return ret;
}

int update_trails(PC *player, Trainer** t, int numTrainers)
{
    int i, ret = 1;
    Map *w = world[curPos.y][curPos.x];

    
    mvprintw(0, 0, "numTrainers: %d\n", numTrainers);

    ret = dijkstra(trails[PLAY], w, player->e.pos, &player->e);
    mvprintw(0, 0, "Updated Player trail.\n");
    for (i = 0; i < numTrainers; i++) {
        mvprintw(0, 0, "Updating Trail for Entity %d\n", t[i]->e.chr);
        ret = dijkstra(trails[t[i]->e.chr], w, t[i]->e.pos, &t[i]->e) || ret;
        mvprintw(0, 0, "Updated Trail %d\n", i);
    }


    return ret;
}

int map_chars(Map *from, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    
    // mvprintw(0, 0, "Mapping terrain to display.\n");
    for (i = 0; i < BOUNDS_Y; i++) {
        // mvprintw(0, 0, "Mapping row %d.\n", i);
        for (j = 0; j < BOUNDS_X; j++) {
            strcpy(map[i][j], (char[2]) {ALL_TERRAIN[from->terrain[i][j]], '\0'});
        }
    }
    // mvprintw(0, 0, "Mapped terrain to display.\n");
    

    return 0;
}

int map_char(Map *from, char map[BOUNDS_Y][BOUNDS_X][10], Point p)
{
    
    strcpy(map[p.y][p.x], (char[2]) {ALL_TERRAIN[from->terrain[p.y][p.x]], '\0'});
    

    return 0;
}


//initers
int init_world()
{
    int i, j;

    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world[i][j] = NULL;
        }
    }

    return 0;
}

int init_trails()
{
    int i;

    for (i = 0; i < num_types_tra; i++) {
        trails[i] = NULL;
    }

    return 0;
}

int ncurses_start() 
{
    initscr();
    noecho();
    curs_set(0);
    start_color();
    clear();
    refresh();


    return 0;
}

int init_colors()
{
    int ret = 0;
    
    
    ret = init_pair(1, COLOR_WHITE, COLOR_BLACK) || ret;
    ret = init_pair(2, COLOR_CYAN, COLOR_BLACK) || ret;
    ret = init_pair(3, COLOR_GREEN, COLOR_BLACK) || ret;
    ret = init_pair(4, COLOR_BLUE, COLOR_BLACK) || ret;
    ret = init_pair(5, COLOR_GREEN, COLOR_BLACK) || ret;
    ret = init_pair(6, COLOR_YELLOW, COLOR_BLACK) || ret;
    ret = init_pair(7, COLOR_WHITE, COLOR_BLACK) || ret;
    ret = init_pair(8, COLOR_MAGENTA, COLOR_BLACK) || ret;
    ret = init_pair(9, COLOR_RED, COLOR_BLACK) || ret;
    ret = init_pair(10, COLOR_WHITE, COLOR_WHITE) || ret;
    ret = init_pair(11, COLOR_WHITE, COLOR_BLACK) || ret;


    return ret;
}

int setup_game(PC *player, Trainer **trainers, heap_t *order, int numTrainers, char display[BOUNDS_Y][BOUNDS_X][10])
{
    int r, i, ret = 0;


    r = WORLD_SIZE/2;
    center = (Point) {r, r};
    curPos = center;

    srand(time(NULL));
    heap_init(order, entity_cmp, NULL);

    world[curPos.y][curPos.x] = malloc(sizeof(*world[center.y][center.x]));
    
    if (!(ret = create_map(world[curPos.y][curPos.x], curPos, center) || ret) && !(ret = map_chars(world[curPos.y][curPos.x], display) || ret)) {
        player->e.hn = heap_insert(order, &(player->e));
        player->e.pos = (Point) {.x=world[curPos.y][curPos.x]->s, .y=BOUNDS_Y-2};
        trails[player->e.chr] = malloc(sizeof(*trails[0]));
        ret = add_trainer(player, display) || ret;

        for (i = 0; i < numTrainers; i++) {
            while (!valid_pos_list(trainers[i]->e.chr, world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x], trainers[i]->e.start)) {
                trainers[i]->e.pos = (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))};
                trainers[i]->e.start = world[curPos.y][curPos.x]->terrain[trainers[i]->e.pos.y][trainers[i]->e.pos.x];
            }
            trainers[i]->e.hn = heap_insert(order, &(trainers[i]->e));
            if (trails[trainers[i]->e.chr] == NULL) {
                trails[trainers[i]->e.chr] = malloc(sizeof(*trails[0]));
                copy_map_alt(world[curPos.y][curPos.x], trails[trainers[i]->e.chr]);
            }
            ret = add_trainer(trainers[i], display) || ret;
            mvprintw(0, 20, (char[2]) {(i + '0'), '\0'});
        }
        world[curPos.y][curPos.x]->trainers = trainers;
    } else {
        ret = 1;
    }


    return ret;
}

//cleaning crew
int cleanup(Trainer **trainers, int numTrainers)
{
    int i, j;

    for (i = 0; i < numTrainers; i++) {
        free(trainers[i]);
    }
    
    for (i = 0; i < num_types_ter; i++) {
        if (trails[i] && trails[i] != NULL) {
            free(trails[i]);
        }
    }

    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            if (world[i][j] && world[i][j] != NULL) {
                free(world[i][j]);
            }
        }
    }
    echo();
    endwin();


    return 0;
}

//The actual game
int gameloop(int numTrainers)
{
    int ret = 0, code = 0;
    char display[BOUNDS_Y][BOUNDS_X][10];
    heap_t order;
    PC *player;
    Trainer **trainers;
    Entity *ent;
    Map *m;
    Point p;


    player = init_trainer(PLAY, (Point) {.x=BOUNDS_X - 3, .y=BOUNDS_Y - 3}, num_types_ter);
    trainers = init_trainers(numTrainers);


    if (!setup_game(player, trainers, &order, numTrainers, display)) {
        m = world[curPos.y][curPos.x];
        player->e.pos = (Point) {.x=m->s, .y=BOUNDS_Y-2};
        ret = print_display(display) || ret;
        ret = color_display(m, trainers, numTrainers) || ret;
        while (!ret) {
            m = world[curPos.y][curPos.x];
            ent = heap_remove_min(&order);
            mvprintw(0, 0, "Turn: %d, Trainer %d (%c)\n", ent->nextTime, ent->order, ALL_TRAINERS[ent->chr]);
            p = ent->pos;
            if (!ent->defeted) {
                if (!ret && check_battle(m, ent, player)) {
                    initiate_battle(ent, player);
                } else if(!(code = ent->do_move(ent, m))) {
                    ent->nextTime += STRESS[ent->chr][world[curPos.y][curPos.x]->terrain[ent->pos.y][ent->pos.x]];
                    ret = map_char(m, display, p) || ret;
                    ret = add_entity_trainer(ent, display) || ret;
                    ret = print_display(display) || ret;
                    ret = color_display(m, trainers, numTrainers) || ret;
                } else if (ret == 1) {
                    mvprintw(0, 0, "Error %d: Entity %d (%c) failed to move. Press any key to quit.\n", ret, ent->chr, ALL_TRAINERS[ent->chr]);
                    getch();
                } else if (code) {
                    ret = code;
                }
            }
            
            
            ent->hn = heap_insert(&order, ent);
            refresh();
        }
    } else {
        ret = 1;
    }

    if (ret == 1) {
        getch();
        mvprintw(0, 0, "There was an error in game. (Press any key)\n");
        getch();
        endwin();
    }

    free(player);
    cleanup(trainers, numTrainers);


    return ret;
}

int testgame()
{
    int ret = 1;
    char display[BOUNDS_Y][BOUNDS_X][10];
    heap_t order;
    PC *player;
    Trainer **trainers;


    player = init_trainer(PLAY, (Point) {.x=BOUNDS_X - 3, .y=BOUNDS_Y - 3}, num_types_ter);
    trainers = init_trainers(2);
    if (!(ret = setup_game(player, trainers, &order, 2, display))) {
        mvprintw(0, 0, "Did setup.\n");
        ret = update_trails(player, trainers, 2) || ret;
        mvprintw(0, 0, "Updated Trails.\n");
        ret = print_display(display) || ret;
        color_display(world[curPos.y][curPos.x], trainers, 2);
        refresh();

        getch();
    } else {
        mvprintw(0, 0, "There was an error in setup.\n");
        getch();
    }
    


    free(player);
    ret = cleanup(trainers, 2) || ret;
    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 1;
    int numTrainers = 10;

    
    if (argc != 1 && argc != 3) {
        printf("Usage: %s --numtrainers <number of trainers to spawn <default: 10>\n", argv[0]);
    } else if (argc == 2 && strcmp(argv[1], "--numtrainers") == 0) {
        numTrainers = atoi(argv[2]);
    }

    if (!init_world() && !ncurses_start() && !init_trails() && !init_colors()) {
        ret = gameloop(numTrainers);
        // ret = testgame();
    } else {
        mvprintw(0, 0, "Something went wrong on init.\n");
        ret = 1;
    }


    return ret;
}