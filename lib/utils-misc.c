#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "lib/queue.h"

#include "structs.c"


//comperators
static int32_t path_cmp(const void *key, const void *with) 
{
  return ((Path *) key)->cost - ((Path *) with)->cost;
}

static int32_t entity_cmp(const void *key, const void *with) 
{
  int ret;

  ret = ((Entity *) key)->nextTime - ((Entity *) with)->nextTime;
  if (ret == 0) {
    ret = ((Entity *) key)->order - ((Entity *) with)->order;
  }

  return ret;
}

//getters
int find_stress(Map *m, Entity *e, Point p, Terrain_e *startTer)
{
    switch (e->chr){
        case PLAY:
        case RIVL:
        case HIKR:
        case SWIM:
        case EXPL:
            return STRESS[e->chr][m->terrain[p.y][p.x]];
            break;
        case PACR:
        case WAND:
            return m->terrain[p.y][p.x] == startTer[startTer[e->order]]? STRESS[0][m->terrain[p.y][p.x]]: STRESS[e->chr][m->terrain[p.y][p.x]];
            break;
        case SENT:
            return D_MAX;
            break;
        default:
            fprintf(stderr, "Error finding stress for %c\n", ALL_TRAINERS[e->chr]);
            return -1;
    }
    return -1;
}

Point get_next_position(Point p, Dir_e d)
{
    switch (d) {
        case NW:
            return (Point) {.x = p.x - 1, .y = p.y - 1};
        case N:
            return (Point) {.x = p.x, .y = p.y - 1};
        case NE:
            return (Point) {.x = p.x + 1, .y = p.y - 1};
        case W:
            return (Point) {.x = p.x - 1, .y = p.y};
        case E:
            return (Point) {.x = p.x + 1, .y = p.y};
        case SW:
            return (Point) {.x = p.x - 1, .y = p.y + 1};
        case S:
            return (Point) {.x = p.x, .y = p.y + 1};
        case SE:
            return (Point) {.x = p.x + 1, .y = p.y + 1};
        default:
            // fprintf(stderr, "Error getting next position from (%d, %d) in direction %d\n", p.x, p.y, d);
            return p;
    }
}

Dir_e get_lower_alt(Point p, Map *m)
{
    int i;
    Point q = get_next_position(p, 1);
    // print_point(p);

    for (i = 1; i < num_dir; i++) {
        // print_point(q);
        if (q.x > 0 && q.x < (BOUNDS_X - 1) && q.y > 0 && q.y < (BOUNDS_Y - 1) && valid_move_ter(m->terrain[q.x][q.y])) {
            q = get_next_position(p, i);
            if (m->alt[q.x][q.y] <= m->alt[p.x][p.y]) {
                return i;
            }
        }
    }

    return -1;
}

//distance finders
int manhattan(Point p, Point q)
{
    int ret;
    // printf("(%d, %d) and (%d, %d)\n", p.x, p.y, q.x, q.y);
    ret = abs(p.x - q.x) + abs(p.y - q.y);
    // printf("ret:%d\n", ret);
    return ret;
}

static int dijkstra(Map *m, Map *w, Point p, Entity *e, Terrain_e startTer)
{
    // printf("Start Dijk's\n");
    static Path path[BOUNDS_Y][BOUNDS_X], *pth;
    static int init = 0;
    heap_t h;
    int x, y, c;
    Point pnt;

    if (!init) {
        for (y = 0; y < BOUNDS_Y; y++) {
            for (x = 0; x < BOUNDS_X; x++) {
                path[y][x].pos[0] = y;
                path[y][x].pos[1] = x;
            }
        }
        init = 1;
    }

    // printf("Init paths\n");
    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            path[y][x].cost = D_MAX;
        }
    }

    path[p.y][p.x].cost = 0;

    heap_init(&h, path_cmp, NULL);

    // printf("fill heap\n");
    for (y = 1; y < BOUNDS_Y - 1; y++) {
        for (x = 1; x < BOUNDS_X - 1; x++) {
            if (find_stress(w, e, (Point){.x=x, .y=y}, startTer) != D_MAX) {
                path[y][x].hn = heap_insert(&h, &path[y][x]);
                // printf("In\t\t");
            }
        }
        // printf("\n");
    }
    // printf("\n");

    // printf("loop start\n");
    c = 0;
    while ((pth = heap_remove_min(&h))) {
        pth->hn = NULL;
        Point here = (Point) {.x = pth->pos[1], .y = pth->pos[0]};

        // printf("check above\n");//above
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check TL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99 
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //TL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check TR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] - 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //TR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check left\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //left
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check right\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0]};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //right
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check bottom\n");
        pnt = (Point) {.x = pth->pos[1], .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //bottom
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check BL\n");
        pnt = (Point) {.x = pth->pos[1] - 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //BL
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }
        // printf("check BR\n");
        pnt = (Point) {.x = pth->pos[1] + 1, .y = pth->pos[0] + 1};
        if ((path[pnt.y][pnt.x].hn) && w->alt[pnt.y][pnt.x] < 99
            && (path[pnt.y][pnt.x].cost > ((pth->cost + find_stress(w, e, here, startTer))))) {    //BR
                path[pnt.y][pnt.x].cost = pth->cost + find_stress(w, e, here, startTer);
                //from stuff for drawing nodes, don't need for part 3
                path[pnt.y][pnt.x].from[0] = pth->pos[0];
                path[pnt.y][pnt.x].from[1] = pth->pos[1];
                heap_decrease_key_no_replace(&h, path[pnt.y][pnt.x].hn);
                c++;
        }

        // printf("X: %d, y:%d, cost:%d\n", pth->pos[0], pth->pos[1], path[y][x].cost);
    }
    // printf("#less: %d\n", c);

    // printf("fill cost map\n");
    for (y = 0; y < BOUNDS_Y; y++) {
        for (x = 0; x < BOUNDS_X; x++) {
            // printf("X: %d, y:%d, cost:%d\n", x, y, path[y][x].cost);
            m->alt[y][x] = path[y][x].cost;
        }
    }

    return 0;
}

//HP's
int print_display(char map[BOUNDS_Y][BOUNDS_X][10])
{
    int i, j;

    for (i = 0; i < BOUNDS_Y; i++) {
        for (j = 0; j < BOUNDS_X; j++) {
            printf("%s", map[i][j]);
        }
        printf("\n");
    }

    return 0;
}

int print_entity(Entity *e) 
{
    printf("Entity: {\n");
    printf("\tChar: %c\n", ALL_TRAINERS[e->chr]);
    printf("\tPos: (%d, %d),\n", e->pos.x, e->pos.y);
    printf("\tCurrent direction: %d\n", e->dir);
    printf("}\n");

    return 0;
}

int print_point(Point p)
{
    printf("(%d, %d)\n", p.x, p.y);

    return 0;
}

//!chess
int valid_move_ter(Terrain_e t)
{
    return !(t == GTE || t == BDR);
}
