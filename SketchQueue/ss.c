#include <stdio.h>
#include <stdlib.h>
void new_t_sketch(int x, int y) { printf("new\n"); }
void delete_t_sketch(char* c) { printf("delete\n"); }
struct sketch_class {
    void* (*new_sketch) (int, ...);

    //void(*update) (void* this, struct flow_key* key, elemtype value);

    //elemtype(*query)(void* this, struct flow_key* key);

    void(*delete_sketch) (void* me);

    int t;
};



#define SKETCH_CLASS(sketch_name, class_name) \
    struct sketch_class class_name = {  \
        .new_sketch = new_##sketch_name##_sketch,\
        /*.update = sketch_name##_sketch_update,\
                .query = sketch_name##_sketch_query,\*/\
        .delete_sketch = delete_##sketch_name##_sketch\
    }
int main() {
    //auto t = (void(*) (int, ...))test;
    SKETCH_CLASS(t, test_class);
    test_class.new_sketch(11, 1);
    test_class.delete_sketch(NULL);
    system("pause");
}