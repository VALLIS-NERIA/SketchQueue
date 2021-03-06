#include "sketch_queue.h"
#include <iostream>
#include <atomic>
#include <synchapi.h>
using namespace std;
using namespace chrono;
//std::atomic<int> count = 0;
atomic<int> sketch_queue::count = 0;
atomic<int> sketch_queue::dropped = 0;
mutex sketch_queue::console_mutex;
high_resolution_clock::duration sketch_queue::time;

inline uint32_t flow_key_hash_old(struct flow_key* key) {
    int hashCode = (int)key->srcip;
    hashCode = (hashCode * 397) ^ (int)key->dstip;
    hashCode = (hashCode * 397) ^ (int)key->srcport;
    hashCode = (hashCode * 397) ^ (int)key->dstport;
    hashCode = (hashCode * 397) ^ (int)key->protocol;
    return (uint32_t)hashCode;
}

int watch(sketch_queue* q) {
    int c = 0;
    while (!sketch_queue::ready);
    q->begin = high_resolution_clock::now();

    bool flag = false;
    while (true) {
        while (q->queue.empty()) {
            if (!flag) {
                q->end = high_resolution_clock::now();
                flag = true;
            }
            if (!sketch_queue::ready) goto end;
        }
        flag = false;

        q->mutex.lock();
        auto item = q->queue.front();
        q->queue.pop();
        q->mutex.unlock();
        //this_thread::sleep_for(10us);
        //cout << ".";
        // do sketch update here
        int t = 0;
        for (int i = 1; i < 100 * q->cycle; i++) {
            t += rand();
        }
        if (t == 0) {
            cerr << "fatal";
            return t;
        }
        ++c;
    }
end:
    sketch_queue::console_mutex.lock();
    //cout << "sketch cost: " << q->cycle << ", in " << duration_cast<milliseconds>(q->end - q->begin).count()        << "ms: " << c << " processed, " << q->dropped_packet << " dropped" << endl;
    sketch_queue::console_mutex.unlock();
    sketch_queue::count+=c;
    sketch_queue::dropped += q->dropped_packet;
    if (sketch_queue::time < (q->end - q->begin)) {
        sketch_queue::time = (q->end - q->begin);
    }
    return 0;
}


sketch_queue::sketch_queue(int cyc) {
    this->tag = cyc;
    for (int i = 0; i < 16; ++i) {
        cycle += (tag | (1 << i) >> i) * (i + 1) * 10;
    }
}

sketch_queue::~sketch_queue() {
    th->detach();
    delete th;
}


void sketch_queue::start() {
    th = new thread(watch, this);
}

void sketch_queue::push(entry e) {
    if (e.tag & this->tag) {
        if (queue.size() >= capacity) {
            ++dropped_packet;
            return;
        }
        mutex.lock();
        queue.push(e);
        mutex.unlock();
    }
}


