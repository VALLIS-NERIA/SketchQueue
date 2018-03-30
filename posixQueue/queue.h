#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>
#include <stdlib.h>
#define new(name) (name*)malloc(sizeof(name))
#define newarr(name, size) (name*)malloc(size * sizeof(name))
#define kfree free

struct flow_key {
    uint32_t srcip;
    uint32_t dstip;
    uint16_t srcport;
    uint16_t dstport;
    uint16_t protocol;
};

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

struct entry pq_peek(struct pkt_queue* me);


//struct sketch_queue { };
typedef unsigned long long elemtype;

struct sketch_class {
    void* (*new_sketch) (int ,...);

    void (*update) (void* this, struct flow_key* key, elemtype value);

    elemtype (*query)(void* this, struct flow_key* key);

    void (*delete_sketch) (void* this);

    int t;
};



#define SKETCH_CLASS(sketch_name, class_name)\
    struct sketch_class class_name = {  \
        .new_sketch = new_##sketch_name##_sketch,\
        .update = sketch_name##_sketch_update,\
        .query = sketch_name##_sketch_query,\
        .delete_sketch = delete_##sketch_name##_sketch\
    };

struct countmax_queue {
    struct pkt_queue* packet_q;
    struct countmax_sketch* sketch;
};


#define NEW_SKETCH_QUEUE(sketch_name, bufsize)        \
    struct ##sketch_name##_queue{                       \
        struct pkt_queue* packet_q;                   \
        struct ##sketch_name##_sketch* sketch;            \
    };
#endif // QUEUE_H
