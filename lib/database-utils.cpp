#include "database-utils.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <climits>
#include <ncurses.h>

#include "pokemon.h"

//globals
std::vector<Pokemon> pokemon;
std::vector<Move> moves;
std::vector<PokeMove> pokeMoves;
std::vector<PokeSpecies> pokeSpecies;
std::vector<Exp> exps;
std::vector<Type> types;
std::vector<PokeStats> pokeStats;
std::vector<Stats> stats;
std::vector<PokeType> pokeTypes;


//Im a theif
std::vector<std::string> pokebase_explode(const std::string& str, const char& ch) {
    std::string next;
    std::vector<std::string> result;

    // For each character in the string
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!std::next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            } else {
                result.push_back(std::to_string(INT_MAX));
            }
        } else {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
         result.push_back(next);
    return result;
}

int load_pokeMoves(char *path);

int load_moves(char *path)
{
    ifstream file;
    char *filename;
    char str[200];
    std::vector<std::string> parts;
    Move move;


    strcpy(filename, path);
    strcat(filename, "moves.csv");

    file.open(filename);

    file.getline(str, 200);
    while (!file.eof()) {
        file.getline(str, 200);
        parts = pokebase_explode(str, ',');

        move.id = atoi(parts.at(0));
        move.identifier = parts.at(1);
        move.gen_id = atoi(parts.at(2));
        move.type_id = atoi(parts.at(3));
        move.power = atoi(parts.at(4));
        move.pp = atoi(parts.at(5));
        move.accuracy = atoi(parts.at(6));
        move.priority = atoi(parts.at(7));
        move.target_id = atoi(parts.at(8));
        move.damage_class_id = atoi(parts.at(9));
        move.effect_id = atoi(parts.at(10));
        move.effect_chance = atoi(parts.at(11));
        move.contest_type_id = atoi(parts.at(12));
        move.contest_effect_id = atoi(parts.at(13));
        move.super_contest_effect_id = atoi(parts.at(14));

        moves.push_back(move);
    }

    return 0;
}

int load_pokeSpecies(char *path);

int load_exp(char *path);

int load_types(char *path);

int load_pokeStats(char *path);

int load_stats(char *path);

int load_pokeTypes(char *path);

int load_pokemon(char *path);

int print_file(char *path) {
    ifstream file;
    std::vector<std::string> parts, top;
    char str[200];
    int i, j, lines;
    int spaces = {0, 2, 15, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60};


    mvprintw(0, 0, "Starting file print.")
    getch();

    file.open(path);

    top = pokebase_explode(str, ',');

    j = 0;
    for (std::vector<std::string>::iterator it = top.begin(); 
            it != top.end(); ++it) {
        mvprintw(0, j, "%s", *it);
        j += (*it).length() + 1;
    }
    i = 0;
    while (!file.eof()) {
        if (i > 45) {
            printw("\nlines through %d\t(Press any key to see the next page.)",
                    lines);
            getch();
            j = 0;
            for (std::vector<std::string>::iterator it = top.begin(); 
                    it != top.end(); ++it) {
                mvprintw(0, spaces[j], "%s", *it);
                j++;
            }
            i = 0;
        }
        file.getline(str, 200);
        parts = pokebase_explode(str, ',');
        j = 0;
        for (std::vector<std::string>::iterator it = parts.begin(); 
                it != parts.end(); ++it) {
            mvprintw(i + 1, spaces[j], "%s", *it);
            j++;
        }
        i++;
        lines++;
    }

    printw("\n (Press any key to exit)");
    getch();

    return 0;
}

// init
int load_database(char *fn)
{
    ifstream file;
    int i;
    char *pokePath = "pokedex/pokedex/data/csv/";
    char *prefix = {"/share/cs327/", "$HOME/.poke327/", "./"};
    char *pf = strdup("");
    char *path;


    mvprintw(0, 0, "Starting loading database.");
    getch();

    strcpy(path, prefix[0]);
    strcat(path, pokePath);
    while (!file.open(path) && i < sizeof(prefix)/sizeof(prefix[0])) {
        i++;
        strcpy(path, prefix[i]);
        strcat(path, pokePath);
    }
    i++;

    if (i <= sizeof(prefix)/sizeof(prefix[0])) {
        mvprintw(0, 0, "found path.");
        getch();
        file.close();
        if (fn != "" && fn != NULL) {
            strcpy(pf, path);

            if (fn[0] == 'p') {
                strcat(pf, "pokemon_");
            }

            strcat(pf, fn);

            print_file(pf);

            return 0;
        }

        load_moves(path);
        load_pokeMoves(path);
        load_types(path);
        load_pokeTypes(path);
        load_stats(path);
        load_pokeStats(path);
        load_exp(path);
        load_pokemon(path);
    } else {
        mvprintw(0, 0, "There was an error loading the database. (Press any key)\n");
        getch();
        return 1;
    }

    return 0;
}