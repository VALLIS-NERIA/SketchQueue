#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "countmax.h"
#include "flow_key.h"
#include "sketch_queue.h"
#include "sketches.h"


MODULE_LICENSE("Dual MIT/GPL");
static struct sketch_queue* q;

static int __init init(void) {
    printk("init begin!!!!\n");
    q = new_sketch_queue(3, 1000);
    printk("init!!!!\n");
    sketch_queue_parse(q, "100,2 100");
    printk("init ok!\n");
    return 0;
}

static void clean(void) {
    delete_sketch_queue(q);
    printk("exit!\n");
}

module_init(init);
module_exit(clean);
