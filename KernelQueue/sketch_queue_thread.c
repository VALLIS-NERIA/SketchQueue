#include <linux/sched.h>
#include <linux/kthread.h>
#include "sketch_queue.h"

struct skq_thread {};



int sketch_queue_work(struct sketch_queue* me) {
    while (!kthread_should_stop()) {
        while (pq_empty(me->queue)) {}
        struct entry e;
        int err = pq_peek(me->queue, &e);
        if (!err) {
            pq_pop(me->queue);
        }
        int i = 0;
        for (i = 0; i < me->sketch_count; ++i) {
            me->sketches[i].update(me->sketches[i].instance, &(e.key), e.size);
        }
    }
    return 0;
}

void sketch_queue_start(struct sketch_queue* me) {
    kernel_thread(sketch_queue_work, me, CLONE_FS);
}

int queue_count;
struct sketch_queue* queues;

void process_packet(struct entry e) {
    int i = 0;
    for (i = 0; i < queue_count; ++i) {
        struct sketch_queue* q = &queues[i];
        if (e.tag & q->mask) {
            pq_push(q->queue, e);
        }
    }
}
