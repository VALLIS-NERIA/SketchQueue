#include "big_queue.h"
#include <iostream>
using namespace std::literals;

std::atomic<int> big_queue::count = 0;

int watch_s(sketch* s) {
    while (!sketch_queue::ready);
    while (true) {
        while (!s->active) {}
        s->mutex.lock();
        ++big_queue::count;

        //++sketch_queue::count;

        //cout << ".";
        // do sketch update here
        //for (int i = 0; i < 1000; i++) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(s->cycle));
        //std::this_thread::sleep_for(10us);
        //}
        int t = 0;
        for (int i = 1; i < 1000 * s->cycle; i++) {
            t += s->next.tag%i;
        }
        if (t == 0) {
            std::cerr << "fatal";
            return t;
        }
    unlock:
        //delete s->next;
        s->active = false;
        s->mutex.unlock();
        //if (t == 0)return t;
    }
    return 0;
}

int proc(big_queue* me, entry ea[], int len) {
    for (int j = 0; j < len; ++j) {
        me->process_packet(ea[j]);
    }
    return 0;
}

void big_queue::process_packet(entry e) {
    // wait for all sketches
    for (int i = 0; i < sketch_count; ++i) {
        auto& s = sketches[i];
        s.mutex.lock();
    }
    // launch
    for (int i = 0; i < sketch_count; ++i) {
        auto& s = sketches[i];
        if (e.tag & (1 << i)) {
            s.next = e;
            s.active = true;
        }
        s.mutex.unlock();
    }
    //std::cout << e.tag % 0x100;

}

void big_queue::process_packets(entry ea[], int len) {
    new std::thread(proc, this, ea, len);
}

big_queue::big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        sketches[i].cycle = (i + 1) * 10;
        ths[i] = new std::thread(watch_s, &sketches[i]);
    }

}

big_queue::~big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        ths[i]->detach();
        delete ths[i];
    }
}
