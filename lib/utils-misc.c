#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"
#include "structs.h"
#include "maps.h"
#include "utils-misc.h"

//globals
int numTrainers;

//constants
const char TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S};
const char ALL_TERRAIN[] = {ROCK, TREE, GRASS_T, WATER, GRASS_S, GATE, PATH, MART, CENTER, BORDER};
const char ALL_TRAINERS[] = {PLAYER, HIKER, RIVAL, PACER, WANDERER, SENTRY, EXPLORER, SWIMMER};
const int ALTITUDE[][2] = {{50, 30}, {43, 25}, {45, 15}, {18, 0}, {45, 20}};

/*
 * rock, tree, tgrass, water, sgrass, gate, path, mart, center, border
 * player, hiker, rival, pacer, wanderer, sentry, explorer, swimmer
 */
const int STRESS[num_types_tra][num_types_ter] = {
    {D_MAX, D_MAX, 20, D_MAX, 10, 10, 10, 10, 10, D_MAX},
    {15, 15, 15, D_MAX, 10, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, 20, D_MAX, 10, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX},
    {D_MAX, D_MAX, 20, D_MAX, 15, D_MAX, 10, 50, 50, D_MAX},
    {D_MAX, D_MAX, D_MAX, 7, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX, D_MAX}
};


//movement utility
Dir_e change_direction(int x, Dir_e d)
{
    if (x == 0) {
        switch (d) {
            case NW:
                return SE;
            case N:
                return S;
            case NE:
                return SW;
            case W:
                return E;
            case E:
                return W;
            case SW:
                return NE;
            case S:
                return S;
            case SE:
                return NW;
            default:
                printw("Error reversing direction %d", d);
                return NONE;
        }
    } else {
        return (x * rand()) % num_dir;
    }
}


//comperators
int32_t path_cmp(const void *key, const void *with) 
{
  return ((Path *) key)->cost - ((Path *) with)->cost;
}

int32_t entity_cmp(const void *key, const void *with) 
{
  int ret;

  ret = ((Entity *) key)->nextTime - ((Entity *) with)->nextTime;
  if (ret == 0) {
    ret = ((Entity *) key)->order - ((Entity *) with)->order;
  }

  return ret;
}

int32_t point_cmp(const void *key, const void *with)
{
    int ret = 0;
    
    ret = ((Point *) key)->x - ((Point *) with)->x;
    if (!ret) {
        ret = ((Point *) key)->y - ((Point *) with)->y;
    }


    return ret;
}

//!chess
int valid_move_ter(Terrain_e t)
{
    return (t != GTE && t != BDR);
}

int valid_pos_trainer(Trainer_e e, Terrain_e t, Terrain_e start)
{
    if (!valid_move_ter(t) || t == MRT || t == CTR) {
        return 1;
    }

    switch (e){
        case PLAY:
        case RIVL:
        case HIKR:
        case SWIM:
        case EXPL:
            return (STRESS[e][t] != D_MAX);
            break;
        case PACR:
        case WAND:
            return (t == start && t != WTR);
            break;
        case SENT:
            return (t != WTR && t != RCK);
            break;
        default:
            return 1;
    }
    return 1;
}

int valid_pos(Trainer_e e, Terrain_e t)
{
    if (valid_move_ter(t) && t != MRT && t != CTR) {
        return 1;
    }
    return (STRESS[e][t] != D_MAX);
}

int containes_trainer(Point p, char map[BOUNDS_Y][BOUNDS_X][10]) {
    int i, ret = 0;


    for (i = 0; i < num_types_tra; i++) {
        if ( (char) map[p.y][p.x][0] == ALL_TRAINERS[i]) {
            ret = 1;
            break;
        }
    }


    return ret;
}

Point check_surroundings_trainer(Point p, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i;
    Point q;


    for (i = 1; i < num_dir; i++) {
        q = get_next_position(p, i);
        switch (map[q.y][q.x][0]) {
            case PLAYER:
            case HIKER:
            case RIVAL:
            case PACER:
            case WANDERER:
            case SENTRY:
            case EXPLORER:
            case SWIMMER:
                return q;
                break;
            default:
                return (Point) {-1, -1};
            
        }
    }

    return (Point) {-1, -1};
}

