#include "utils-misc.h"

#include <iostream>
#include <string>
#include <fstream>
#include <ncurses.h>

#include "maps.h"
#include "trainers.h"
#include "entity.h"


//globals
int numTrainers;
Map *trails[num_types_tra];

//constants
int (*movement[]) (Entity *e, Map *m, Plane<char> map) = {move_player, move_hiker, move_rival, move_pacer, move_wanderer, move_sentry, move_explorer, move_swimmer};
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
                return N;
            case SE:
                return NW;
            default:
                mvprintw(0, 0, "Error reversing direction %d", d);
                return NONE;
        }
    } else {
        return static_cast<Dir_e>((x * rand()) % num_dir);
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
    ret = ((Entity *) key)->turnOrder - ((Entity *) with)->turnOrder;
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
bool valid_move_ter(Terrain_e t)
{
    return (t != GTE && t != BDR);
}

bool valid_pos_trainer(Trainer_e e, Terrain_e t, Terrain_e start)
{
    if (!valid_move_ter(t) || t == MRT || t == CTR) {
        return false;
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
            return false;
    }
    return false;
}

bool valid_pos(Trainer_e e, Terrain_e t)
{
    return (valid_move_ter(t) && t != MRT && t != CTR && STRESS[e][t] != D_MAX);
}

bool containes_trainer(Point p, Plane<char> map) {
    int i;
    bool ret = false;


    for (i = 0; i < num_types_tra; i++) {
        if ((char) map.a[p.y][p.x] == ALL_TRAINERS[i]) {
            ret = true;
            break;
        }
    }


    return ret;
}

Point check_surroundings_trainer(const Point p, const Plane<char> map)
{
    int i;
    Point q;


    for (i = 1; i < num_dir; i++) {
        q = get_next_position(p, static_cast<Dir_e>(i));
        switch (map.a[q.y][q.x]) {
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
                return Point(-1, -1);
            
        }
    }

    return Point(-1, -1);
}

//getters
int find_stress(Map *m, Entity *e, Point p)
{
    mvprintw(0, 0, "Finding Stress at (%d, %d) for Entity %d at terrain %d", p.x, p.y, e->get_chr(), m->get_map_terrain().a[p.y][p.x]);
    switch (e->get_chr()){
        case PLAY:
        case RIVL:
        case HIKR:
        case SWIM:
        case EXPL:
            return STRESS[e->get_chr()][m->get_map_terrain().a[p.y][p.x]];
            break;
        case PACR:
        case WAND:
            return m->get_map_terrain().a[p.y][p.x] == e->start? STRESS[0][m->get_map_terrain().a[p.y][p.x]]: STRESS[e->get_chr()][m->get_map_terrain().a[p.y][p.x]];
            break;
        case SENT:
            return D_MAX;
            break;
        default:
            mvprintw(0, 0, "Error finding stress for %c", ALL_TRAINERS[e->get_chr()]);
            clrtoeol();
            getch();
            return -1;
    }
    return -1;
}

Point get_next_position(Point p, Dir_e d)
{
    switch (d) {
        case NW:
            return Point(p.x - 1, p.y - 1);
        case N:
            return Point(p.x, p.y - 1);
        case NE:
            return Point(p.x + 1, p.y - 1);
        case W:
            return Point(p.x - 1, p.y);
        case E:
            return Point(p.x + 1, p.y);
        case SW:
            return Point(p.x - 1, p.y + 1);
        case S:
            return Point(p.x, p.y + 1);
        case SE:
            return Point(p.x + 1, p.y + 1);
        default:
            // fprintf(stderr, "Error getting next position from (%d, %d) in direction %d\n", p.x, p.y, d);
            return p;
    }
}

Dir_e get_lower_alt(Point p, Map *m)
{
    int i;
    Point q;
    Dir_e d;


    for (i = 1; i < num_dir; i++) {
        d = (Dir_e) i;
        q = get_next_position(p, d);
        if (q.x > 0 && q.x < (BOUNDS_X - 1) && q.y > 0 && q.y < (BOUNDS_Y - 1) && valid_move_ter(m->get_map_terrain().a[q.x][q.y])) {
            if (m->get_map_alt().a[q.x][q.y] <= m->get_map_alt().a[p.x][p.y]) {
                return d;
            }
        }
    }

    return NONE;
}

Dir_e get_lower_cost(Point p, Plane<Terrain_e> ter, Plane<int> cost)
{
    int i;
    Point q;
    Dir_e d;


    for (i = 1; i < num_dir; i++) {
        d = (Dir_e) i;
        q = get_next_position(p, d);
        if (q.x > 0 && q.x < (BOUNDS_X - 1) && q.y > 0 && q.y < (BOUNDS_Y - 1) && 
                valid_move_ter(ter.a[q.x][q.y])) {
            if (cost.a[q.x][q.y] <= cost.a[p.x][p.y]) {
                return d;
            }
        }
    }

    return NONE;
}

Entity* find_entity_pos(Trainer **t, Point p) 
{
    int i;


    for (i = 0; i < numTrainers; i++) {
        if (!point_cmp(&p, &t[i]->pos)) {
            return t[i];
        }
    }


    return NULL;
}

//movement functions
int move_player(Entity *self, Map *wrld, Plane<char> map)
{
    int ret = 0;
    char c;
    Dir_e d;
    Point p;


    do {
        c = getch();


        switch (c) {
            case '7':
            case 'y':
                d = NW;
                ret = 8;
                break;
            case '8':
            case 'k':
                d = N;
                ret = 8;
                break;
            case '9':
            case 'u':
                d = NE;
                ret = 8;
                break;
            case '6':
            case 'l':
                d = E;
                ret = 8; 
                break;
            case '3':
            case 'n':
                d = SE;
                ret = 8;
                break;
            case '2':
            case 'j':
                d = S;
                ret = 8;
                break;
            case '1':
            case 'b':
                d = SW;
                ret = 8;
                break;
            case '4':
            case 'h':
                d = W;
                ret = 8;
                break;
            case '>':
                p = get_next_position(self->pos, self->dir);
                if (wrld->get_map_terrain().a[p.y][p.x] == MRT || wrld->get_map_terrain().a[p.y][p.x] == CTR) {
                    enter_building(wrld->get_map_terrain().a[p.y][p.x]);
                    d = self->dir;
                    ret = 0;
                } else {
                    mvprintw(0, 0, "That is not a building!");
                }
                break;
            case '5':
            case ' ':
            case '.':
                d = NONE;
                ret = 8;
                break;
            case 'q':
                mvprintw(0, 0, "Quitting app.\n");
                getch();
                ret = 3;
                break;
            case 't':
                display_trainer_list(wrld, self, BOUNDS_Y + 1, 0);
                break;
            case 'f':
                ret = 10;
                break;
            // case 'n':
            //     ret = 11;
            //     break;
            // case 's':
            //     ret = 12;
            //     break;
            // case 'e':
            //     ret = 13;
            //     break;
            // case 'w':
            //     ret = 14;
                break;
            default:
                d = NONE;
                mvprintw(0, 0, "Invalid input detected!\n");
        }

        if (ret == 8) {
            p = get_next_position(self->pos, d);
            mvprintw(0, 0, "Input: %c, Next position: (%d, %d)", c, p.x, p.y);
            if (self->pos.x != p.x || self->pos.y != p.y) {
                if (valid_pos((Trainer_e) self->get_chr(), wrld->get_map_terrain().a[p.y][p.x])) {
                    if (containes_trainer(p, map)) {
                        ret = 6;
                    } else {
                        self->pos = p;
                        self->dir = d;
                        return 0;
                    }
                } else if(wrld->get_map_terrain().a[p.y][p.x] == GTE) {
                    switch (self->dir) {
                        case N:
                            ret = 11;
                            break;
                        case S:
                            ret = 12;
                            break;
                        case E:
                            ret = 13;
                            break;
                        case W:
                            ret = 14;
                        default:
                            ret = 11;
                    }
                } else {
                    mvprintw(0, 0, "Failed to move player");
                    ret = 2;
                }
            }
        }
    } while (!ret);

    return ret;
}

int move_hiker(Entity *self, Map *wrld, Plane<char> map)
{
    Map *t = trails[HIKR];
    Dir_e d = get_lower_cost(self->pos, wrld->get_map_terrain(), t->get_map_alt());
    Point q = get_next_position(self->pos, d);

    
    if ((t->get_map_alt().a[q.y][q.x] <= t->get_map_alt().a[self->pos.y][self->pos.x] || rand() % 500 == 0) &&
            !containes_trainer(q, map) && 
            valid_pos_trainer((Trainer_e) self->get_chr(), wrld->get_map_terrain().a[q.y][q.x], self->start)) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_cost(self->pos, wrld->get_map_terrain(), t->get_map_alt());
        q = get_next_position(self->pos, d);
        if (d > 0 && 
                !containes_trainer(q, map) && 
                valid_pos_trainer((Trainer_e) self->get_chr(), t->get_map_terrain().a[q.y][q.x], t->get_map_terrain().a[self->pos.y][self->pos.x])) {
            self->pos = q;
            self->dir = d;
        } else if (containes_trainer(q, map)) {
            return 6;
        } else {
            mvprintw(0, 0, "Hiker didn't move.\n");
            return 1;
        }
    }

    return 0;
}

