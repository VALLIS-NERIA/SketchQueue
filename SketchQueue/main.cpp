#include "sketch_queue.h"
#include "big_queue.h"
#include <Windows.h>
#include <iostream>
#include "seq_queue.h"

using namespace std;
using namespace chrono;

inline int rand_byte() {
    return rand() & 0xff;
}

inline uint32_t rand_uint32() {
    uint32_t i = 0;
    i += rand_byte();
    i <<= 8;
    i += rand_byte();
    i <<= 8;
    i += rand_byte();
    i <<= 8;
    i += rand_byte();
    return i;
}

inline uint32_t rand_uint16() {
    uint16_t i = 0;
    i += rand_byte();
    i <<= 1;
    i += rand_byte();
    return i;
}


int sketch_num = big_queue::sketch_count;

int* queues;
entry* entries;

void flow_gen(size_t size) {
    entries = new entry[size];
    queues = new int[size];
    for (unsigned int i = 0; i < size; ++i) {
        entries[i].key.srcip = rand_uint32();
        entries[i].key.dstip = rand_uint32();
        entries[i].key.srcport = rand_uint16();
        entries[i].key.dstport = rand_uint16();
        entries[i].key.protocol = rand_uint16();
        entries[i].size = rand_uint16();
        entries[i].tag = rand_uint16();
        //queues[i] = rand_byte() % sketch_num;
    }
    //return array;
}

std::atomic<bool> sketch_queue::ready = false;
int packet_speed;

int big() {
    //std::cin >> sketch_num;
    big_queue* bq = new big_queue();

    //bq.process_packets(entries, 100000);

    //sketch_queue::ready = true;
    sketch_queue::ready = true;
    int i = 0;
    bq->start();
    while (i < 100000) {
        auto pkt = packet_speed / 100;
        while (pkt-- > 0 && i < 100000) {
            //for (int j = 0; j < sketch_num; ++j) {
            bq->push(entries[i]);
            //}
            ++i;
        }
        this_thread::sleep_for(milliseconds(10));
    }
    sketch_queue::ready = false;
    //while (1);
    return 0;
}


int multi() {
    //std::cin >> sketch_num;

    //flow_gen(100000);
    auto sks = new sketch_queue[sketch_num];
    for (int i = 0; i < sketch_num; i++) {
        sks[i].tag = (1 << (i));
        sks[i].capacity = 1000;
        //sks[i].cycle = 10 * (i + 1);
    }
    sks[0].cycle = 1;
    sks[1].cycle = 2;
    sks[2].cycle = 10;
    sks[3].cycle = 50;
    int i = 0;
    int n = 10;
    for (int i = 0; i < sketch_num; i++) {
        sks[i].start();
    }
    sketch_queue::ready = true;

    while (i < 100000) {
        auto pkt = packet_speed / 100;
        while (pkt-- > 0 && i < 100000) {
            for (int j = 0; j < sketch_num; ++j) {
                sks[j].push(entries[i]);
            }
            ++i;
        }
        this_thread::sleep_for(milliseconds(10));
    }
    sketch_queue::ready = false;

    //Sleep(500);
    //std::cout << sketch_queue::count;
    //while (1);
    //system("pause");
    return 0;
}

int seq() {
    auto sq = new seq_queue();
    int i = 0;
    sketch_queue::ready = true;
    sq->start();
    while (i < 100000) {
        auto pkt = packet_speed / 100;
        while (pkt-- > 0 && i < 100000) {
            sq->push(entries[i]);
            ++i;
        }
        this_thread::sleep_for(milliseconds(10));
    }
    sketch_queue::ready = false;

    return 0;
}

int main() {
    flow_gen(100000);
    while (1) {
        sketch_queue::count = 0;
        sketch_queue::dropped = 0;
        cin >> packet_speed;
        cout << "seq:" << endl;
        seq();
        this_thread::sleep_for(5s);
        cout << "1 queue: " << endl;
        big();
        //getc(stdin);
        this_thread::sleep_for(5s);

        cout << "4 queue: " << endl;
        //multi();
        this_thread::sleep_for(1s);
        cout << sketch_queue::count << "/" << sketch_queue::dropped << endl;
    }
}

int main_() {
    flow_gen(100000);
    auto a = new entry[100][10000];

    LARGE_INTEGER begin, end, frequency;
    //std::cout << "begin" << std::endl;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&begin);
    for (int x = 0; x < 10; x++) {
        int k = 0;
        for (int j = 0; j < 1000; j++) {
            for (int i = 0; i < 100; i++) {
                a[i][j] = entries[k];
                k = (++k);
            }
        }
    }
    QueryPerformanceCounter(&end);
    int cycle = (int)((double)(end.QuadPart - begin.QuadPart) * 1000 / 1000000);
    std::cout << cycle;
    system("pause");
    return 0;
}
