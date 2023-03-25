#ifndef MAPS_H
#define MAPS_H


#include "heap.h"
#include "utils-misc.h"


class Map {
    private:
        Terrain_e terrain[BOUNDS_Y][BOUNDS_X];
        int alt[BOUNDS_Y][BOUNDS_X];
        //n, s, e, w
        int gates[4];


        Map(Terrain_e **t, int **a, int *g);
    protected:
        //HPs
        int print_map_alt() const;
        int print_map_terrain() const;
        int print_map() const;
        int print_cost_map() const;

        //!chess
        int check_map() const;

        //ine de beningi
        int make_boundary(const Point curPos);
        int add_terrain(const int x, const int y, const Terrain_e c);
        int spread_seed(const queue_t *qx, const queue_t *qy);
        int fill_map();
        int trailblaze(const Point curPos, const Point center);

        //glitch
        Map *mapdup(Map *m) const;
    public:
        Trainer **trainers;
        heap_t *order;


        //Bobs
        Map(const Point curPos, const Point center) { create_map(curPos, center); }
        Map(const Map *m) { mapdup(m); }
        ~Map() { free(this); }

        //-cats
        Terrain_e **copy_map_terrain() const;
        int **copy_map_alt() const;
        Map *copy_map() const;

        //Indi Beningi
        Map *create_map(const Point curPos, const Point center);


        //tomodachi
        friend int dijkstra(Map *m, Map *w, Point p, Entity *e);
        friend Dir_e get_lower_alt(Point p, Map *m);
};

#endif