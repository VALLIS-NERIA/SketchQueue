#include "big_queue.h"
#include <iostream>
using namespace std;
using namespace chrono;
atomic<int> big_queue::count = 0;

int watch_s(sketch* s) {
    while (!sketch_queue::ready);
    while (true) {
        while (!s->active) {}
        s->mutex.lock();
        int t = 1;
        ++s->count;
        for (int i = 1; i < 100 * s->cycle; i++) {
            t += rand();
        }
        if (t == 0) {
            cout << "fatal";
            return t;
        }
    unlock:
        //delete s->next;
        s->active = false;
        s->mutex.unlock();
    }
    return 0;
}

int proc(big_queue* me, entry ea[], int len) {
    high_resolution_clock::time_point begin, end;
    begin = high_resolution_clock::now();
    int c = 0;
    int j;
    for (j = 0; j < len; ++j) {
        bool busy;
        do {
            busy = false;
            for (int i = 0; i < me->sketch_count; ++i) {
                auto& s = me->sketches[i];
                busy |= s.active;
                //s.mutex.lock();
            }
        } while (busy);
        // launch
        for (int i = 0; i < me->sketch_count; ++i) {
            auto& s = me->sketches[i];
            if (ea[j].tag & (1 << i)) {
                s.next = ea[j];
                ++c;
                s.active = true;
            }
            //s.mutex.unlock();
        }
    }
    for (int i = 0; i < me->sketch_count; ++i) {
        auto& s = me->sketches[i];
        cout << s.count << endl;
    }
    end = high_resolution_clock::now();
    cout << duration_cast<milliseconds>(end - begin).count() << ": " << big_queue::count << "," << c << endl;
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
    new thread(proc, this, ea, len);
}

big_queue::big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        sketches[i].cycle = (i + 1) * 10;
        ths[i] = new thread(watch_s, &sketches[i]);
    }

}

big_queue::~big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        ths[i]->detach();
        delete ths[i];
    }
}
