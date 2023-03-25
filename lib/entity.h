#ifndef ENTITY_H
#define ENTITY_H

#include "heap.h"
#include "constants.h"
#include "utils-misc.h"


class Entity {
    private:
        int chr;
        
    protected:
        int nextTime;
        int order;
        bool defeated;
        Terrain_e start;


        int print_entity() const;
    public:
        heap_node_t *hn;
        Point pos;
        Dir_e dir;
        int (*do_move)(Map *m, char map[BOUNDS_Y][BOUNDS_X][10]);


        Entity(const int i, const Point p, const Terrain_e st);
        Entity(const int i, const Point p, const Terrain_e st, const Dir_e d);
        ~Entity() { free(this); }

        bool operator==(Entity *e) const { return this->start == e->start; }
        bool operator!=(Entity *e) const { return this->start != e->start; }

        bool is_defeated() const { return this->defeated; }
        int get_chr() const { return this->chr; }
        Point get_pos() const { return this->pos; }

        void defeat() { this->defeated = true; }
        void update_time(const int i) { this->nextTime += i; }

        bool compare(const void *with) const;
};

#endif