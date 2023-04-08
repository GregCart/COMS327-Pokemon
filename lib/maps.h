#ifndef MAPS_H
#define MAPS_H


#include "structs.h"

class Entity;
class Trainer;

class Map {
    private:
        Map(Plane<Terrain_e> t, Plane<int> a, int *g, bool b);
    protected:
        Plane<Terrain_e> terrain;
        Plane<int> alt;
        //n, s, e, w
        int gates[4];


        //!chess
        int check_map() const;

        //ine de beningi
        int make_boundary(const Point curPos);
        int add_terrain(const int x, const int y, const Terrain_e c);
        int spread_seed(queue_t *qx, queue_t *qy);
        int fill_map();
        int trailblaze(const Point curPos, const Point center);
    public:
        Trainer **trainers;
        heap_t order;

        //HPs
        int print_map_alt() const;
        int print_map_terrain() const;
        int print_map() const;
        int print_cost_map() const;

        //Bobs
        Map() {}
        Map(const Point curPos, const Point center);
        Map(const Point curPos, const Point center, int gates[4]);
        Map(Map *m);
        ~Map();

        //Peepers
        Plane<Terrain_e> get_map_terrain() const;
        Plane<int> get_map_alt() const;
        int (&get_map_gates()) [4];
        
        //-cats
        Plane<Terrain_e> copy_map_terrain(Map *m);
        Plane<int> copy_map_alt(Map *m);
        Map *copy_map(Map *m);

        //in stone
        void set_map_terrain(Plane<Terrain_e> t);
        void set_map_alt(Plane<int> i);
        void set_map(Map *m);

        //Indi Beningi
        int create_map(const Point curPos, const Point center);
        int create_map(const Point curPos, const Point center, const int gates[4]);


        //tomodachi
        friend int dijkstra(Map *m, Map *w, Point p, Entity *e);
        friend Dir_e get_lower_alt(Point p, Map *m);
};

#endif