//getters
int find_stress(Map *m, Entity *e, Point p)
{
    mvprintw(0, 0, "Finding Stress at (%d, %d) for Entity %d at terrain %d\n", p.x, p.y, e->chr, m->terrain[p.y][p.x]);
    switch (e->chr){
        case PLAY:
        case RIVL:
        case HIKR:
        case SWIM:
        case EXPL:
            return STRESS[e->chr][m->terrain[p.y][p.x]];
            break;
        case PACR:
        case WAND:
            return m->terrain[p.y][p.x] == e->start? STRESS[0][m->terrain[p.y][p.x]]: STRESS[e->chr][m->terrain[p.y][p.x]];
            break;
        case SENT:
            return D_MAX;
            break;
        default:
            mvprintw(0, 0, "Error finding stress for %c\n", ALL_TRAINERS[e->chr]);
            return -1;
    }
    return -1;
}

Point get_next_position(Point p, Dir_e d)
{
    switch (d) {
        case NW:
            return (Point) {.x = p.x - 1, .y = p.y - 1};
        case N:
            return (Point) {.x = p.x, .y = p.y - 1};
        case NE:
            return (Point) {.x = p.x + 1, .y = p.y - 1};
        case W:
            return (Point) {.x = p.x - 1, .y = p.y};
        case E:
            return (Point) {.x = p.x + 1, .y = p.y};
        case SW:
            return (Point) {.x = p.x - 1, .y = p.y + 1};
        case S:
            return (Point) {.x = p.x, .y = p.y + 1};
        case SE:
            return (Point) {.x = p.x + 1, .y = p.y + 1};
        default:
            // fprintf(stderr, "Error getting next position from (%d, %d) in direction %d\n", p.x, p.y, d);
            return p;
    }
}

Dir_e get_lower_alt(Point p, Map *m)
{
    int i;
    Point q;


    for (i = 1; i < num_dir; i++) {
        // print_point(q);
        q = get_next_position(p, i);
        if (q.x > 0 && q.x < (BOUNDS_X - 1) && q.y > 0 && q.y < (BOUNDS_Y - 1) && valid_move_ter(m->terrain[q.x][q.y])) {
            if (m->alt[q.x][q.y] <= m->alt[p.x][p.y]) {
                return i;
            }
        }
    }

    return -1;
}

Entity* find_entity_pos(Trainer **t, Point p) 
{
    int i;


    for (i = 0; i < numTrainers; i++) {
        if (!point_cmp(&p, &t[i]->e.pos)) {
            return &t[i]->e;
        }
    }


    return NULL;
}

//HP's
int print_display(char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            mvprintw(i + 1, j, "%s", map[i][j]);
        }
    }

    return 0;
}

int color_display(Map *m, PC *player, Trainer **ts)
{
    int i, j;


    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            mvchgat(i + 1, j, 1, A_NORMAL, m->terrain[i][j] + 1, NULL);
        }
    }

    for (i = 0; i < numTrainers; i++) {
        mvchgat(ts[i]->e.pos.y + 1, ts[i]->e.pos.x, 1, A_NORMAL, 11, NULL);
    }

    mvchgat(player->e.pos.y + 1, player->e.pos.x, 1, A_NORMAL, 11, NULL);


    return 0;
}

int print_entity(Entity *e) 
{
    printw("Entity: {\n");
    printw("\tChar: %c\n", ALL_TRAINERS[e->chr]);
    printw("\tPos: (%d, %d),\n", e->pos.x, e->pos.y);
    printw("\tStart: %d\n", e->start);
    printw("\tTime: %d\n", e->nextTime);
    printw("\tHn? %d\n", e->hn == NULL);
    printw("\tMove? %d\n", e->do_move == NULL);
    printw("\tCurrent direction: %d\n", e->dir);
    printw("}\n");

    return 0;
}

int print_entity_trainer_inline(Entity *e, Entity *player, int y, int x)
{
    int dist_x, dist_y;
    char *compas_x, *compas_y, *def;


    dist_x = player->pos.x - e->pos.x;
    dist_y = player->pos.y - e->pos.y;

    if (dist_x > 0) {
        compas_x = "west";
    } else {
        dist_x = -dist_x;
        compas_x = "east";
    }

    if (dist_y > 0) {
        compas_y = "north";
    } else {
        dist_y = -dist_y;
        compas_y = "south";
    }

    def = "\r";
    if (e->defeted) {
        def = "DEFEATED";
    }

    mvprintw(y, x, "Entity %c, %d %s and %d %s %s", ALL_TRAINERS[e->chr], dist_y, compas_y, dist_x, compas_x, def);


    return 0;
}

