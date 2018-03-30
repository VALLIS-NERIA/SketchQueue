#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "countmax.h"
#include "flow_key.h"

struct sketch_class {
    void* (*new_sketch) (int, ...);

    void(*update) (void* this, struct flow_key* key, elemtype value);

    elemtype(*query)(void* this, struct flow_key* key);

    void(*delete_sketch) (void* this);

    int t;
};



#define SKETCH_CLASS(sketch_name, class_name)\
    struct sketch_class class_name = {  \
        .new_sketch = new_##sketch_name##_sketch,\
        .update = sketch_name##_sketch_update,\
        .query = sketch_name##_sketch_query,\
        .delete_sketch = delete_##sketch_name##_sketch\
    }


SKETCH_CLASS(countmax, countmax_class);


MODULE_LICENSE("MIT");
static struct countmax_sketch* cm;
static int __init init(void) {
    cm = new_countmax_sketch(100, 4);
    struct flow_key key;
    elemtype value = 10;
    countmax_sketch_update(cm, &key, 10);
    printk("%d\n", countmax_sketch_query(cm, &key));
    return 0;
}

static void clean(void) { delete_countmax_sketch(cm); }

module_init(init);
module_exit(clean);