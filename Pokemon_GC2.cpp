#include <time.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ncurses.h>

#include "lib/utils-misc.h"
#include "lib/maps.h"
#include "lib/trainers.h"
#include "lib/structs.h"
#include "lib/database-utils.h"



//globals
Plane<Map *, WORLD_SIZE, WORLD_SIZE> world;
Point curPos, center;
Plane<char> display;


//display updates
int add_entity_trainer(Entity *e, Plane<char> &map)
{
    map.a[e->pos.y][e->pos.x] = ALL_TRAINERS[e->get_chr()];
    mvprintw(0, 0, "%c to (%d, %d)\n", ALL_TRAINERS[e->get_chr()], e->pos.x, e->pos.y);
    mvprintw(0, 50, "Actual: %c", map.a[e->pos.y][e->pos.x]);


    return 0;
}

int add_trainer(Trainer *t, Plane<char> &map)
{
    int ret = 0;


    ret = add_entity_trainer(t, map);


    return ret;
}

int update_trails(PC *player, Trainer** t)
{
    int i, ret = 1;
    bool visited[num_types_tra];
    Map *w = world.a[curPos.y][curPos.x];

    
    mvprintw(0, 0, "numTrainers: %d", numTrainers);
    clrtoeol();
    for (i = 0; i < num_types_tra; i++) {
        visited[i] = false;
    }

    ret = dijkstra(*trails[PLAY], w, player->pos, player);
    visited[0] = true;
    mvprintw(0, 0, "Updated Player trail.");
    for (i = 0; i < numTrainers; i++) {
        if (!visited[t[i]->get_chr()]) {
            mvprintw(0, 0, "Updating Trail for Entity %c", ALL_TRAINERS[t[i]->get_chr()]);
            ret = dijkstra(*trails[t[i]->get_chr()], (Map *) w, (Point) player->pos, (Entity *) t[i]) || ret;
            mvprintw(0, 0, "Updated Trail %d", i);
            clrtoeol();
            visited[t[i]->get_chr()] = true;
        }
    }
    mvprintw(0, 0, "Updated trails.");
    clrtoeol();


    return ret;
}

int map_chars(Map *from, Plane<char> &map)
{
    int i, j;
    Plane<Terrain_e> ter;

    
    mvprintw(0, 0, "Mapping terrain to display.\n");
    memcpy(ter.a, from->get_map_terrain().a, sizeof(ter.a));
    for (i = 0; i < BOUNDS_Y; i++) {
        mvprintw(0, 0, "Mapping row %d.\n", i);
        for (j = 0; j < BOUNDS_X; j++) {
            mvprintw(0, 0, "At (%d, %d) Terrain: %d, Char: %c", j, i, ter.a[i][j], ALL_TERRAIN[ter.a[i][j]]);
            clrtoeol();
            map.a[i][j] = ALL_TERRAIN[ter.a[i][j]];
            mvprintw(0, 50, "Actual: %c", map.a[i][j]);
        }
    }
    mvprintw(0, 0, "Mapped terrain to display.\n");

    return 0;
}

int map_char(Map *from, Plane<char> &map, Point p)
{
    
    map.a[p.y][p.x] = ALL_TERRAIN[from->get_map_terrain().a[p.y][p.x]];
    

    return 0;
}


//initers
int init_world()
{
    int i, j;


    clear();
    mvprintw(0, 0, "Initializing world.");
    refresh();
    world = Plane<Map *, (size_t) WORLD_SIZE, (size_t) WORLD_SIZE>();
    for (i = 0; i < WORLD_SIZE; i++) {
        for (j = 0; j < WORLD_SIZE; j++) {
            world.a[i][j] = NULL;
        }
    }
    mvprintw(0, 0, "Initialized World");
    refresh();

    return 0;
}

int init_trails()
{
    int i;


    clear();
    mvprintw(0, 0, "Initializing Trails\n");
    refresh();
    for (i = 0; i < num_types_tra; i++) {
        trails[i] = NULL;
    }
    mvprintw(0, 0, "Initialized Trails\n");
    refresh();

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
    mvprintw(0, 0, "Initialized ncurses");
    refresh();


    return 0;
}

int init_colors()
{
    int ret = 0;
    
    
    clear();
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

    mvprintw(0, 0, "Initialized colors");
    refresh();


    return ret;
}