int move_rival(Entity *self, Map *wrld, Plane<char> map)
{
    Map *m = trails[RIVL];
    Dir_e d = get_lower_cost(self->pos, wrld->get_map_terrain(), m->get_map_alt());
    Point q = get_next_position(self->pos, d);


    // mvprintw(0, 0, "%c, %d\n", ALL_TRAINERS[(Trainer_e) self->get_chr()], m->get_map_alt().a[self->pos.y][self->pos.x]);
    // getch();
    if ((m->get_map_alt().a[q.y][q.x] <= m->get_map_alt().a[self->pos.y][self->pos.x] || rand() % 500 == 0) &&
        !containes_trainer(q, map) && 
        valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_cost(self->pos, wrld->get_map_terrain(), m->get_map_alt());
        q = get_next_position(self->pos, d);
        if (d > 0 && !containes_trainer(q, map) && 
            valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            mvprintw(0, 0, "Rival failed to move.\n");
        }
    }

    return 0;
}

int move_pacer(Entity *self, Map *wrld, Plane<char> map)
{
    const Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    if (m->get_map_terrain().a[q.y][q.x] == self->start &&
        !containes_trainer(q, map) && 
        valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], self->start)) {
        self->pos = q;
    } else {
        d = change_direction(0, self->dir);
        self->pos = get_next_position(self->pos, d);
        self->dir = d;
    }

    return 0;
}

