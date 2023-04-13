#include "pokemon.h"


Pokemon::Pokemon(int id, const char *iden, int spec_id, int h, int w, int b_exp, int ord, int is_def):
    Entity(-1, Point(-1, -1), num_types_ter)
{
    this->id = id;
    this->identifier = iden;
    this->species_id = spec_id;
    this->height = h;
    this->weight = w;
    this->base_exp = b_exp;
    this->order = ord;
    this->movesList = NULL;
    this->is_default = is_def;
}

Pokemon::Pokemon(int id, const char *iden, int spec_id, int h, int w, int b_exp, int ord, int *moveList, int is_def):
    Entity(-1, Point(-1, -1), num_types_ter)
{
    this->id = id;
    this->identifier = iden;
    this->species_id = spec_id;
    this->height = h;
    this->weight = w;
    this->base_exp = b_exp;
    this->order = ord;
    this->movesList = moveList;
    this->is_default = is_def;
}
