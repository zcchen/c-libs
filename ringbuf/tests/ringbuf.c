#include "testing_helper.h"
#include "ringbuf.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define MY_BUF_SIZE 256

uint8_t *linebuf_init(size_t size)
{
    uint8_t *ret = malloc(size * sizeof(uint8_t));
    for (int i = 0; i < size; ++i) {
        *(ret + i) = '\0';
    }
    return ret;
}

void ringbuf_print(const struct ringbuf_t *rbuf, const char *s)
{
    printf("--> ringbuf: %s.\n", s);
    printf("------> head: %d\n", rbuf->head);
    printf("------> tail: %d\n", rbuf->tail);
    for (int i = 0; i < sizeof(rbuf->buf); ++i) {
        int index = rbuf->head + i;
        if (index < sizeof(rbuf->buf)) {
            printf("rbuf[%d]: 0x%x\n", index, rbuf->buf[index]);
        }
        else {
            printf("rbuf[%d]: 0x%x\n",
                    (int)(index - sizeof(rbuf->buf)), rbuf->buf[index - sizeof(rbuf->buf)]);
        }
    }
    printf("----------------\n");
}

int test_ringbuf_push_check_length(const char *buf)
{
    printf("\n");
    struct ringbuf_t rbuf;
    int ret = 0;
    ringbuf_init(&rbuf);
    ret = ringbuf_push(&rbuf, (uint8_t*)buf, strlen(buf), false);
    ringbuf_print(&rbuf, "after pushed");
    assert(ringbuf_used(&rbuf) == ret);
    if (ret == strlen(buf)) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_ringbuf_pop_check_length_once_for_all(const char *buf)
{
    printf("\n");
    uint8_t *pop_buf = linebuf_init(MY_BUF_SIZE);
    struct ringbuf_t rbuf;
    ringbuf_init(&rbuf);
    ringbuf_push(&rbuf, (uint8_t*)buf, strlen(buf), false);
    ringbuf_print(&rbuf, "after pushed");
    int pop_ret = ringbuf_pop(&rbuf, (uint8_t*)pop_buf, MY_BUF_SIZE);
    ringbuf_print(&rbuf, "after pop once for all");

    printf("pop_buf: \n");
    int i;
    for (i = 0; i < strlen(buf); ++i) {
        printf("pop_buf[%d]: 0x%x\n", i, pop_buf[i]);
    }
    assert(i == strlen(buf));
    printf("-------------\n");

    if (pop_ret == strlen(buf)) {
        return 0;
    }
    else {
        return pop_ret;
    }
}

int test_ringbuf_pop_check_length_one_by_one(const char *buf)
{
    printf("\n");
    uint8_t *pop_buf = linebuf_init(MY_BUF_SIZE);

    struct ringbuf_t rbuf;
    ringbuf_init(&rbuf);
    ringbuf_push(&rbuf, (uint8_t*)buf, strlen(buf), false);
    for (int i = 0; i < MY_BUF_SIZE; ++i) {
        /*printf("i: <%d>\n", i);*/
        if (ringbuf_pop(&rbuf, (uint8_t*)(pop_buf + i), 1) <= 0) {
            break;
        }
        /*ringbuf_print(&rbuf, "after pop in i");*/
    }
    ringbuf_print(&rbuf, "after pop one by one");

    printf("pop_buf: \n");
    for (int i = 0; i < strlen(buf); ++i) {
        printf("pop_buf[%d]: 0x%x\n", i, pop_buf[i]);
    }
    printf("-------------\n");

    assert(rbuf.head == rbuf.tail);
    if (strlen((char*)pop_buf) == strlen(buf)) {
        return 0;
    }
    else {
        return strlen((char*)pop_buf);
    }
}


int main(void)
{
    TEST_RETURN(test_ringbuf_push_check_length("123456789"));
    TEST_RETURN(test_ringbuf_pop_check_length_once_for_all("123456789"));
    TEST_RETURN(test_ringbuf_pop_check_length_one_by_one("123456789"));
    return 0;
}