int move_wanderer(Entity *self, Map *wrld, Plane<char> map)
{
    const Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;
    int i;

    if (m->get_map_terrain().a[self->pos.y][self->pos.x] == m->get_map_terrain().a[q.y][q.x] &&
        !containes_trainer(q, map) && 
        valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
        self->pos = q;
    } else {
        for (i = 0; i < num_dir; i++) {
            d = change_direction(rand(), self->dir);
            q = get_next_position(self->pos, d);
            if (m->get_map_terrain().a[self->pos.y][self->pos.x] == m->get_map_terrain().a[q.y][q.x] && 
                valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
                self->pos = q;
                self->dir = d;
                break;
            }
        }
    }

    return 0;
}

int move_sentry(Entity *self, Map *wrld, Plane<char> map)
{
    //no
    return 0;
}

int move_explorer(Entity *self, Map *wrld, Plane<char> map)
{
    const Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;
    int i;

    if (valid_move_ter(m->get_map_terrain().a[q.y][q.x]) && 
        valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x]) &&
        !containes_trainer(q, map)) {
        self->pos = q;
        return 0;
    } else {
        for (i = 0; i < num_dir; i++) {
            d = change_direction(rand(), self->dir);
            q = get_next_position(self->pos, d);
            if (valid_pos(EXPL, m->get_map_terrain().a[q.y][q.x]) && 
                valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
                if (!containes_trainer(q, map))
                self->pos = q;
                self->dir = d;
                return 0;
            }
        }
    }

    printw( "Explorer stuck\n");
    return 1;
}

