#include "big_queue.h"
#include <iostream>
using namespace std;
using namespace chrono;
atomic<int> big_queue::count = 0;

int watch_s(sketch* s) {
    while (!sketch_queue::ready);
    while (true) {
        while (!s->active) {
            if (!s->cont) {
                return 0;
            }
        }
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

int proc(big_queue* me) {
    high_resolution_clock::time_point begin, end;
    begin = high_resolution_clock::now();
    int c = 0;
    int j;
    while (1) {
        while (me->queue.empty()) {
            if (!sketch_queue::ready) {
                for (int i = 0; i < me->sketch_count; ++i) {
                    me->sketches[i].cont = false;
                }
                goto end;
            }
        }
        me->mutex.lock();
        auto item = me->queue.front();
        me->queue.pop();
        me->mutex.unlock();

        // wait
        bool busy;
        do {
            busy = false;
            for (int i = 0; i < me->sketch_count; ++i) {
                auto& s = me->sketches[i];
                busy |= s.active;
            }
        }
        while (busy);

        // launch
        for (int i = 0; i < me->sketch_count; ++i) {
            auto& s = me->sketches[i];
            if (item.tag & (1 << i)) {
                s.next = item;
                s.active = true;
            }
        }
        ++c;
    }
end:
    for (int i = 0; i < me->sketch_count; ++i) {
        auto& s = me->sketches[i];
        cout << "s" << i << " (cost "<<s.cycle<<") processed " << s.count << endl;
    }
    end = high_resolution_clock::now();
    cout << "in " << duration_cast<milliseconds>(end - begin).count() << ": " << c << " processed, " << me->dropped_count
        << " dropped" << endl;
    return 0;
}

void big_queue::start() {
    new thread(proc, this);
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
    //new thread(proc, this, ea, len);
}

big_queue::big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        ths[i] = new thread(watch_s, &sketches[i]);
    }
    sketches[0].cycle = 1;
    sketches[1].cycle = 2;
    sketches[2].cycle = 10;
    sketches[3].cycle = 50;
}

big_queue::~big_queue() {
    for (int i = 0; i < sketch_count; ++i) {
        ths[i]->detach();
        delete ths[i];
    }
}

void big_queue::push(const entry& entry) {
    if (queue.size() >= capacity) {
        ++dropped_count;
        return;
    }
    else {
        mutex.lock();
        queue.push(entry);
        mutex.unlock();
    }
}
