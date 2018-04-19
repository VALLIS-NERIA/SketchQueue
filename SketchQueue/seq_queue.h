#ifndef SEQ_QUEUE_H
#define SEQ_QUEUE_H
#include "big_queue.h"

class seq_queue : public big_queue {
public:
    void start();

    seq_queue();
    ~seq_queue();
};

#endif // SEQ_QUEUE_H
