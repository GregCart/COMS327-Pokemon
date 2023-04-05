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
        Trainer(const Trainer_e te, const Point p, const Terrain_e st) : Entity((int) te, p, st) {}
        Trainer(const Trainer_e te, const Point p, const Terrain_e st, const Dir_e d) : Entity((int) te, p, st, d) {}
        ~Trainer() {}

        //initializers
        friend Trainer** init_trainers(const int num);
};

using PC = Trainer;
using Rival = Trainer;
using Hiker = Trainer;


#endif