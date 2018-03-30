#include "sketch_queue.h"
#include <iostream>
#include <atomic>
using namespace std;

//std::atomic<int> count = 0;
atomic<int> sketch_queue::count = 0;

inline uint32_t flow_key_hash_old(struct flow_key* key) {
    int hashCode = (int)key->srcip;
    hashCode = (hashCode * 397) ^ (int)key->dstip;
    hashCode = (hashCode * 397) ^ (int)key->srcport;
    hashCode = (hashCode * 397) ^ (int)key->dstport;
    hashCode = (hashCode * 397) ^ (int)key->protocol;
    return (uint32_t)hashCode;
}

int watch(decltype(sketch_queue::queue)& q, mutex& m) {
    while (!sketch_queue::ready);
    int t = 0;
    while (1) {
        while (q.empty()) {}
        m.lock();
        auto item = q.front();
        q.pop();
        m.unlock();
        ++sketch_queue::count;
        //cout << ".";
        // do sketch update here
        for (int i = 0; i < 1000; i++) {
            auto hash = flow_key_hash_old(&item.key);
            item.key.dstip = hash;
            t += hash;
        }
        if (t == 0)return t;
    }
}

sketch_queue::sketch_queue() { }

sketch_queue::~sketch_queue() {
    th->detach();
    delete th;
}


void sketch_queue::start() {
    th = new std::thread(watch, ref(queue), ref(mutex));
}

void sketch_queue::push(entry e) {
    mutex.lock();
    queue.push(e);
    mutex.unlock();
}
