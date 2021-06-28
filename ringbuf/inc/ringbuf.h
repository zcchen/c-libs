#ifndef CTOOLINGS_RINGBUGH_H
#define CTOOLINGS_RINGBUGH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUF_SIZE 64

typedef unsigned char uint8_t;

struct ringbuf_t {
    uint8_t buf[BUF_SIZE];
    uint8_t head;
    uint8_t tail;
    union {
        uint8_t all;
        struct rx_err_t {
            uint8_t overflow:1;
            uint8_t rsvd:7;
        } bit;
    } error;
};

int ringbuf_init(volatile struct ringbuf_t *ringbuf);
int ringbuf_used(const volatile struct ringbuf_t *ringbuf);
int ringbuf_remain(const volatile struct ringbuf_t *ringbuf);
int ringbuf_push(volatile struct ringbuf_t *ringbuf,
                 uint8_t *linebuf, size_t linebuf_maxlen, const bool force_flag);
int ringbuf_pop(volatile struct ringbuf_t *ringbuf, uint8_t *linebuf, size_t linebuf_maxlen);

#ifdef __cplusplus
extern }
#endif

#endif /* ifndef CTOOLINGS_RINGBUGH_H */
