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

int pq_peek(struct pkt_queue* me, struct entry* ret) {
    if (me->head != me->tail) {
        *ret = me->data[me->head];
        return 0;
    }
    else {
        return -1; 
    }

}

void delete_pkt_queue(struct pkt_queue* me) {
    kfree(me->data);
    kfree(me);
}

void sketch_queue_parse(struct sketch_queue* me, char* s) {
    char buf[512];
    strcpy(buf, s);
    s = buf;
    char* arg[64];
    int i = 0;
    arg[i++] = s;
    printk("%s\n", s);
    while (*s != '\0') {
        if (i >= 64) break;
        if (*s == ' ') {
            *s = '\0';
            s++;
            arg[i++] = s;
            printk("%d : %s\n", i, s);
        }
        else {
            s++;
            printk("%d : %s\n", i, s);
        }
    }
    if (i < me->sketch_count) {
        error("insufficient params\n");
        return;
    }
    else {
        init_sketch_queue(me, arg);
    }
}

void init_sketch_queue(struct sketch_queue* me, char** arg) {
    int i = 0;
    for (i = 0; i < me->sketch_count; i++) {
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

void delete_sketch_queue(struct sketch_queue* me) {
    int i = 0;
    for (i = 0; i < me->sketch_count; i++) {
        me->sketches[i].delete(me->sketches[i].instance);
    }
    delete_pkt_queue(me->queue);
    kfree(me->sketches);
    kfree(me);
}

struct sketch_queue* new_sketch_queue(uint32_t sketch_mask) {
    struct sketch_queue* q = new(struct sketch_queue);
    q->queue = new_pkt_queue(1000);
    int count;
    uint32_t x = sketch_mask;
    // gets the number of "1" bit of x.
    for (count = 0; x; count++) x &= x - 1;
    q->sketches = newarr(struct sketch_base, count);
    uint32_t mask = 1;
    int i = 0;
    for (i = 0; i < 32; (i++, mask >>= 1)) {
        uint32_t c = (sketch_mask & mask);
        if (c != 0) {
            q->sketches[q->sketch_count++] = get_sketch_class(c);
        }
    }
    return q;
}
