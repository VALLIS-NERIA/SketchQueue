#ifndef SKETCH_QUEUE_H
#define SKETCH_QUEUE_H
#include<queue>
#include "key.h"
#include <mutex>
#include <atomic>




class sketch_queue {
public:
    static std::atomic<int> count;
    static std::atomic<bool> ready;
    std::mutex mutex;
    std::queue<entry> queue;
    std::thread* th;
    //int cycle = 1;
    uint16_t tag;
    int cycle;
    sketch_queue(int tag);
    sketch_queue(){}
    ~sketch_queue();
    void start();
    void push(entry e);
};
#endif // SKETCH_QUEUE_H
