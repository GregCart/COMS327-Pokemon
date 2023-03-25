#include "entity.h"


//Bobs
Entity::Entity(int i, Point p, Terrain_e st) 
{ 
    this->chr = i;
    this->pos = p;
    this->start = st;
    this->nextTime = 0;
    this->defeated = 0;
    this->hn = (heap_node_t *) malloc(sizeof(this->hn));
    this->do_move = NULL;
    this->dir = static_cast<Dir_e>(rand() % num_dir);
}

Entity::Entity(int i, Point p, Terrain_e st, Dir_e d) 
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

//comperators
bool Entity::compare(const void *with) const
{
  return(this)->nextTime == ((Entity *) with)->nextTime &&
    (this)->order == ((Entity *) with)->order;
}

