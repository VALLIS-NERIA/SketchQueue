#include "sketch_queue.h"

struct pkt_queue* new_pkt_queue(size_t bufsize) {
    struct pkt_queue* me = new(struct pkt_queue);
    me->data = newarr(struct entry, bufsize);
    me->n = bufsize;
    me->head = 0;
    me->tail = 0;
    return me;
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

void delete_pkt_queue(struct pkt_queue* me) {
    kfree(me->data);
    kfree(me);
}

void sketch_queue_parse(struct sketch_queue* me, char* s) {
    char* arg[64];
    int i = 0;
    arg[i++] = s;
    while (*s != '\0') {
        if (*s == ' ') {
            *s = '\0';
            s++;
            arg[i++] = s;            
        }
        s++;
    }
    if(i<me->sketch_count) {
        error("insufficient params\n");
        return;
    }
}

void init_sketch_queue(struct sketch_queue* me, char** arg) {
    for (int i = 0; i < me->sketch_count; i++) {
        int para1, para2;
        char* s = arg[i];
        while (*s != '\0') {
            if (*s == ',') {
                *s = '\0';
                s++;
                if (!kstrtos32(arg[i], 10, &para1)) {
                    if (!kstrtos32(s, 10, &para2)) {
                        me->sketches[i].instance = me->sketches[i].create(para1, para2);
                        goto next;
                    }
                }
                error("invaild argument");
            }
            s++;
        }
        if (!kstrtos32(arg[i], 10, &para1)) {
            me->sketches[i].instance = me->sketches[i].create(para1);
        }
        else {
            error("invaild argument");
        }
    next:;
    }
    //kfree(arg);
}

struct sketch_queue* new_sketch_queue(uint32_t sketch_mask) {
    struct sketch_queue* q = new(struct sketch_queue);
    int count;
    uint32_t x = sketch_mask;
    // gets the number of "1" bit of x.
    for (count = 0; x; count++) x &= x - 1;
    q->sketches = newarr(struct sketch_base, count);
    uint32_t mask = 1;
    for (int i = 0; i < 32; (i++, mask >>= 1)) {
        uint32_t c = (sketch_mask & mask);
        if (c != 0) {
            q->sketches[q->sketch_count++] = get_sketch_class(c);
        }
    }
    return q;
}
