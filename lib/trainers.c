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
                if (wrld->terrain[self->pos.y][self->pos.x] == MRT || wrld->terrain[self->pos.y][self->pos.x] == CTR) {
                    enter_building(wrld->terrain[self->pos.y][self->pos.x]);
                    d = self->dir;
                    ret = 8;
                } else {
                    mvprintw(0, 0, "That is not a building!\n");
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
            default:
                d = NONE;
                mvprintw(0, 0, "Invalid input detected!\n");
        }

        if (ret == 8) {
            p = get_next_position(self->pos, d);
            mvprintw(0, 0, "Input: %c, Next position: (%d, %d)\n", c, p.x, p.y);
            if (self->pos.x != p.x || self->pos.y != p.y) {
                if (valid_pos(self->chr, wrld->terrain[p.y][p.x])) {
                    if (containes_trainer(p, map)) {
                        ret = 6;
                    } else {
                        self->pos = p;
                        self->dir = d;
                        return 0;
                    }
                } else {
                    mvprintw(0, 0, "Failed to move player\n");
                    ret = 2;
                }
            }
        }
    } while (!ret);
    

    return ret;
}

int move_hiker(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *t = trails[HIKR];
    Dir_e d = get_lower_alt(self->pos, t);
    Point q = get_next_position(self->pos, d);

    
    // mvprintw(0, 0, "%c, %d\n", ALL_TRAINERS[self->chr], t->alt[self->pos.y][self->pos.x]);
    // getch();
    if ((t->alt[q.y][q.x] <= t->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) &&
        !containes_trainer(q, map) && 
        !valid_pos_trainer(self->chr, t->terrain[q.y][q.x], t->terrain[self->pos.y][self->pos.x])) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_alt(self->pos, t);
        q = get_next_position(self->pos, d);
        if (d > 0 && 
            !containes_trainer(q, map) && 
            !valid_pos_trainer(self->chr, t->terrain[q.y][q.x], t->terrain[self->pos.y][self->pos.x])) {
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

int move_rival(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = trails[RIVL];
    Dir_e d = get_lower_alt(self->pos, m);
    Point q = get_next_position(self->pos, d);


    // mvprintw(0, 0, "%c, %d\n", ALL_TRAINERS[self->chr], m->alt[self->pos.y][self->pos.x]);
    // getch();
    if ((m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) &&
        !containes_trainer(q, map) && 
        !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_alt(self->pos, m);
        q = get_next_position(self->pos, d);
        if (d > 0 && !containes_trainer(q, map) && 
            !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            mvprintw(0, 0, "Rival failed to move.\n");
        }
    }

    return 0;
}

int move_pacer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    if (m->terrain[q.y][q.x] == self->start &&
        !containes_trainer(q, map) && 
        !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], self->start)) {
        self->pos = q;
    } else {
        d = change_direction(0, self->dir);
        self->pos = get_next_position(self->pos, d);
        self->dir = d;
    }

    return 0;
}

int move_wanderer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = wrld;
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;
    int i;

    if (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x] &&
        !containes_trainer(q, map) && 
        !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
        self->pos = q;
    } else {
        for (i = 0; i < num_dir; i++) {
            d = change_direction(rand(), self->dir);
            q = get_next_position(self->pos, d);
            if (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x] && 
                !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
                self->pos = q;
                self->dir = d;
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
    Dir_e d;
    int i;

    if (valid_move_ter(m->terrain[q.y][q.x]) && 
        !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x]) &&
        !containes_trainer(q, map)) {
        self->pos = q;
        return 0;
    } else {
        for (i = 0; i < num_dir; i++) {
            d = change_direction(rand(), self->dir);
            q = get_next_position(self->pos, d);
            if (valid_pos(EXPL, m->terrain[q.y][q.x]) && 
                !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
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

int move_swimmer(Entity *self, Map *wrld, char map[BOUNDS_Y][BOUNDS_X][10])
{
    Map *m = trails[SWIM];
    Dir_e d = get_lower_alt(self->pos, m);
    Point q = get_next_position(self->pos, d);

    if (valid_pos(self->chr, m->terrain[q.y][q.x]) && 
        (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 10 == 0) &&
        !containes_trainer(q, map) && 
        !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
        self->pos = q;
        self->dir = d;
    } else {
        d = get_lower_alt(self->pos, m);
        if ((d > 0) && 
            !valid_pos_trainer(self->chr, m->terrain[q.y][q.x], m->terrain[self->pos.y][self->pos.x])) {
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
