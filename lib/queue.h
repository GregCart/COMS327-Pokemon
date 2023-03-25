#ifndef QUEUE_H
#define QUEUE_H

// typedef extent_type new_name;

typedef struct queue_node{
    int data;
    struct queue_node *next;
} queue_node_t;

typedef struct queue{
    queue_node_t *front, *back;
    int length;
} queue_t;

int queue_enqueue(const queue_t *q, int i);
int queue_dequeue(const queue_t *q, int *i);
int queue_front(const queue_t *q, int *i);
int queue_length(const queue_t *q);
int queue_is_empty(const queue_t *q);
int queue_init(const queue_t *q);
int queue_destroy(const queue_t *q);

#endif