#ifndef DB_UTILS_H
#define DB_UTILS_H


#include <vector>

#include "structs.h"


//forwards
class Pokemon;

//globals
extern std::vector<Pokemon> pokemon;
extern std::vector<Move> moves;
extern std::vector<PokeMove> pokeMoves;
extern std::vector<PokeSpecies> pokeSpecies;
extern std::vector<Exp> exps;
extern std::vector<Type> types;
extern std::vector<PokeStats> pokeStats;
extern std::vector<Stats> stats;
extern std::vector<PokeType> pokeTypes;


// init
int load_database(char *fn);

#endif