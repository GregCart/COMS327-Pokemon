#ifndef POKEMON_H
#define POKEMON_H


class Pokemon {
    private:
        int id;
        char *identifier;
        int species_id;

    protected:
        int height, weight;
        int base_exp;
        int order;
        int *movesList;

    public:
        bool is_default;


        int get_id();
        char *get_identifier();
        int get_species_id();

        int *get_height_weight();
        int get_base_xp();
        int get_order();

        
};

#endif