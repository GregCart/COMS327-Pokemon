#ifndef STRUCTS_H
#define STRUCTS_H


#include "heap.h"
#include "constants.h"
#include "queue.h"


typedef struct point {
    int x, y;
    point(const int x, const int y) { this->x = x; this->y = y; }
    point() { this->x = 0; this->y = 0; };
} Point;

typedef struct path {
    heap_node_t *hn;    
    int pos[2];
    int from[2];
    int cost;
} Path;

typedef struct move {
    int id;
    char *identifier;
    int gen_id, type_id;
    int power, pp, accuracy, priority;
    int target_id, damage_class_id;
    int effect_id, effect_chance;
    int contest_type_id, contest_effect_id, super_contest_effect_id;
} Move;

typedef struct pokemon_move {
    int poke_id, version_group_id, move_id, pokemon_move_method_id;
    int level, order;
} PokeMove;

typedef struct pokemon_species {
    int id;
    char *identifier;
    int gen_id, evolves_from_species_id, evolution_chain_id;
    int color_id, shape_id, habitat_id;
    int gender_rate, capture_rate;
    int base_happiness;
    bool is_baby;
    int hatch_counter;
    bool has_gender_differences;
    int growth_rate_id;
    bool forms_switchable, is_legendary, is_mythical;
    int order, conquest_order;
} PokeSpecies;

typedef struct experience {
    int growth_rate_id, level, experience;
} Exp;

typedef struct type_name {
    int type_id,local_language_id;
    char *name;
} Type;

typedef struct pokemon_stats {
    int pokemon_id, stat_id, base_stat, effort;
} PokeStats;

typedef struct stats {
    int id, damage_class_id;
    char *identifier;
    bool is_battle_only;
    int game_index;
} Stats;

typedef struct pokemon_type {
    int pokemon_id, type_id, slot;
} PokeType;

#endif