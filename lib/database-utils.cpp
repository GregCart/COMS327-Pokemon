#include "database-utils.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <climits>
#include <ncurses.h>

#include "pokemon.h"


using namespace std;

//globals
vector<Pokemon> pokemon;
vector<Move> moves;
vector<PokeMove> pokeMoves;
vector<PokeSpecies> pokeSpecies;
vector<Exp> exps;
vector<Type> types;
vector<PokeStats> pokeStats;
vector<Stats> stats;
vector<PokeType> pokeTypes;


//Im a theif
vector<string> pokebase_explode(const string& str, const char& ch) {
    string next;
    vector<string> result;

    // For each character in the string
    for (string::const_iterator it = str.begin(); it != str.end(); it++) {
        // If we've hit the terminal character
        if (*it == ch) {
            // If we have some characters accumulated
            if (!next.empty()) {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            } else {
                result.push_back(to_string(INT_MAX));
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

int load_pokeMoves(char *path)
{
    return 0;
}

int load_moves(char *path)
{
    ifstream file;
    char filename[100];
    char str[200];
    vector<string> parts;
    Move move;


    strcpy(filename, path);
    strcat(filename, "moves.csv");

    file.open(filename);

    file.getline(str, 200);
    while (!file.eof()) {
        file.getline(str, 200);
        parts = pokebase_explode(str, ',');

        move.id = stoi(parts.at(0));
        strcpy(move.identifier, parts.at(1).c_str());
        move.gen_id = stoi(parts.at(2));
        move.type_id = stoi(parts.at(3));
        move.power = stoi(parts.at(4));
        move.pp = stoi(parts.at(5));
        move.accuracy = stoi(parts.at(6));
        move.priority = stoi(parts.at(7));
        move.target_id = stoi(parts.at(8));
        move.damage_class_id = stoi(parts.at(9));
        move.effect_id = stoi(parts.at(10));
        move.effect_chance = stoi(parts.at(11));
        move.contest_type_id = stoi(parts.at(12));
        move.contest_effect_id = stoi(parts.at(13));
        move.super_contest_effect_id = stoi(parts.at(14));

        moves.push_back(move);
    }

    return 0;
}

int load_pokeSpecies(char *path)
{
    return 0;
}

int load_exp(char *path)
{
    return 0;
}

int load_types(char *path)
{
    return 0;
}

int load_pokeStats(char *path)
{
    return 0;
}

int load_stats(char *path)
{
    return 0;
}

int load_pokeTypes(char *path)
{
    return 0;
}

int load_pokemon(char *path)
{
    return 0;
}

int print_file(char *path) {
    ifstream file;
    vector<string> parts, top;
    char str[200];
    int i, j, lines;
    vector<string>::iterator it, itl;


    clear();
    mvprintw(0, 0, "Starting file print from %s", path);
    getch();

    file.open(path);
    if (file.is_open()) {
        file.getline(str, 200);

        top = pokebase_explode(str, ',');

        clear();
        j = 0;
        for (it = top.begin(); it != top.end(); ++it) {
            mvprintw(0, j, "%s\t", (*it).c_str());
            j += (*it).length() + 5;
        }
        // mvprintw(0, 0, "%s", str);
        getch();
        i = 0;
        lines = 0;
        while (!file.eof()) {
            if (i > 45) {
                printw("\nlines through %d\t(Press any key to see the next page.)",
                        lines);
                getch();
                clear();
                j = 0;
                for (it = top.begin(); it != top.end(); ++it) {
                    mvprintw(0, j, "%s", (*it).c_str());
                    j += (*it).length() + 5;
                }
                i = 0;
            }
            file.getline(str, 200);
            parts = pokebase_explode(str, ',');
            j = 0;
            for (itl = parts.begin(), it = top.begin(); itl != parts.end(); ++it, ++itl) {
                if (strcmp((*itl).c_str(), to_string(INT_MAX).c_str())) {
                    mvprintw(i + 1, j, " %s", (*itl).c_str());
                }
                j += (*it).length() + 5;
            }
            i++;
            lines++;
        }
    } else {
        clear();
        mvprintw(0, 0, "Failed to open file.");
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
    const char *pokePath = "pokedex/pokedex/data/csv/";
    char *home = getenv("HOME");
        strcat(home, "./poke327");
    const char *prefix[] = {"/share/cs327/", home, "/Documents/SE 327/Pokemon/"};
    char pf[150];
    char path[200];


    mvprintw(0, 0, "Starting loading database.");

    strcpy(pf, prefix[0]);
    strcat(pf, pokePath);
    strcat(path, pf);
    strcat(path, "pokemon.csv");
    file.open(path, ios::binary);
    i = 0;
    while (!file.is_open() && (unsigned) i < sizeof(prefix)/sizeof(prefix[0])) {
        i++;
        strcpy(pf, prefix[0]);
        strcat(pf, pokePath);
        strcat(path, pf);
        strcat(path, "pokemon.csv");
        file.open(path, ios::binary);
    }
    i++;

    if ((unsigned) i <= sizeof(prefix)/sizeof(prefix[0])) {
        mvprintw(0, 0, "found path %s", path);
        if (fn) {
            mvprintw(0, 0, "Printing file %s", fn);

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