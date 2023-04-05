#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ncurses.h>

#include "lib/utils-misc.h"
#include "lib/maps.h"
#include "lib/trainers.h"
#include "lib/structs.h"



//globals
Map *world[WORLD_SIZE][WORLD_SIZE];
Point curPos, center;
char display[BOUNDS_Y][BOUNDS_X][10];


//display updates
int add_entity_trainer(Entity *e, char map[BOUNDS_Y][BOUNDS_X][10])
{
    strcpy(map[e->pos.y][e->pos.x], new char[2] {ALL_TRAINERS[e->get_chr()], '\0'});
    mvprintw(0, 0, "%c to (%d, %d)\n", ALL_TRAINERS[e->get_chr()], e->pos.x, e->pos.y);


    return 0;
}

int add_trainer(Trainer *t, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int ret = 0;


    ret = add_entity_trainer(t, map);


    return ret;
}

int update_trails(PC *player, Trainer** t)
{
    int i, ret = 1;
    int visited[num_types_tra];
    Map *w = world[curPos.y][curPos.x];

    
    // mvprintw(0, 0, "numTrainers: %d\n");
    for (i = 0; i < num_types_tra; i++) {
        visited[i] = 0;
    }

    ret = dijkstra(trails[PLAY], w, player->pos, player);
    visited[0] = 1;
    mvprintw(0, 0, "Updated Player trail.\n");
    for (i = 0; i < numTrainers; i++) {
        if (!visited[t[i]->get_chr()]) {
            mvprintw(0, 0, "Updating Trail for Entity %d\n", t[i]->get_chr());
            ret = dijkstra((Map *) trails[t[i]->get_chr()], (Map *) w, (Point) player->pos, (Entity *) &t[i]) || ret;
            mvprintw(0, 0, "Updated Trail %d\n", i);
            visited[t[i]->get_chr()] = 1;
        }
    }


    return ret;
}

int map_chars(Map *from, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;
    Terrain_e ter[BOUNDS_Y][BOUNDS_X];

    
    mvprintw(0, 0, "Mapping terrain to display.\n");
    memcpy(ter, from->get_map_terrain(), sizeof(ter));
    for (i = 0; i < BOUNDS_Y; i++) {
        mvprintw(0, 0, "Mapping row %d.\n", i);
        for (j = 0; j < BOUNDS_X; j++) {
            strcpy(map[i][j], new char[2] {ALL_TERRAIN[ter[i][j]], '\0'});
        }
    }
    mvprintw(0, 0, "Mapped terrain to display.\n");

    return 0;
}

int map_char(Map *from, char map[BOUNDS_Y][BOUNDS_X][10], Point p)
{
    
    strcpy(map[p.y][p.x], new char[2] {ALL_TERRAIN[from->get_map_terrain()[p.y][p.x]], '\0'});
    

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


    mvprintw(0, 0, "Initializing Trails\n");
    for (i = 0; i < num_types_tra; i++) {
        trails[i] = NULL;
    }
    mvprintw(0, 0, "Initialized Trails\n");

    return 0;
}

int ncurses_start() 
{
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
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
    ret = init_pair(7, COLOR_YELLOW, COLOR_BLACK) || ret;
    ret = init_pair(8, COLOR_MAGENTA, COLOR_BLACK) || ret;
    ret = init_pair(9, COLOR_RED, COLOR_BLACK) || ret;
    ret = init_pair(10, COLOR_WHITE, COLOR_WHITE) || ret;
    ret = init_pair(11, COLOR_WHITE, COLOR_BLACK) || ret;


    return ret;
}

