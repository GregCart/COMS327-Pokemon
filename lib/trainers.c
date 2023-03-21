#include "utils-misc.h"
#include "trainers.h"


//constants
const int (*movement[]) (Entity *e, Map *m, char map[BOUNDS_Y][BOUNDS_X][10]) = {move_player, move_hiker, move_rival, move_pacer, move_wanderer, move_sentry, move_explorer, move_swimmer};

//globals
Map *trails[num_types_tra];


//movement functions
int move_player(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    int ret = 0;
    char c;
    Dir_e d;
    Point p;


    c = getch();


    switch (c) {
        case '7':
        case 'y':
            d = NW; 
            break;
        case '8':
        case 'k':
            d = N;
            break;
        case '9':
        case 'u':
            d = NE;
            break;
        case '6':
        case 'l':
            d = E; 
            break;
        case '3':
        case 'n':
            d = SE;
            break;
        case '2':
        case 'j':
            d = S;
            break;
        case '1':
        case 'b':
            d = SW;
            break;
        case '4':
        case 'h':
            d = W;
            break;
        case '>':
            enter_building(wrld->terrain[self->pos.y][self->pos.x]);
            d = self->dir;
            break;
        case '5':
        case ' ':
        case '.':
            d = NONE;
            break;
        case 'q':
            mvprintw(0, 0, "Quitting app.\n");
            getch();
            ret = 3;
            break;
        default:
            d = NONE;
    }

    if (ret == 0) {
        p = get_next_position(self->pos, d);
        mvprintw(0, 0, "Input: %c, Next position: (%d, %d)\n", c, p.x, p.y);
        if (self->pos.x != p.x || self->pos.y != p.y) {
            if (valid_pos(self->chr, wrld->terrain[p.y][p.x])) {
                self->pos = p;
                self->dir = d;
            } else {
                mvprintw(0, 0, "Failed to move player\n");
                ret = 2;
            }
        }
    }
    

    return ret;
}

int move_hiker(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *t = trails[HIKR];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    
    if (t->alt[q.y][q.x] <= t->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) {
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, t);
        if (d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            printw("Hiker stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_rival(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = trails[RIVL];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    // printw("Current space cost: %d\n", m->alt[self->pos.y][self->pos.x]);
    if (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) {
        // print_point(q);
        // printw("new cost: %d\n", m->alt[q.y][q.x]);
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, m);
        if(d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            printw( "Rival stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_pacer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = m;
    Point q = get_next_position(self->pos, self->dir);

    if (m->terrain[q.y][q.x] == self->start) {
        self->pos = q;
    } else {
        self->pos = get_next_position(self->pos, change_direction(0, self->dir));
    }

    return 0;
}

int move_wanderer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    int i;

    if (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x]) {
        self->pos = q;
    } else {
        for (i = 0; i < num_dir; i++) {
            q = get_next_position(self->pos, change_direction(rand(), self->dir));
            if (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x]) {
                self->pos = q;
                break;
            }
        }
    }

    return 0;
}

int move_sentry(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    //no
    return 0;
}

int move_explorer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    int i;

    if (valid_move_ter(m->terrain[q.y][q.x]) && (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x])) {
        self->pos = q;
        return 0;
    } else {
        for (i = 0; i < num_dir; i++) {
            q = get_next_position(self->pos, change_direction(rand(), self->dir));
            if (valid_pos(EXPL, m->terrain[q.y][q.x])) {
                self->pos = q;
                return 0;
            }
        }
    }

    printw( "Explorer stuck\n");
    return 1;
}

int move_swimmer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = trails[SWIM];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    if (valid_pos(self->chr, m->terrain[q.y][q.x]) && 
        (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 10 == 0) &&
        !containes_trainer(q, map)) {
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, m);
        if (d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            mvprintw(0, 0, "Swimmer stuck\n");
            return 1;
        }
    }

    return 0;
}

//initializers
Entity* init_entity(int i, Point p, Terrain_e st) 
{
    Entity* e = malloc(sizeof(*e));
    

    if (e != NULL) {
        e->chr = i;
        e->pos = p;
        e->start = st;
        e->nextTime = 0;
        e->defeted = 0;
        e->hn = NULL;
        e->do_move = NULL;
        e->dir = rand() % num_dir;
    }

    return e;
}

Trainer* init_trainer(Trainer_e te, Point p, Terrain_e st) 
{
    Trainer* t = malloc(sizeof(*t));

    if (t != NULL) {
        t->e = *(init_entity(te, p, st));
        t->e.do_move = movement[te];
    }

    return t;
}

Trainer** init_trainers(const int num)
{
    int i = 0;
    Trainer **trainers = malloc(sizeof(Trainer) * num);

    if (num >= 2) {
        Hiker *h = init_trainer(HIKR, (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))}, num_types_ter);
        Rival *r = init_trainer(RIVL, (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))}, num_types_ter);
        h->e.order = 0;
        r->e.order = 1;
        trainers[0] = h;
        trainers[1] = r;
        i = 2;
    }

    while (i < num) {
        Trainer *t = init_trainer(1 + (rand() % (num_types_tra - 1)), (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))}, num_types_ter);
        t->e.order = i;
        trainers[i] = t;
        i++;
    }

    return trainers;
}

//HP's
int print_trainer(Trainer *t)
{
    print_entity(&t->e);

    return 0;
}
