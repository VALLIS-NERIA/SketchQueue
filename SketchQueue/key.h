#ifndef KEY_H
#define KEY_H
#include <cstdint>

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
    uint16_t tag;
};

#endif // KEY_H
