#ifndef SKETCH_QUEUE_H
#define SKETCH_QUEUE_H
#include "flow_key.h"
#include "sketches.h"
struct entry {
    struct flow_key key;
    uint16_t size;
    uint16_t tag;
};


struct pkt_queue {
    struct entry* data;
    size_t n;
    /**
    * ....XXXXXXX...
    *     ^      ^
    *     |      |
    *     head   tail
    * XXXX.......XXX
    *     ^      ^
    *     |      |
    *     tail   head
    */
    int head;
    int tail;
    int empty;
};

struct pkt_queue* new_pkt_queue(size_t bufsize);

int pq_size(struct pkt_queue* me);

int pq_peek(struct pkt_queue* me, struct entry* ret);

static inline int pq_empty(struct pkt_queue* me) {
    return !(me->tail == me->head);
}

void delete_pkt_queue(struct pkt_queue* me);



struct sketch_queue {
    struct sketch_base* sketches;
    struct pkt_queue* queue;
    int sketch_count;
};

/**
 * @brief: alloc a new sketch_queue, and initalize the vtables of every sketch.
 * @param sketch_mask: a combination of sketches. Each bit determines the corresponding sketch is assigned or not. \
 For the meaning of every bit, see the definition of enum sketch_type.
 */
struct sketch_queue* new_sketch_queue(uint32_t sketch_mask);

void sketch_queue_parse(struct sketch_queue* me, char* s);

/* arg: each string should contians an integer or multiple integers sperated with commas. */
void init_sketch_queue(struct sketch_queue* me, char** arg);

void delete_sketch_queue(struct sketch_queue* me);
#endif // SKETCH_QUEUE_H