int move_swimmer(Entity *self, Map *wrld, Plane<char> map)
{
    Map *m = trails[SWIM];
    Dir_e d = get_lower_cost(self->pos, wrld->get_map_terrain(), m->get_map_alt());
    Point q = get_next_position(self->pos, d);

    if (valid_pos((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x]) && 
        (m->get_map_alt().a[q.y][q.x] <= m->get_map_alt().a[self->pos.y][self->pos.x] || rand() % 10 == 0) &&
        !containes_trainer(q, map) && 
        valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_cost(self->pos, wrld->get_map_terrain(), m->get_map_alt());
        if ((d > 0) && 
            !containes_trainer(q, map) && 
            valid_pos_trainer((Trainer_e) self->get_chr(), m->get_map_terrain().a[q.y][q.x], m->get_map_terrain().a[self->pos.y][self->pos.x])) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            mvprintw(0, 0, "Swimmer stuck\n");
            return 1;
        }
    }

    return 0;
}


//HP's
int print_display(const Plane<char> map)
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            mvprintw(0, 0, "char: %c.", map.a[i][j]);
            clrtoeol();
            mvprintw(i + 1, j, "%c", map.a[i][j]);
        }
    }

    return 0;
}

int color_display(const Map *m, const PC *player, Trainer **ts)
{
    int i, j;


    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            mvchgat(i + 1, j, 1, A_NORMAL, m->get_map_terrain().a[i][j] + 1, NULL);
        }
    }

    for (i = 0; i < numTrainers; i++) {
        mvchgat(ts[i]->pos.y + 1, ts[i]->pos.x, 1, A_NORMAL, 11, NULL);
    }

    mvchgat(player->pos.y + 1, player->pos.x, 1, A_NORMAL, 11, NULL);


    return 0;
}

int print_entity_trainer_inline(const Entity *e, const Entity *player, const int y, const int x)
{
    int dist_x, dist_y;
    const char *compas_x, *compas_y, *def;


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
    if (e->is_defeated()) {
        def = "DEFEATED";
    }

    mvprintw(y, x, "Entity %c, %d %s and %d %s %s", ALL_TRAINERS[e->get_chr()], dist_y, compas_y, dist_x, compas_x, def);


    return 0;
}

