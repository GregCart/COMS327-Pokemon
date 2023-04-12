#ifndef POKEMON_H
#define POKEMON_H


#include "entity.h"


class Pokemon: public Entity {
    private:
        int id;
        const char *identifier;
        int species_id;

    protected:
        int height, weight;
        int base_exp;
        int order;
        int *movesList;

    public:
        bool is_default;


        const int get_id() const { return this->id; }
        const char *get_identifier() const { return this->identifier; }
        const int get_species_id() const { return this->species_id; }

        int *get_height_weight() const { return new int[2] {this->height, this->weight}; }
        int get_base_xp() const { return this->base_exp; }
        int get_order() const { return this->order; }
        int *get_moves() const { return this->movesList; }
        
};

#endif