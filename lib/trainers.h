#ifndef TRAINERS_H
#define TRAINERS_H

#include "entity.h"


class Trainer : Entity {
    private:
    protected:
        //HP's
        int print_trainer(Trainer *t);
    public:
        Trainer();
        Trainer(Trainer_e te, Point p, Terrain_e st);

        //initializers
        Trainer** init_trainers(const int num);
} PC, Hiker, Rival;

#endif