#ifndef SKETCH_QUEUE_H
#define SKETCH_QUEUE_H
#include<queue>
#include <stddef.h>
#include <mutex>
#include <atomic>


struct flow_key {
    uint32_t srcip;
    uint32_t dstip;
    uint16_t srcport;
    uint16_t dstport;
    uint16_t protocol;
};

struct entry {
    flow_key key;
    uint16_t size;
};

class sketch_queue {
public:
    static std::atomic<int> count;
    static std::atomic<bool> ready;
    sketch_queue();
    ~sketch_queue();
    std::mutex mutex;
    std::queue<entry> queue;
    std::thread* th;
    void start();
    void push(entry e);
};
#endif // SKETCH_QUEUE_H
