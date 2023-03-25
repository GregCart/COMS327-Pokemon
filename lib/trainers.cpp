#include "trainers.h"



//Bobs
Trainer::Trainer(Trainer_e te, Point p, Terrain_e st)
{
    Entity(te, p, st);
}

//initializers
Trainer** Trainer::init_trainers(const int num)
{
    int i = 0;
    Trainer **trainers = (Trainer **) malloc(sizeof(Trainer) * num);

    if (num >= 2) {
        Hiker *h = &Trainer(HIKR, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        Rival *r = &Trainer(RIVL, Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        h->order = 0;
        r->order = 1;
        trainers[0] = h;
        trainers[1] = r;
        i = 2;
    }

    while (i < num) {
        Trainer *t = &Trainer(static_cast<Trainer_e>(1 + (rand() % (num_types_tra - 1))), Point(2 + (rand() % (BOUNDS_X - 3)), 2 + (rand() % (BOUNDS_Y - 3))), num_types_ter);
        t->order = i;
        trainers[i] = t;
        i++;
    }

    return trainers;
}

//HP's
int Trainer::print_trainer(Trainer *t)
{
    print_entity();

    return 0;
}
