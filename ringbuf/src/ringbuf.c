#include "ringbuf.h"

int ringbuf_init(volatile struct ringbuf_t *ringbuf)
{
    ringbuf->head = 0;
    ringbuf->tail = 0;
    ringbuf->error.byte = 0;
    for (int i = 0; i < sizeof(ringbuf->buf); ++i) {
        ringbuf->buf[i] = '\0';
    }
    return 0;
}

int ringbuf_used(const volatile struct ringbuf_t *ringbuf)
{
    if (ringbuf->head <= ringbuf->tail) {
        return (ringbuf->tail - ringbuf->head);
    }
    else {
        return (ringbuf->head + sizeof(ringbuf->buf) - ringbuf->tail);
    }
}
int ringbuf_remain(const volatile struct ringbuf_t *ringbuf)
{
    return sizeof(ringbuf->buf) - ringbuf_used(ringbuf);
}

int ringbuf_push(volatile struct ringbuf_t *ringbuf,
                 uint8_t *linebuf, size_t linebuf_maxlen, const bool force_flag)
{
    int i = 0;
    if (ringbuf->tail + 1 == ringbuf->head || \
        (ringbuf->tail == sizeof(ringbuf->buf) - 1 && ringbuf->head == 0)) {
        if (!force_flag) {
            return -1;
        }
    }
    while (i < linebuf_maxlen) {
        ringbuf->buf[ringbuf->tail] = *(linebuf + i);
        ringbuf->tail ++;
        i++;
        if (ringbuf->tail == ringbuf->head) {
            ringbuf->error.bits.overflow = 1;
        }
        if (ringbuf->tail >= sizeof(ringbuf->buf)) {
            ringbuf->tail = 0;
        }
        if (ringbuf->tail + 1 == ringbuf->head || \
            (ringbuf->tail == sizeof(ringbuf->buf) - 1 && ringbuf->head == 0)) {
            if (!force_flag) {
                break;
            }
        }
    }
    return i;
}

int ringbuf_pop(volatile struct ringbuf_t *ringbuf, uint8_t *linebuf, size_t linebuf_maxlen)
{
    int i = 0;
    if (ringbuf->tail == ringbuf->head) {
        return -1;
    }
    while (i < linebuf_maxlen) {
        *(linebuf + i) = ringbuf->buf[ringbuf->head];
        ringbuf->head ++;
        i++;
        if (ringbuf->head >= sizeof(ringbuf->buf)) {
            ringbuf->head = 0;
        }
        if (ringbuf->head == ringbuf->tail) {
            break;
        }
    }
    return i;
}