int print_point(Point p)
{
    printw("(%d, %d)\n", p.x, p.y);

    return 0;
}

//distance finders
int manhattan(Point p, Point q)
{
    int ret;


    ret = abs(p.x - q.x) + abs(p.y - q.y);


    return ret;
}

int dijkstra(Map *m, Map *w, Point p, Entity *e)
{
    // printw("Start Dijk's\n");
    static Path path[BOUNDS_Y][BOUNDS_X], *pth;
    static int init = 0;
    heap_t h;
    int x, y, c;
    Point pnt;

    if (!init) {
        for (y = 0; y < BOUNDS_Y; y++) {
            for (x = 0; x < BOUNDS_X; x++) {
                path[y][x].pos[0] = y;
                path[y][x].pos[1] = x;
            }
        }
        init = 1;
        mvprintw(0, 0, "Initialized Path positions\n");
    }
    

    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            path[y][x].cost = D_MAX;
        }
    }
    mvprintw(0, 0, "Initialized Path costs.\n");

    path[p.y][p.x].cost = 0;
    mvprintw(0, 0, "Set point cost to 0\n");

    heap_init(&h, path_cmp, NULL);
    mvprintw(0, 0, "Initialized Heap\n");

    for (y = 1; y < BOUNDS_Y - 1; y++) {
        for (x = 1; x < BOUNDS_X - 1; x++) {
            if (find_stress(w, e, (Point) {.x=x, .y=y}) != D_MAX) {
                mvprintw(0, 0, "Heap Node at (%d, %d): %d\n", x, y, path[y][x].hn == NULL);
                path[y][x].hn = heap_insert(&h, &path[y][x]);
            }
        }
    }
    mvprintw(0, 0, "Filled Heap\n");

    c = 0;
    while ((pth = heap_remove_min(&h))) {
        pth->hn = NULL;
        Point here = (Point) {.x = pth->pos[1], .y = pth->pos[0]};
        mvprintw(0, 0, "%d\n", c);

        mvprintw(0, 10, "T\n");
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "TL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //TL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "TR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //TR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "L\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //left
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "R\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //right
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "B\n");
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //bottom
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "BL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //BL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "BR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //BR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
        }
        c++;
    }

    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            m->alt[y][x] = path[y][x].cost;
        }
    }

    return 0;
}

//new screen manipulation
WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	wrefresh(local_win);		

	return local_win;
}

int enter_building(Terrain_e t)
{
    WINDOW *map = malloc(sizeof(*map));


    map = dupwin(stdscr);
    clear();
    mvprintw(1, 0, "This interface is not implemented yet.");
    getch();
    overwrite(map, stdscr);
    free(map);


    return 0;
}

int initiate_battle(Entity *trainer, PC *player)
{
    WINDOW *map = malloc(sizeof(*map));


    map = dupwin(stdscr);
    clear();
    mvprintw(1, 0, "This interface is not implemented yet.");
    getch();
    overwrite(map, stdscr);
    free(map);
    trainer->defeted = 1;


    return 0;
}

int display_trainer_list(Map *m, Entity *player, int y, int x)
{
    int i;


    i = 0;
    mvprintw(y, x, "Displaying list of trainers\n");
    while (i < numTrainers) {
        refresh();
        print_entity_trainer_inline(&m->trainers[i]->e, player, y, x);
        // c = getch();

        switch (getch()) {
            case KEY_UP:
                i--;
                break;
            case KEY_DOWN:
                i++;
                break;
            case 'q':
                i = numTrainers;
                break;
            default:
                mvprintw(0, 0, "Invalid input detected.\n");
        }

        if (i >= numTrainers) {
            i = numTrainers;
            mvprintw(y, x, "End of the list, press any jey to exit\n");
            getch();
        } else if(i < 0) {
            i = 0;
            mvprintw(y, x, "Top of the list.\n");
            getch();
        }
    }

    return 0;
}

//battle stuff
int check_battle(Map *wrld, Entity *e, PC *player)
{
    int i, ret = 0;
    Point p;


    for(i = 0; i < num_dir; i++) {
        p = get_next_position(e->pos, i);
        if (p.x == player->e.pos.x && p.y == player->e.pos.y) {
            if (e->dir == i) {
                ret = 1;
            }
            break;
        }
    }

    
    return ret;
}


