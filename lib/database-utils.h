#include "structs.h"


//forwards
class Pokemon;

//globals
extern const Pokemon pokemon[];
extern const Move moves[];
extern const PokeMove pokeMoves[];
extern const PokeSpecies pokeSpecies[];
extern const Exp exps[];
extern const Type types[];
extern const PokeStats pokeStats[];
extern const Stats stats[];
extern const PokeType pokeTypes[];


// init
int load_database();