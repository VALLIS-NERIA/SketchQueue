#include "queue.h"

struct pkt_queue* new_pkt_queue(size_t bufsize) {
    struct pkt_queue* me = new(struct pkt_queue);
    me->data = newarr(struct entry, bufsize);
    me->n = bufsize;
    me->head = 0;
    me->tail = 0;
}

int pq_size(struct pkt_queue* me) {
    if (me->tail >= me->head) {
        return me->tail - me->head;
    }
    else {
        return me->n - me->head + me->tail;
    }
}

struct entry pq_peek(struct pkt_queue* me) {
    return me->data[me->head];

}
