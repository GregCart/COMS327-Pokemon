#include "database-utils.h"

#include <fstream>
#include <cstring>

#include "pokemon.h"

//globals
const Pokemon pokemon[];
const Move moves[];
const PokeMove pokeMoves[];
const PokeSpecies pokeSpecies[];
const Exp exp[];
const Type type[];
const PokeStats pokeStats[];
const Stats stats[];
const PokeType pokeTypes[];


int load_pokeMoves(char *path);

int load_moves(char *path)
{
    ifstream file;
    char *filename;

    strcpy(filename, path);
    strcat(filename, "moves.csv");

    file.open(filename);


}

int load_pokeSpecies(char *path);

int load_exp(char *path);

int load_types(char *path);

int load_pokeStats(char *path);

int load_stats(char *path);

int load_pokeTypes(char *path);

int load_pokemon(char *path);

// init
int load_database()
{
    ifstream file;
    int i;
    char *pokePath = "pokedex/pokedex/data/csv/";
    char *prefix[] = {"/share/cs327/", "$HOME/.poke327/", "./"};
    char *path;

    strcpy(path, prefix[0]);
    strcat(path, pokePath);
    while (!file.open(path) && i < sizeof(prefix)/sizeof(prefix[0])) {
        i++;
        strcpy(path, prefix[i]);
        strcat(path, pokePath);
    }
    i++;

    if (i <= sizeof(prefix)/sizeof(prefix[0])) {
        file.close();

        load_moves(path);
        load_pokeMoves(path);
        load_types(path);
        load_pokeTypes(path);
        load_stats(path);
        load_pokeStats(path);
        load_exp(path);
        load_pokemon(path);
    } else {
        printf("There was an error loading the database. (Press any key)\n");
    }
}