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
    //sketch(int cyc) : cycle(cyc) {}
};



class big_queue {
public:
    static std::atomic<int> count;
    static const int sketch_count = 6;
    sketch sketches[sketch_count];
    std::thread* ths[sketch_count];

    void process_packet(entry e);
    void process_packets(entry ea[], int len);
    big_queue();
    ~big_queue();
};


#endif // BIG_QUEUE_H