Trainer** init_trainers()
{
    int i = 0;
    Trainer **trainers = (Trainer **) malloc(sizeof(Trainer) * numTrainers);

    if (numTrainers >= 2) {
        Hiker h = Trainer(HIKR, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        Rival r = Trainer(RIVL, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        h.order = 0;
        r.order = 1;
        trainers[0] = &h;
        trainers[1] = &r;
        i = 2;
    }

    while (i < numTrainers) {
        Trainer t = Trainer(static_cast<Trainer_e>(1 + (rand() % (num_types_tra - 1))), Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        t.order = i;
        trainers[i] = &t;
        i++;
    }

    return trainers;
}

int init_map(PC *player, Dir_e d) {
    int ret = 0;
    int i, n, s, e, w;
    Map *m, map;

    
    world[curPos.y][curPos.x] = (Map *) malloc(sizeof(*world[center.y][center.x]));
    
    m = world[curPos.y][curPos.x];

    if (curPos.y > 0 && world[curPos.y - 1][curPos.x] != NULL) {
        // printf("N->s:%d, ", world[curPos.y - 1][curPos.x]->n);
        s = world[curPos.y - 1][curPos.x]->get_map_gates()[0];
    } else {
        s = 2 + (rand() % (BOUNDS_X - 3));
    }
    
    if (curPos.y < (WORLD_SIZE - 1) && world[curPos.y + 1][curPos.x] != NULL) {
        // printf("S->n:%d, ", world[curPos.y + 1][curPos.x]->s);
        n = world[curPos.y + 1][curPos.x]->get_map_gates()[1];
    } else {
        n = 2 + (rand() % (BOUNDS_X - 3));
    }
    
    if (curPos.x < (WORLD_SIZE - 1) && world[curPos.y][curPos.x + 1] != NULL) {
        // printf("E->w:%d, ", world[curPos.y][curPos.x + 1]->w);
        e = world[curPos.y][curPos.x + 1]->get_map_gates()[2];
    } else {
        e = 2 + (rand() % (BOUNDS_Y - 3));
    }
    
    if (curPos.x > 0 && world[curPos.y][curPos.x - 1] != NULL) {
        // printf("W->e:%d", world[curPos.y][curPos.x - 1]->e);
        w = world[curPos.y][curPos.x - 1]->get_map_gates()[3];
    } else {
        w = 2 + (rand() % (BOUNDS_Y - 3));
    }

    m = new Map(curPos, center, new int[] {n, s, e, w});

    heap_init(&m->order, entity_cmp, NULL);

    mvprintw(0, 0, "Map Heap created\n");

    m->trainers = init_trainers();
    
    if (!(ret) && 
            !(ret = map_chars(m, display) || ret)) {
        player->hn = heap_insert(&m->order, &(player));
        player->pos = Point(((int *) m->get_map_gates())[1], BOUNDS_Y-2);
        trails[player->get_chr()] = (Map *) malloc(sizeof(*trails[0]));

        for (i = 0; i < numTrainers; i++) {
            while (valid_pos_trainer((Trainer_e) (m->trainers[i]->get_chr()), 
                    ((Terrain_e **) m->get_map_terrain())[m->trainers[i]->pos.y][m->trainers[i]->pos.x], m->trainers[i]->start)) {
                mvprintw(0, 0, "Test");
                getch();
                m->trainers[i]->pos = Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3)));
                m->trainers[i]->start = m->get_map_terrain()[m->trainers[i]->pos.y][m->trainers[i]->pos.x];
            }
            m->trainers[i]->hn = heap_insert(&m->order, &(m->trainers[i]));
            if (trails[m->trainers[i]->get_chr()] == NULL) {
                trails[m->trainers[i]->get_chr()] = (Map *) malloc(sizeof(*trails[0]));
                trails[m->trainers[i]->get_chr()]->set_map_alt(m->get_map_alt());
            }
            ret = add_trainer(m->trainers[i], display) || ret;
            mvprintw(0, 20, new char[2] {static_cast<char>(i + '0'), '\0'});
        }

        switch (d) {
            case N:
                player->pos = Point(m->get_map_gates()[1], BOUNDS_Y-2);
                break;
            case S:
                player->pos = Point(m->get_map_gates()[0], 2);
                break;
            case E:
                player->pos = Point(2, m->get_map_gates()[2]);
                break;
            case W:
                player->pos = Point(BOUNDS_X-2, m->get_map_gates()[2]);
                break;
            default:
                player->pos = Point(m->get_map_gates()[1], BOUNDS_Y-2);
        }
            
    } else {
        ret = 1;
    }


    return ret;
}

int init_game() 
{
    return (init_world() + ncurses_start() + init_trails() + init_colors());
}

int setup_game(PC *player)
{
    int r, ret = 0;


    r = WORLD_SIZE/2;
    center = Point(r, r);
    curPos = center;
    
    srand(time(NULL));

    init_map(player, NONE);


    return ret;
}

//handelers
int handle_code(int code, PC *player, Entity *ent)
{
    int ret = 0;
    Point q;


    switch (code) { 
        case 2:
            break;
        case 6: //fight
            mvprintw(0, 0, "Battle Between %c and The Player!\n", ALL_TRAINERS[ent->get_chr()]);
            q = check_surroundings_trainer(ent->pos, display);
            if (q.x > 0) {
                initiate_battle(ent, player);
            }
            break;
        case 10:
            mvprintw(BOUNDS_Y + 2, 0, "Please enter fly coords (x, y): ");
            curPos.x = 200 + (int) (getch() - '0');
            curPos.y = 200 + (int) (getch() - '0');
            break;
        case 11:
            curPos.y -= 1;
            break;
        case 12:
            curPos.y += 1;
            break;
        case 13:
            curPos.x += 1;
            break;
        case 14:
            curPos.x -= 1;
            break;
        default:
            ret = code;
    }

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

    if (world[curPos.y][curPos.x] == NULL) {
       init_map(player, player->dir);
    }


    return ret;
}

