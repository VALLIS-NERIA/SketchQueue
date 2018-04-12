#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "countmax.h"
#include "flow_key.h"
#include "sketch_queue.h"
#include "sketches.h"

#include <linux/sched.h>
#include <linux/kthread.h>


MODULE_LICENSE("Dual MIT/GPL");
static int queue_count;
static struct sketch_queue** queues;
static int count;
static int count1;

int sketch_queue_work(struct sketch_queue* me) {
    while (!kthread_should_stop()) {
        if (pq_empty(me->queue)) { continue; }
        struct entry e;
        int err = pq_peek(me->queue, &e);
        if (!err) {
            pq_pop(me->queue);
        }
        int i = 0;
        for (i = 0; i < me->sketch_count; ++i) {
            me->sketches[i].update(me->sketches[i].instance, &(e.key), e.size);
        }
        ++count1;
    }
    return 0;
}

struct task_struct* qt;

void sketch_queue_start(struct sketch_queue* me) {
    qt = kthread_create(sketch_queue_work, me, "work thread");
    kthread_bind(qt, 1);
    wake_up_process(qt);
    //kernel_thread(sketch_queue_work, me, CLONE_FS);
}


void process_packet(struct entry e) {
    int i = 0;
    for (i = 0; i < queue_count; ++i) {
        struct sketch_queue* q = queues[i];
        if (e.tag & q->mask) {
            pq_push(q->queue, e);
            ++count;
        }
    }
}

//static struct sketch_queue* q;

static int __init init(void) {
    printk("init begin!!!!\n");
    count = 0;
    count1 = 0;
    queue_count = 1;
    queues = newarr(struct sketch_queue*, 1);
    struct sketch_queue* q = new_sketch_queue(3, 1000);
    queues[0] = q;
    printk("init!!!!\n");
    sketch_queue_parse(queues[0], "100,2 100");
    printk("init ok!\n");
    sketch_queue_start(queues[0]);
    printk("start ok!\n");
    int c = 10000;
    while (c-- > 0) {
        struct entry e;
        e.key = rand_flow_key();
        e.size = rand_uint16();
        e.tag = rand_uint16();
        process_packet(e);
    }
    printk("process ok!\n");
    return 0;
}

static void clean(void) {
    kthread_stop(qt);
    udelay(1000)
    ;
    delete_sketch_queue(queues[0]);
    kfree(queues);
    printk("count: %d, count1: %d\n", count, count1);
    printk("exit!\n");
}

module_init(init);
module_exit(clean);
