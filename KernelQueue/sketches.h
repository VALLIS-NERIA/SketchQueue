#ifndef SKETCHES_H
#define SKETCHES_H
#include "countmax.h"
#include "countsketch.h"
#include "fss.h"

enum sketch_types {
    sketch_countmax = 1,
    sketch_countsketch = 2,
    sketch_fss = 4
};

struct sketch_base {
    void* instance;

    void* (*create)(int, ...);

    void (*update)(void* this, struct flow_key* key, elemtype value);

    elemtype (*query)(void* this, struct flow_key* key);

    void (*delete)(void* this);
};


#define SKETCH_VTABLE_DEC(var_name, sketch_name) \
    struct sketch_base ##var_name## = {                             \
        .create = (void* (*)(int, ...)) new_##sketch_name##_sketch,               \
        .update = (void (*)(void*, struct flow_key*, elemtype)) sketch_name##_sketch_update,                \
        .query = (elemtype (*)(void*, struct flow_key*)) sketch_name##_sketch_query,                  \
        .delete = (void (*)(void*)) delete_##sketch_name##_sketch          \
    }

#define SKETCH_VTABLE_SET(var_name, sketch_name) do {  \
        var_name.instance = NULL; \
        var_name.create = (void* (*)(int, ...)) new_##sketch_name##_sketch;               \
        var_name.update = (void (*)(void*, struct flow_key*, elemtype)) sketch_name##_sketch_update;                \
        var_name.query = (elemtype (*)(void*, struct flow_key*)) sketch_name##_sketch_query;                  \
        var_name.delete = (void (*)(void*)) delete_##sketch_name##_sketch;          \
    } while(0)

static inline struct sketch_base get_sketch_class(enum sketch_types type) {
    struct sketch_base ret;
    switch (type) {
    case sketch_countmax:
        SKETCH_VTABLE_SET(ret, countmax);
        break;
    case sketch_countsketch:
        SKETCH_VTABLE_SET(ret, countsketch);
        break;
    case sketch_fss:
        SKETCH_VTABLE_SET(ret, fss);
        break;
    default:
        ret.instance = NULL;
        ret.create = NULL;
        ret.update = NULL;
        ret.query = NULL;
        ret.delete = NULL;
    }
    return ret;
}

#undef SKETCH_VTABLE_SET
#undef SKETCH_VTABLE_DEC
#endif // SKETCHES_H
