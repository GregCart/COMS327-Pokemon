#ifndef TRAINERS_H
#define TRAINERS_H

#include "entity.h"


class Trainer : public Entity {
    private:
    protected:
        //HP's
        int print_trainer(Trainer *t);
    public:
        Trainer();
        Trainer(Trainer_e te, Point p, Terrain_e st) : Entity(te, p, st) {}
        Trainer(Trainer_e te, Point p, Terrain_e st, Dir_e d) : Entity(te, p, st, d) {}

        //initializers
        friend Trainer** init_trainers(const int num);
};

using PC = Trainer;
using Rival = Trainer;
using Hiker = Trainer;


#endif