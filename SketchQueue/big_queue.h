#ifndef BIG_QUEUE_H
#define BIG_QUEUE_H
#include <mutex>
#include <atomic>
#include <vector>
#include "key.h"
#include "sketch_queue.h"

struct sketch {
public:
    int cycle;
    std::atomic<bool> active = false;
    std::mutex mutex;
    std::atomic<int> count = 0;
    entry next;
    bool cont = true;
    //sketch(int cyc) : cycle(cyc) {}
};



class big_queue {
public:
    static std::atomic<int> count;
    static const int sketch_count = 4;

    sketch sketches[sketch_count];
    std::thread* ths[sketch_count];
    std::queue<entry> queue;
    std::mutex mutex;
    int capacity=4000;
    std::atomic<int> dropped_count = 0;

    void start();
    void process_packet(entry e);
    void process_packets(entry ea[], int len);
    big_queue();
    ~big_queue();
    void push(const entry& entry);
};


#endif // BIG_QUEUE_H