int print_point(Point p)
{
    printw("(%d, %d)", p.x, p.y);

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
    static Plane<Path> path;
    Path *pth;
    static int init = 0;
    heap_t h;
    int x, y, c;
    Plane<int> tmp;
    Point pnt;

    if (!init) {
        for (y = 0; y < BOUNDS_Y; y++) {
            for (x = 0; x < BOUNDS_X; x++) {
                path.a[y][x].pos[0] = y;
                path.a[y][x].pos[1] = x;
            }
        }
        init = 1;
        mvprintw(0, 0, "Initialized Path positions\n");
    }
    

    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            path.a[y][x].cost = D_MAX;
        }
    }
    mvprintw(0, 0, "Initialized Path costs.");
    clrtoeol();

    path.a[p.y][p.x].cost = 0;
    mvprintw(0, 0, "Set point cost to 0");

    heap_init(&h, path_cmp, NULL);
    mvprintw(0, 0, "Initialized Heap");
    clrtoeol();

    for (y = 1; y < BOUNDS_Y - 1; y++) {
        for (x = 1; x < BOUNDS_X - 1; x++) {
            if (find_stress(w, e, Point(x, y)) != D_MAX) {
                mvprintw(0, 0, "Heap Node at (%d, %d): %d", x, y, path.a[y][x].hn == NULL);
                path.a[y][x].hn = heap_insert(&h, &path.a[y][x]);
            }
        }
    }
    mvprintw(0, 0, "Filled Heap");

    c = 0;
    while ((pth = (Path *) heap_remove_min(&h))) {
        pth->hn = NULL;
        Point here = Point(pth->pos[1], pth->pos[0]);
        mvprintw(0, 0, "%d\n", c);

        mvprintw(0, 10, "T\n");
        pnt = Point(pth->pos[1], pth->pos[0] - 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99 
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "TL\n");
        pnt = Point(pth->pos[1] - 1, pth->pos[0] - 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99 
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //TL
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "TR\n");
        pnt = Point(pth->pos[1] + 1, pth->pos[0] - 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //TR
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "L\n");
        pnt = Point(pth->pos[1] - 1, pth->pos[0]);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //left
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "R\n");
        pnt = Point(pth->pos[1] + 1, pth->pos[0]);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //right
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "B\n");
        pnt = Point(pth->pos[1], pth->pos[0] + 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //bottom
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "BL\n");
        pnt = Point(pth->pos[1] - 1, pth->pos[0] + 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //BL
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        mvprintw(0, 10, "BR\n");
        pnt = Point(pth->pos[1] + 1, pth->pos[0] + 1);
        if ((path.a[pnt.y][pnt.x].hn) && w->get_map_alt().a[pnt.y][pnt.x] < 99
            && (path.a[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here))))) {    //BR
                path.a[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here);
                //from stuff for drawing nodes, don't need for part 3
                path.a[pnt.y][pnt.x].from[0] = pth->pos[0];
                path.a[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path.a[pnt.y][pnt.x].hn);
        }
        c++;
    }

    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            tmp.a[y][x] = path.a[y][x].cost;
        }
    }

    m->set_map_alt(tmp);

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
    WINDOW *map = (WINDOW *) malloc(sizeof(*map));


    map = dupwin(stdscr);
    clear();
    mvprintw(1, 0, "This interface is not implemented yet.\n");
    getch();
    overwrite(map, stdscr);
    free(map);


    return 0;
}

int initiate_battle(Entity *trainer, PC *player)
{
    WINDOW *map = (WINDOW *) malloc(sizeof(*map));


    map = dupwin(stdscr);
    clear();
    mvprintw(1, 0, "This interface is not implemented yet.");
    getch();
    overwrite(map, stdscr);
    free(map);
    trainer->defeat();


    return 0;
}

int display_trainer_list(const Map *m, const Entity *player, const int y, const int x)
{
    int i;


    i = 0;
    mvprintw(y, x, "Displaying list of trainers\n");
    while (i < numTrainers) {
        refresh();
        print_entity_trainer_inline(m->trainers[i], player, y, x);
        // c = getch();

        switch (getch()) {
            case KEY_UP:
                i--;
                break;
            case KEY_DOWN:
                i++;
                break;
            case 'q':
            case 27:
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
bool check_battle(const Map *wrld, const Entity *e, const PC *player)
{
    int i;
    bool ret = false;
    Point p;
    Dir_e d;


    for(i = 0; i < num_dir; i++) {
        d = static_cast<Dir_e>(i);
        p = get_next_position(e->pos, d);
        if (p.x == player->pos.x && p.y == player->pos.y) {
            if (e->dir == d && player->dir == change_direction(0, d)) {
                ret = true;
            }
            break;
        }
    }

    
    return ret;
}

