#include <cstdlib>
#include "queue.h"

int queue_enqueue(queue_t *q, int i)
{
    queue_node_t *n;

    if (!(n = (queue_node_t *) malloc(sizeof(*n)))) {
        return 1;
    }

    n->data = i;
    n->next = NULL;
    
    q->length++;
    if (q->back) {
        q->back->next = n;
    } else {
        q->front = n;
    }
    q->back = n;

    return 0;
}

int queue_dequeue(queue_t *q, int *i)
{
    if (!q->length) {   //q is empty
        return 1;
    }

    queue_node_t *tmp;


    *i = q->front->data;
    tmp = q->front;
    q->front = q->front->next;
    free(tmp);

    if (!--q->length) {
        q->back = NULL;
    }

    return 0;
}

int queue_front(queue_t *q, int *i)
{
    if (!q->length) {   //q is empty
        return 1;
    }

    *i = q->front->data;

    return 0;
}

int queue_length(queue_t *q)
{
    return q->length;
}

int queue_is_empty(queue_t *q)
{
    return !q->length;
}

int queue_init(queue_t *q) 
{
    q->front = q->back = NULL;
    q->length = 0;

    return 0;
}

int queue_destroy(queue_t *q)
{
    queue_node_t *tmp;

    while ((tmp = q->front)) {  //extra parenthesis supresses warning
        q->front = q->front->next;
        free(tmp);
    }
    /* These assignments are not necesary, because the queue is already 
     * destroyed. It is an error to use this queue again, regaurdless
     * of whether or not we do these assignments
     */

    q->back = NULL;
    q->length = 0;

    return 0;
}