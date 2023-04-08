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
int add_entity_trainer(Entity *e, Plane<char> map)
{
    map.a[e->pos.y][e->pos.x] = ALL_TRAINERS[e->get_chr()];
    mvprintw(0, 0, "%c to (%d, %d)\n", ALL_TRAINERS[e->get_chr()], e->pos.x, e->pos.y);


    return 0;
}

int add_trainer(Trainer *t, Plane<char> map)
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

    
    // mvprintw(0, 0, "numTrainers: %d\n");
    for (i = 0; i < num_types_tra; i++) {
        visited[i] = false;
    }

    ret = dijkstra(trails[PLAY], w, player->pos, player);
    visited[0] = true;
    mvprintw(0, 0, "Updated Player trail.\n");
    for (i = 0; i < numTrainers; i++) {
        if (!visited[t[i]->get_chr()]) {
            mvprintw(0, 0, "Updating Trail for Entity %c\n", ALL_TRAINERS[t[i]->get_chr()]);
            ret = dijkstra((Map *) trails[t[i]->get_chr()], (Map *) w, (Point) player->pos, (Entity *) &t[i]) || ret;
            mvprintw(0, 0, "Updated Trail %d\n", i);
            visited[t[i]->get_chr()] = true;
        }
    }


    return ret;
}

int map_chars(Map *from, Plane<char> map)
{
    int i, j;
    Plane<Terrain_e> ter;

    
    mvprintw(0, 0, "Mapping terrain to display.\n");
    memcpy(ter.a, from->get_map_terrain().a, sizeof(ter.a));
    for (i = 0; i < BOUNDS_Y; i++) {
        mvprintw(0, 0, "Mapping row %d.\n", i);
        for (j = 0; j < BOUNDS_X; j++) {
            map.a[i][j] = ALL_TERRAIN[ter.a[i][j]];
        }
    }
    mvprintw(0, 0, "Mapped terrain to display.\n");

    return 0;
}

int map_char(Map *from, Plane<char> map, Point p)
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

    mvprintw(0, 0, "");
    clrtoeol();
    mvprintw(0, 0, "Initailizing Trainers");
    refresh();
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
    mvprintw(0, 0, "");
    clrtoeol();
    mvprintw(0, 0, "Initailized Trainers");
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
        player->hn = heap_insert(&m->order, &(player));
        player->pos = Point(((int *) m->get_map_gates())[1], BOUNDS_Y-2);
        trails[player->get_chr()] = (Map *) malloc(sizeof(*trails[0]));

        memcpy(ter.a, m->get_map_terrain().a, sizeof(ter.a));

        for (i = 0; i < numTrainers; i++) {
            while (valid_pos_trainer((Trainer_e) (m->trainers[i]->get_chr()), 
                    ter.a[m->trainers[i]->pos.y][m->trainers[i]->pos.x], m->trainers[i]->start)) {
                m->trainers[i]->pos = Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3)));
                m->trainers[i]->start = ter.a[m->trainers[i]->pos.y][m->trainers[i]->pos.x];
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

    if (world.a[curPos.y][curPos.x] == NULL) {
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
    int ret = 0, code = 0;
    PC *player;
    Trainer **trainers;
    Entity *ent;
    Map *m;
    Point p, q;


    player = new Trainer(PLAY, Point(BOUNDS_X - 3, BOUNDS_Y - 3), num_types_ter);
    if (!setup_game(player)) {
        mvprintw(0, 0, "Setup Successful\n");
        m = world.a[curPos.y][curPos.x];
        trainers = m->trainers;
        ret = add_trainer(player, display) || ret;
        ret = print_display(display) || ret;
        ret = color_display(m, player, trainers) || ret;
        update_trails(player, trainers);
        while (!ret) {
            m = world.a[curPos.y][curPos.x];
            trainers = m->trainers;
            ent = (Entity *) heap_remove_min(&m->order);
            mvprintw(0, 0, "Turn: %d, Trainer %d (%c)\n", ent->check_time(), ent->check_order(), ALL_TRAINERS[ent->get_chr()]);
            p = ent->pos;
            if (!ent->is_defeated()) {
                if (!ret && check_battle(m, ent, player)) {
                    mvprintw(0, 0, "Battle Between %c and The Player!\n", ALL_TRAINERS[ent->get_chr()]);
                    initiate_battle(ent, player);
                } else if(!(code = ent->do_move(m, display))) {
                    ent->update_time(STRESS[ent->get_chr()][world.a[curPos.y][curPos.x]->get_map_terrain().a[ent->pos.y][ent->pos.x]]);
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