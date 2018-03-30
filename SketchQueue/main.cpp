#include "sketch_queue.h"
#include <Windows.h>
#include <iostream>

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


int sketch_num = 5;

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
        queues[i] = rand_byte() % sketch_num;
    }
    //return array;
}

std::atomic<bool> sketch_queue::ready = false;

int main__() {
    std::cin >> sketch_num;
    flow_gen(100000);
    auto sks = new sketch_queue[sketch_num];
    for (int i = 0; i < sketch_num; i++) {
        sks[i].start();
    }
    int i = 0;
    int n = 10;
    while (i < 1000000) {
        sks[queues[i % 100000]].push(entries[i % 100000]);
        ++i;
    }
    sketch_queue::ready = true;
    Sleep(500);
    std::cout << sketch_queue::count;
    system("pause");
    return 0;
}


void test(int x, int y) { std::cout << x + y; }

int main() {
    auto t = (void(*) (int, ...))test;
    t(1, 2);
    system("pause");
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