//cleaning crew
int cleanup()
{
    int i, j;

    
    
    for (i = 0; i < num_types_ter; i++) {
        if (trails[i] && trails[i] != NULL) {
            free(trails[i]);
        }
    }

    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            if (world[i][j] && world[i][j] != NULL) {
                for (i = 0; i < numTrainers; i++) {
                    free(world[i][j]->trainers[i]);
                }
                free(world[i][j]);
            }
        }
    }
    echo();
    endwin();


    return 0;
}

//The actual game
int gameloop()
{
    int ret = 0, code = 0;
    char display[BOUNDS_Y][BOUNDS_X][10];
    PC *player;
    Trainer **trainers;
    Entity *ent;
    Map *m;
    Point p, q;


    player = new Trainer(PLAY, Point(BOUNDS_X - 3, BOUNDS_Y - 3), num_types_ter);
    if (!setup_game(player)) {
        mvprintw(0, 0, "Setup Successful\n");
        getch();
        m = world[curPos.y][curPos.x];
        trainers = m->trainers;
        ret = add_trainer(player, display) || ret;
        ret = print_display(display) || ret;
        ret = color_display(m, player, trainers) || ret;
        update_trails(player, trainers);
        while (!ret) {
            m = world[curPos.y][curPos.x];
            trainers = m->trainers;
            ent = (Entity *) heap_remove_min(&m->order);
            mvprintw(0, 0, "Turn: %d, Trainer %d (%c)\n", ent->check_time(), ent->check_order(), ALL_TRAINERS[ent->get_chr()]);
            p = ent->pos;
            if (!ent->is_defeated()) {
                if (!ret && check_battle(m, ent, player)) {
                    mvprintw(0, 0, "Battle Between %c and The Player!\n", ALL_TRAINERS[ent->get_chr()]);
                    initiate_battle(ent, player);
                } else if(!(code = ent->do_move(m, display))) {
                    ent->update_time(STRESS[ent->get_chr()][world[curPos.y][curPos.x]->get_map_terrain()[ent->pos.y][ent->pos.x]]);
                    ret = map_char(m, display, p) || ret;
                    ret = add_entity_trainer(ent, display) || ret;
                    ret = print_display(display) || ret;
                    ret = color_display(m, player, trainers) || ret;
                    if (ent->get_chr() == PLAY) {
                        update_trails(player, trainers);
                    }
                } else if (ret == 1) {
                    mvprintw(0, 0, "Error %d: Entity %d (%c) failed to move. Press any key to quit.\n", ret, ent->get_chr(), ALL_TRAINERS[ent->get_chr()]);
                    getch();
                } else if (code) {
                    ret = handle_code(code, player, ent) || ret;
                }
                ent->hn = heap_insert(&m->order, ent);
            }
            
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

    cleanup();


    return ret;
}

int testgame()
{
    int ret = 1;
    char display[BOUNDS_Y][BOUNDS_X][10];
    PC *player;


    numTrainers = 2;
    player = new Trainer(PLAY, Point(BOUNDS_X - 3, BOUNDS_Y - 3), num_types_ter);
    if (!(ret = setup_game(player))) {
        mvprintw(0, 0, "Did setup.\n");
        ret = update_trails(player, world[curPos.y][curPos.x]->trainers) || ret;
        mvprintw(0, 0, "Updated Trails.\n");
        ret = print_display(display) || ret;
        color_display(world[curPos.y][curPos.x], player, world[curPos.y][curPos.x]->trainers);
        refresh();

        getch();
    } else {
        mvprintw(0, 0, "There was an error in setup.\n");
        getch();
    }
    

    ret = cleanup() || ret;
    return ret;
}


int main(int argc, char *argv[])
{
    int ret = 1;
    numTrainers = 10;

    
    if (argc != 1 && argc != 3) {
        printf("Usage: %s --numtrainers <number of trainers to spawn <default: 10>\n", argv[0]);
    } else if (argc == 2 && strcmp(argv[1], "--numtrainers") == 0) {
        numTrainers = atoi(argv[2]);
    }

    if (!init_game()) {
        mvprintw(0, 0, "Init successful\n");
        ret = gameloop();
        // ret = testgame();
    } else {
        mvprintw(0, 0, "Something went wrong on init.\n");
        ret = 1;
    }


    return ret;
}