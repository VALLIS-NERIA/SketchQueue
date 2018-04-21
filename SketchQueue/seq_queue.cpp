#include "seq_queue.h"
#include "chrono"
#include "iostream"
using namespace std;
using namespace chrono;

int proc_1(big_queue* me) {
    high_resolution_clock::time_point begin, end;
    begin = high_resolution_clock::now();
    int c = 0;
    int j;
    while (1) {
        while (me->queue.empty()) {
            if (!sketch_queue::ready) {
                goto end;
            }
        }
        me->mutex.lock();
        auto item = me->queue.front();
        me->queue.pop();
        me->mutex.unlock();

        // launch
        for (int i = 0; i < me->sketch_count; ++i) {
            auto& s = me->sketches[i];
            if (item.tag & (1 << i)) {
                int t = 1;
                ++s.count;
                for (int i = 1; i < 100 * s.cycle; i++) {
                    t += rand();
                }
                if (t == 0) {
                    cout << "fatal";
                    return t;
                }
            }
        }
        ++c;
    }
end:
    for (int i = 0; i < me->sketch_count; ++i) {
        auto& s = me->sketches[i];
        //cout << "s" << i << " (cost " << s.cycle << ") processed " << s.count << endl;
    }
    end = high_resolution_clock::now();
    //cout << "in " << duration_cast<milliseconds>(end - begin).count() << ": " << c << " processed, " << me->dropped_count        << " dropped" << endl;
    cout << duration_cast<milliseconds>(end - begin).count() << "," << c << "," << me->dropped_count ;
    return 0;
}


void seq_queue::start() {
    new thread(proc_1, this);
}

seq_queue::seq_queue()
{
}


seq_queue::~seq_queue()
{
}
