#ifndef MAPS_H
#define MAPS_H

#include <cstdlib>
#include <cstring>

#include "structs.h"
#include "queue.h"
#include "trainers.h"


class Map {
    private:
        Map(Terrain_e **t, int **a, int *g);
    protected:
        Terrain_e terrain[BOUNDS_Y][BOUNDS_X];
        int alt[BOUNDS_Y][BOUNDS_X];
        //n, s, e, w
        int gates[4];


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
        int spread_seed(queue_t *qx, queue_t *qy);
        int fill_map();
        int trailblaze(const Point curPos, const Point center);
    public:
        Trainer **trainers;
        heap_t *order;


        //Bobs
        Map();
        Map(const Point curPos, const Point center) { create_map(curPos, center); }
        Map(const Point curPos, const Point center, int gates[4]) { create_map(curPos, center, gates); }
        Map(Map *m) { m = copy_map(); }
        ~Map() {  }

        //Peepers
        Terrain_e **get_map_terrain() const { return (Terrain_e **) this->terrain; }
        int **get_map_alt() { return (int **) this->alt; }
        int *get_map_gates() {return (int *) this->gates; }
        
        //-cats
        Terrain_e **copy_map_terrain() const;
        int **copy_map_alt() const;
        Map *copy_map() const;

        //in stone
        void set_map_terrain(Terrain_e **t) { memcpy(terrain, t, sizeof(**t)); };
        void set_map_alt(int **i) { memcpy(alt, i, sizeof(**i)); };
        void set_map(Map *m) { memcpy(this, m, sizeof(*m)); };

        //Indi Beningi
        int create_map(const Point curPos, const Point center);
        int create_map(const Point curPos, const Point center, int gates[4]);


        //tomodachi
        friend int dijkstra(Map *m, Map *w, Point p, Entity *e);
        friend Dir_e get_lower_alt(Point p, Map *m);
};

#endif