#ifndef WIRESHARK_MANUF_H
#define WIRESHARK_MANUF_H

typedef struct {
    int         mask;
    const char* mac;
    const char* vendor;
    const char* desc;
}manuf_t;

extern const manuf_t wireshark_macids[30380];

#endif

