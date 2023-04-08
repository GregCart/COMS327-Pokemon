#include "entity.h"

#include <cstdlib>
#include <ncurses.h>


//Bobs
Entity::Entity(const int i, const Point p, const Terrain_e st) 
{ 
    this->chr = i;
    this->pos = p;
    this->start = st;
    this->nextTime = 0;
    this->defeated = 0;
    this->dir = static_cast<Dir_e>(0);
    this->hn = (heap_node_t *) malloc(sizeof(this->hn));
    this->do_move = NULL;
    this->dir = static_cast<Dir_e>(rand() % num_dir);
}

Entity::Entity(const int i, const Point p, const Terrain_e st, const Dir_e d) 
{ 
    this->chr = i;
    this->pos = p;
    this->start = st;
    this->nextTime = 0;
    this->defeated = 0;
    this->hn = (heap_node_t *) malloc(sizeof(this->hn));
    this->do_move = NULL;
    this->dir = d;
}

Entity::~Entity()
{
    
}

//comperators
bool Entity::compare(const void *with) const
{
  return(this)->nextTime == ((Entity *) with)->nextTime &&
    (this)->order == ((Entity *) with)->order;
}

//HPs
int Entity::print_entity()  const
{
    printw("Entity: {\n");
    printw("\tChar: %d\n", this->get_chr());
    printw("\tPos: (%d, %d),\n", this->pos.x, this->pos.y);
    printw("\tStart: %d\n", this->start);
    printw("\tTime: %d\n", this->nextTime);
    printw("\tHn? %d\n", this->hn == NULL);
    printw("\tMove? %d\n", this->do_move == NULL);
    printw("\tCurrent direction: %d\n", this->dir);
    printw("}\n");
    refresh();

    return 0;
}