Trainer** init_trainers()
{
    int i = 0;
    Trainer **trainers = (Trainer **) malloc(sizeof(Trainer) * numTrainers);


    mvprintw(0, 0, "Initailizing Trainers");
    clrtoeol();
    refresh();
    if (numTrainers >= 2) {
        Hiker *h = new Trainer(HIKR, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        Rival *r = new Trainer(RIVL, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        h->turnOrder = 1;
        r->turnOrder = 2;
        h->do_move = movement[HIKR];
        r->do_move = movement[RIVL];
        h->hn = NULL;
        r->hn = NULL;
        trainers[0] = h;
        trainers[1] = r;
        i = 2;
    }

    while (i < numTrainers) {
        Trainer *t = new Trainer((Trainer_e) (1 + (rand() % (num_types_tra - 1))), Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        t->turnOrder = i + 1;
        t->hn = NULL;
        t->do_move = movement[t->get_chr()];
        trainers[i] = t;
        i++;
    }    
    mvprintw(0, 0, "Initailized Trainers");
    clrtoeol();
    refresh();

    return trainers;
}

int init_map(PC *player, Dir_e d) {
    int ret = 0;
    int i, n, s, e, w;
    Map *m;
    Plane<Terrain_e> ter;

    if (curPos.y > 0 && world.a[curPos.y - 1][curPos.x] != NULL) {
        // printf("N->s:%d, ", world.a[curPos.y - 1][curPos.x]->n);
        s = world.a[curPos.y - 1][curPos.x]->get_map_gates()[0];
    } else {
        s = 2 + (rand() % (BOUNDS_X - 3));
    }
    
    if (curPos.y < (WORLD_SIZE - 1) && world.a[curPos.y + 1][curPos.x] != NULL) {
        // printf("S->n:%d, ", world.a[curPos.y + 1][curPos.x]->s);
        n = world.a[curPos.y + 1][curPos.x]->get_map_gates()[1];
    } else {
        n = 2 + (rand() % (BOUNDS_X - 3));
    }
    
    if (curPos.x < (WORLD_SIZE - 1) && world.a[curPos.y][curPos.x + 1] != NULL) {
        // printf("E->w:%d, ", world.a[curPos.y][curPos.x + 1]->w);
        e = world.a[curPos.y][curPos.x + 1]->get_map_gates()[2];
    } else {
        e = 2 + (rand() % (BOUNDS_Y - 3));
    }
    
    if (curPos.x > 0 && world.a[curPos.y][curPos.x - 1] != NULL) {
        // printf("W->e:%d", world.a[curPos.y][curPos.x - 1]->e);
        w = world.a[curPos.y][curPos.x - 1]->get_map_gates()[3];
    } else {
        w = 2 + (rand() % (BOUNDS_Y - 3));
    }

    world.a[curPos.y][curPos.x] = new Map(curPos, center, new int[] {n, s, e, w});
    m = world.a[curPos.y][curPos.x];

    heap_init(&m->order, entity_cmp, NULL);

    mvprintw(0, 0, "Map Heap created");
    clrtoeol();

    m->trainers = init_trainers();

    if (!(ret) && 
            !(ret = map_chars(m, display) || ret)) {
        player->hn = heap_insert(&m->order, player);
        player->pos = Point(((int *) m->get_map_gates())[1], BOUNDS_Y-2);
        player->start = m->get_map_terrain().a[player->pos.y][player->pos.x];
        trails[player->get_chr()] = (Plane<int> *) malloc(sizeof(*trails[0]));

        memcpy(ter.a, m->get_map_terrain().a, sizeof(ter.a));

        clear();
        for (i = 0; i < numTrainers; i++) {
            while (!valid_pos_trainer((Trainer_e) (m->trainers[i]->get_chr()), 
                    ter.a[m->trainers[i]->pos.y][m->trainers[i]->pos.x], m->trainers[i]->start)) {
                m->trainers[i]->pos = Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3)));
                m->trainers[i]->start = ter.a[m->trainers[i]->pos.y][m->trainers[i]->pos.x];
            }
            
            m->trainers[i]->hn = heap_insert(&m->order, m->trainers[i]);

            if (trails[m->trainers[i]->get_chr()] == NULL) {
                trails[m->trainers[i]->get_chr()] = (Plane<int> *) malloc(sizeof(*trails[0]));
            }
            ret = add_trainer(m->trainers[i], display) || ret;
            
            printw(new char[2] {static_cast<char>(i + '0'), '\0'});
            refresh();
        }

        switch (d) {
            case N:
                player->pos = Point(m->get_map_gates()[1], BOUNDS_Y-2);
                break;
            case S:
                player->pos = Point(m->get_map_gates()[0], 2);
                break;
            case E:
                player->pos = Point(2, m->get_map_gates()[3]);
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
    return (ncurses_start() + init_world() + init_trails() + init_colors());
}

int setup_game(PC *player)
{
    int r, ret = 0;


    r = WORLD_SIZE/2;
    center = Point(r, r);
    curPos = center;
    
    srand(time(NULL));

    ret = init_map(player, NONE);
    player->do_move = movement[player->get_chr()];


    return ret;
}

//handelers
int handle_code(int code, PC *player, Entity *ent)
{
    int ret = 0, i;
    Point q, p;


    mvprintw(23, 0, "Test 4: %d", code);
    clrtoeol();
    getch();

    p = curPos;
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

    if (world.a[curPos.y][curPos.x] == NULL) {
       init_map(player, player->dir);
    }

    if (point_cmp(&p, &curPos)) {
        player->set_time(0);
        for (i = 0; i < numTrainers; i++) {
            world.a[curPos.y][curPos.x]->trainers[i]->set_time(0);
        }
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
            if (world.a[i][j] && world.a[i][j] != NULL) {
                for (i = 0; i < numTrainers; i++) {
                    free(world.a[i][j]->trainers[i]);
                }
                free(world.a[i][j]);
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
    int ret = 0, code = 0, i;
    PC *player;
    Trainer **trainers;
    Entity *ent;
    Map *m;
    Point p, q;

    
    player = new Trainer(PLAY, Point(BOUNDS_X - 3, BOUNDS_Y - 3), num_types_ter);
    player->turnOrder = 0;
    if (!setup_game(player)) {
        clear();
        mvprintw(0, 0, "Setup Successful\n");
        m = world.a[curPos.y][curPos.x];
        trainers = m->trainers;
        ret = map_chars(m, display) || ret;
        ret = add_trainer(player, display) || ret;
        for (i = 0; i < numTrainers; i++) {
            ret = add_trainer(m->trainers[i], display) || ret;
        }
        ret = print_display(display) || ret;
        ret = color_display(m, player, trainers) || ret;
        refresh();
        update_trails(player, trainers);
        // for (i = 0; i < num_types_tra; i++) {
        //     if (trails[i] != NULL) {
        //         mvprintw(23, 0, "DONE!");
        //         getch();
        //         mvprintw(23, 0, "Trail: %c\n", ALL_TRAINERS[i]);
        //         print_cost_map(*trails[i]);
        //         getch();
        //     }
        // }
        while (!ret) {
            m = world.a[curPos.y][curPos.x];
            trainers = m->trainers;
            ent = (Entity *) heap_remove_min(&m->order);
            mvprintw(0, 0, "Turn: %d, Trainer %d (%c)", ent->check_time(), ent->check_order(), ALL_TRAINERS[ent->get_chr()]);
            clrtoeol();
            p = ent->pos;
            mvprintw(23, 0, "Test 0");
            clrtoeol();
            getch();
            if (!ent->is_defeated()) {
                mvprintw(23, 0, "Test 1");
            getch();
                if (!ret && check_battle(m, ent, player)) {
                    mvprintw(0, 0, "Battle Between %c and The Player", ALL_TRAINERS[ent->get_chr()]);
                    initiate_battle(ent, player);
                } else if(!(code = ent->do_move(ent, m, display))) {
                    mvprintw(23, 0, "Test 10");
            getch();
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
                
                ent->update_time(STRESS[ent->get_chr()][world.a[curPos.y][curPos.x]->get_map_terrain().a[ent->pos.y][ent->pos.x]]);
                mvprintw(23, 0, "Test 6: %d", ent->check_time());
    getch();
                
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
    PC *player;


    numTrainers = 2;
    player = new Trainer(PLAY, Point(BOUNDS_X - 3, BOUNDS_Y - 3), num_types_ter);
    if (!(ret = setup_game(player))) {
        mvprintw(0, 0, "Did setup.\n");
        ret = update_trails(player, world.a[curPos.y][curPos.x]->trainers) || ret;
        mvprintw(0, 0, "Updated Trails.\n");
        ret = print_display(display) || ret;
        color_display(world.a[curPos.y][curPos.x], player, world.a[curPos.y][curPos.x]->trainers);
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
    int ret = 1, i = 1;
    char *arg = NULL;
    numTrainers = 10;
    

    if (!init_game()) {
        while (i < argc) {
            if (strcmp(argv[i], "--numtrainers") == 0) {
                numTrainers = atoi(argv[i + 1]);
                i += 2;
            } else if (strcmp(argv[i], "--displayfile") == 0) {
                arg = argv[i+1];
                i += 2;
            } else {
                i++;
            }
        }
        clear();
        mvprintw(0, 0, "Init successful\n");
        if (!load_database(arg)) {
            clear();
            mvprintw(0, 0, "Database loaded successful\n");
            ret = gameloop();
        } else {
            ret = 1;
        }
        // ret = testgame();
        cleanup();
    } else {
        mvprintw(0, 0, "Something went wrong on init.\n");
        ret = 1;
    }


    return ret;
}