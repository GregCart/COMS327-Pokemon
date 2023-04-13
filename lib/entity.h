#ifndef ENTITY_H
#define ENTITY_H


#include "structs.h"


class Map;

class Entity {
    private:
        int chr;
        
    protected:
        int nextTime;
        bool defeated;
    public:
        heap_node_t *hn;
        Point pos;
        Dir_e dir;
        Terrain_e start;
        int (*do_move)(Entity *, Map *m, Plane<char> map);
        int turnOrder;


        Entity();
        Entity(const int i, const Point p, const Terrain_e st);
        Entity(const int i, const Point p, const Terrain_e st, const Dir_e d);
        ~Entity();

        bool operator==(Entity *e) const { return this->start == e->start; }
        bool operator!=(Entity *e) const { return this->start != e->start; }

        bool is_defeated() const { return this->defeated; }
        int get_chr() const { return this->chr; }
        int check_time() const { return this->nextTime; }
        int check_order() const {return this->turnOrder; }

        void defeat() { this->defeated = true; }
        void update_time(const int i) { this->nextTime += i; }
        void set_time(const int i) { this->nextTime = i; }

        bool compare(const void *with) const;

        int print_entity() const;

        friend int32_t entity_cmp(const void *key, const void *wwith);
};

#endif