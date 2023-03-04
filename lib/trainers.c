#include "utils-misc.c"


Map *trails[num_types_tra];
Terrain_e *startTer;

//movement functions
int move_player(Entity *self, Map *m)
{
    return 0;
}

int move_hiker(Entity *self, Map *m)
{
    Map *m = trails[HIKR];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    printf("Current space cost: %d\n", m->alt[self->pos.y][self->pos.x]);
    if (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) {
        // print_point(q);
        printf("new cost: %d\n", m->alt[q.y][q.x]);
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, m);
        if (d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            fprintf(stderr, "Hiker stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_rival(Entity *self, Map *m)
{
    Map *m = trails[RIVL];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    printf("Current space cost: %d\n", m->alt[self->pos.y][self->pos.x]);
    if (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 500 == 0) {
        // print_point(q);
        printf("new cost: %d\n", m->alt[q.y][q.x]);
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, m);
        if(d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            fprintf(stderr, "Rival stuck\n");
            return 1;
        }
    }

    return 0;
}

int move_pacer(Entity *self, Map *m)
{
    Map *m = m;
    Point q = get_next_position(self->pos, self->dir);

    if (m->terrain[q.y][q.x] == startTer[self->order]) {
        self->pos = q;
    } else {
        self->pos = get_next_position(self->pos, change_direction(0, self->dir));
    }

    return 0;
}

int move_wanderer(Entity *self, Map *m)
{
    Map *m = m;
    Point q = get_next_position(self->pos, self->dir);
    int i;

    if (m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x]) {
        self->pos = q;
    } else {
        for (i = 0; i < num_dir; i++) {
            q = get_next_position(self->pos, change_direction(rand(), self->dir));
            if (m->alt[q.x][q.y] && m->alt[self->pos.x][self->pos.y] && m->terrain[self->pos.y][self->pos.x] == m->terrain[q.y][q.x]) {
                self->pos = q;
                break;
            }
        }
    }

    return 0;
}

int move_sentry(Entity *self, Map *m)
{
    //no
    return 0;
}

int move_explorer(Entity *self, Map *m)
{
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

    fprintf(stderr, "Explorer stuck\n");
    return 1;
}

int move_swimmer(Entity *self, Map *m)
{
    Map *m = trails[SWIM];
    Point q = get_next_position(self->pos, self->dir);
    Dir_e d;

    if (valid_pos(self->chr, m->terrain[q.y][q.x]) && (m->alt[q.y][q.x] <= m->alt[self->pos.y][self->pos.x] || rand() % 10 == 0)) {
        self->pos = q;
    } else {
        d = get_lower_alt(self->pos, m);
        if (d > 0) {
            self->pos = get_next_position(self->pos, d);
            self->dir = d;
        } else {
            fprintf(stderr, "Swimmer stuck\n");
            return 1;
        }
    }

    return 0;
}

const int (*movement[]) (Entity *e) = {move_player, move_hiker, move_rival, move_pacer, move_wanderer, move_sentry, move_explorer, move_swimmer};

//initializers
Entity* init_entity(int i, Point p) 
{
    Entity* e = malloc(sizeof(*e));

    // print_map_terrain(m);
    // printf("\n");

    if (e != NULL) {
        e->chr = i;
        e->pos = p;
        e->nextTime = 0;
        e->hn = NULL;
        e->move = NULL;
        e->dir = rand() % num_dir;
    }

    return e;
}

Trainer* init_trainer(Trainer_e te, Point p) 
{
    Trainer* t = malloc(sizeof(*t));

    if (t != NULL) {
        t->e = *(init_entity(te, p));
        t->e.move = movement[te];
    }

    return t;
}

Trainer** init_trainers(const int num)
{
    int i = 0;
    Trainer **trainers = malloc(sizeof(Trainer) * num);

    if (num >= 2) {
        Hiker *h = init_trainer(HIKR, (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))});
        Rival *r = init_trainer(RIVL, (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))});
        h->e.order = 0;
        r->e.order = 1;
        trainers[0] = h;
        trainers[1] = r;
        i = 2;
    }

    while (i < num) {
        Trainer *t = init_trainer(1 + (rand() % (num_types_tra - 1)), (Point) {.x = 2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))});
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
