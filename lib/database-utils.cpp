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
vector<TypeName> typeNames;
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
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    PokeMove data;
    int i;


    strcpy(filename, path);
    strcat(filename, "pokemon_moves.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();
    refresh();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(6);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < 6; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.poke_id = stoi(parts.at(0).c_str());
            data.version_group_id = stoi(parts.at(1).c_str());
            data.move_id = stoi(parts.at(2).c_str());
            data.pokemon_move_method_id = stoi(parts.at(3).c_str());
            data.level = stoi(parts.at(4).c_str());
            data.order = stoi(parts.at(5).c_str());

            pokeMoves.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_moves(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    Move move;
    int i;


    strcpy(filename, path);
    strcat(filename, "moves.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();
    refresh();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(15);
            // mvprintw(1, 0, "for %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < 15; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            move.id = stoi(parts.at(0).c_str());
            move.identifier = parts.at(1).c_str();
            move.gen_id = stoi(parts.at(2).c_str());
            move.type_id = stoi(parts.at(3).c_str());
            move.power = stoi(parts.at(4).c_str());
            move.pp = stoi(parts.at(5).c_str());
            move.accuracy = stoi(parts.at(6).c_str());
            move.priority = stoi(parts.at(7).c_str());
            move.target_id = stoi(parts.at(8).c_str());
            move.damage_class_id = stoi(parts.at(9).c_str());
            move.effect_id = stoi(parts.at(10).c_str());
            move.effect_chance = stoi(parts.at(11).c_str());
            move.contest_type_id = stoi(parts.at(12).c_str());
            move.contest_effect_id = stoi(parts.at(13).c_str());
            move.super_contest_effect_id = stoi(parts.at(14).c_str());

            moves.push_back(move);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_pokeSpecies(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    PokeSpecies data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "pokemon_species.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 20;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.id = stoi(parts.at(0).c_str());
            data.identifier = parts.at(1).c_str();
            data.gen_id = stoi(parts.at(2).c_str());
            data.evolves_from_species_id = stoi(parts.at(3).c_str());
            data.evolution_chain_id = stoi(parts.at(4).c_str());
            data.color_id = stoi(parts.at(5).c_str());
            data.shape_id = stoi(parts.at(6).c_str());
            data.habitat_id = stoi(parts.at(7).c_str());
            data.gender_rate = stoi(parts.at(8).c_str());
            data.capture_rate = stoi(parts.at(9).c_str());
            data.base_happiness = stoi(parts.at(10).c_str());
            data.is_baby = stoi(parts.at(11).c_str());
            data.hatch_counter = stoi(parts.at(12).c_str());
            data.has_gender_differences = stoi(parts.at(13).c_str());
            data.growth_rate_id = stoi(parts.at(14).c_str());
            data.forms_switchable = stoi(parts.at(15).c_str());
            data.is_legendary = stoi(parts.at(16).c_str());
            data.is_mythical = stoi(parts.at(17).c_str());
            data.order = stoi(parts.at(18).c_str());
            data.conquest_order = stoi(parts.at(19).c_str());

            pokeSpecies.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_exp(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    Exp data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "experience.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 3;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.growth_rate_id = stoi(parts.at(0).c_str());
            data.level = stoi(parts.at(1).c_str());
            data.experience = stoi(parts.at(2).c_str());

            exps.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_types(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    TypeName data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "type_names.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 4;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.type_id = stoi(parts.at(0).c_str());
            data.local_language_id = stoi(parts.at(1).c_str());
            data.name = parts.at(2).c_str();

            typeNames.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_pokeStats(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    PokeStats data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "pokemon_stats.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 4;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.pokemon_id = stoi(parts.at(0).c_str());
            data.stat_id = stoi(parts.at(1).c_str());
            data.base_stat = stoi(parts.at(2).c_str());
            data.effort = stoi(parts.at(3).c_str());

            pokeStats.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_stats(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    Stats data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "stats.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 5;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.id = stoi(parts.at(0).c_str());
            data.damage_class_id = stoi(parts.at(1).c_str());
            data.identifier = parts.at(2).c_str();
            data.is_battle_only = stoi(parts.at(3).c_str());
            data.game_index = stoi(parts.at(4).c_str());

            stats.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

    return 0;
}

int load_pokeTypes(char *path)
{
    ifstream file;
    char filename[100], str[250];
    vector<string> parts;
    PokeType data;
    int i, size;


    strcpy(filename, path);
    strcat(filename, "pokemon_types.csv");
    mvprintw(0, 0, "Loading file %s", filename);
    clrtoeol();

    file.open(filename, ios::binary);

    mvprintw(0, 0, "Opened file %s", filename);
    clrtoeol();

    if (file.is_open()) {
        file.getline(str, 250);
        size = 3;
        while (!file.eof()) {
            file.getline(str, 250);
            parts = pokebase_explode(str, ',');
            parts.resize(size);
            // mvprintw(1, 0, "For# %s: %s", parts.at(0).c_str(), str);
            // clrtoeol();
            // refresh();

            for (i = 0; i < size; i++) {
                if (parts.at(i).empty()) {
                    parts.at(i).assign(to_string(INT_MAX).c_str());
                }
            }

            data.pokemon_id = stoi(parts.at(0).c_str());
            data.type_id = stoi(parts.at(1).c_str());
            data.slot = stoi(parts.at(2).c_str());

            pokeTypes.push_back(data);
        }
    } else {
        clear();
        mvprintw(0, 0, "Error Trying to open file %s. (Press any key)", filename);
        getch();
    }

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
    const char *prefix[] = {"./", "/share/cs327/", home};
    char pf[150] = "";
    char path[200] = "";


    clear();
    mvprintw(0, 0, "Starting loading database.");

    strcpy(pf, prefix[0]);
    strcat(pf, pokePath);
    strcat(path, pf);
    strcat(path, "pokemon.csv");
    file.open(path, ios::binary);
    mvprintw(0, 0, "Testing path %s", path);
    clrtoeol();
    i = 0;
    while (!file.is_open() && (unsigned) i < sizeof(prefix)/sizeof(prefix[0])) {
        i++;
        strcpy(pf, "");
        strcpy(pf, prefix[i]);
        strcat(pf, pokePath);
        strcpy(path, pf);
        strcat(path, "pokemon.csv");
        file.open(path, ios::binary);
        mvprintw(0, 0, "Testing path %s", path);
        clrtoeol();
    }
    i++;

    if (file.is_open() || (unsigned) i <= sizeof(prefix)/sizeof(prefix[0])) {
        clear();
        mvprintw(0, 0, "found path %s", pf);
        if (fn != NULL) {
            mvprintw(0, 0, "Printing file %s", fn);

            if (fn[0] == 'p') {
                strcat(pf, "pokemon_");
            }

            strcat(pf, fn);
            strcat(pf, ".csv");

            print_file(pf);

            return 0;
        }
        try {
            load_moves(pf);
            load_pokeMoves(pf);
            load_types(pf);
            load_pokeTypes(pf);
            load_stats(pf);
            load_pokeStats(pf);
            load_exp(pf);
            load_pokemon(pf);
        } catch (exception &e) {
            printw("\nThere was an error loading the database: %s. (Press any key)", e.what());
            getch();
            return 1;
        }
    } else {
        mvprintw(0, 0, "Could not find/open database. (Press any key)\n");
        getch();
        return 1;
    }

    return 0;
}