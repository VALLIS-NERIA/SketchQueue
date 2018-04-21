#ifndef SKETCH_QUEUE_H
#define SKETCH_QUEUE_H
#include<queue>
#include "key.h"
#include <mutex>
#include <atomic>




class sketch_queue {
public:
    static std::mutex console_mutex;
    static std::atomic<int> count;
    static std::atomic<bool> ready;
    static std::atomic<int> dropped;
    static std::chrono::high_resolution_clock::duration time;

    std::mutex mutex;
    std::queue<entry> queue;
    std::thread* th;
    //int cycle = 1;
    uint16_t tag;
    int cycle;
    int capacity;
    int dropped_packet=0;
    std::chrono::high_resolution_clock::time_point begin, end;
    sketch_queue(int tag);
    sketch_queue(){}
    ~sketch_queue();
    void start();
    void push(entry e);
    std::string des();
};
#endif // SKETCH_QUEUE_H
