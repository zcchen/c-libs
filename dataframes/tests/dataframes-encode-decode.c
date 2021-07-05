#include "testing_helper.h"
#include "dataframes.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

void print_buffer(volatile uint8_t* buf, const size_t s, const char* prompt)
{
    printf("%s: ", prompt);
    for (int i = 0; i < s; ++i) {
        printf("[0x%X] ", buf[i]);
    }
    printf("\n");
}

void printf_dataframes(struct dataframes_t *data)
{
    printf(">>> dataframes: \n");
    printf("head: [0x%X]\n", data->head.frame);
    printf("length: [0x%X]\n", data->length.value);
    print_buffer(data->data.frames, data->data.size, "data");
    printf("checksum: [0x%X]\n", data->checksum.value);
    printf("tail: [0x%X]\n", data->tail.frame);
}

int test_dataframes__init_encode_decode(struct dataframes_list_t *data,
                                        struct dataframes_list_t *recv)
{
    printf("--- try to encode data: %p ---\n", data);
    printf(">>> init the buffer ...\n");
    size_t buffer_size = 1024;
    size_t encoded_size = 0;
    size_t decoded_size = 0;
    volatile uint8_t buffer[buffer_size];
    for (int i = 0; i < buffer_size; ++i) {
        buffer[i] = '\0';
    }
    print_buffer(buffer, 10, ">>> buffer");

    struct dataframes_t recv_dataframes;
    struct dataframes_t send_dataframes;
    printf(">>> init the dataframes...\n");
    assert(0 == dataframes__init(&recv_dataframes, 0xAB, 0x66, DATAFRAMES_CHECKSUM_SUM));
    assert(0 == dataframes__init(&send_dataframes, 0xAB, 0x66, DATAFRAMES_CHECKSUM_SUM));

    printf(">>> Set the <data:%p> to send_dataframes ...\n", data);
    printf(">>> data list size: %ld\n", dataframes_list__getsize(data));
    printf("ret for setdata: %d\n", dataframes__setdata(&send_dataframes, data));
    assert(0 == dataframes__setdata(&send_dataframes, data));
    printf_dataframes(&send_dataframes);
    printf(">>> Encode the send_dataframes to send buffer ...\n");
    assert(0 == dataframes__encode_list(&send_dataframes, buffer, buffer_size, &encoded_size));
    printf(">>> Encoded buf size: %ld.\n", encoded_size);

    print_buffer(buffer, encoded_size, "buffer");

    printf(">>> Decode the sent buffer to recv_dataframes ...\n");
    printf("ret: %d\n",
           dataframes__decode_list(&recv_dataframes, buffer, buffer_size, &decoded_size));
    assert(0 == dataframes__decode_list(&recv_dataframes, buffer, buffer_size, &decoded_size));
    printf_dataframes(&recv_dataframes);
    printf(">>> Decoded size: %ld\n", decoded_size);

    assert(encoded_size != 0);
    assert(decoded_size != 0);
    assert(decoded_size == encoded_size);

    printf(">>> Checking the recv datalist ...\n");
    printf(">>> recv list size: %ld\n", dataframes_list__getsize(recv));
    printf("ret for getdata: %d\n", dataframes__getdata(&recv_dataframes, recv));
    assert(0 == dataframes__getdata(&recv_dataframes, recv));

    printf(">>> Checking the used size of recv datalist ...\n");
    if (!data) {    // NULL input
        assert(0 == dataframes_list__getsize(recv));
    }
    else {
        assert(dataframes_list__getsize(data) == dataframes_list__getsize(recv));
    }

    printf(">>> all assert test passed <<<\n");
    printf("------------------------------\n");
    return 0;
}

int main(void)
{
    struct dataframes_list_t recv_null = {.capacity = 0};
    dataframes_list__init(&recv_null, 16);

    TEST_RETURN(test_dataframes__init_encode_decode(NULL, &recv_null));
    TEST_RETURN(test_dataframes__init_encode_decode(NULL, &recv_null));

    struct dataframes_list_t datalist4set = {.capacity = 0};
    dataframes_list__init(&datalist4set, 16);
    struct dataframes_list_t datalist4recv = {.capacity = 0};
    dataframes_list__init(&datalist4recv, 16);

    uint8_t a = 0x11;
    int8_t b = -0x11;
    uint16_t c = 0x2233;
    int16_t d = -0x2233;
    uint32_t e = 0x44556677;
    int32_t f = -0x44556677;
    uint64_t g = 0x1234567890ABCDEF;
    int64_t h = -0x1234567890ABCDEF;
    float i = 0.1;
    double j = 0.2;
    long double k = 0.3;
    char* str = "hello";
    dataframes_list__setvalue(&datalist4set, 0, dataframes_UINT8_T, &a);
    dataframes_list__setvalue(&datalist4set, 1, dataframes_INT8_T, &b);
    dataframes_list__setvalue(&datalist4set, 2, dataframes_UINT16_T, &c);
    dataframes_list__setvalue(&datalist4set, 3, dataframes_INT16_T, &d);
    dataframes_list__setvalue(&datalist4set, 4, dataframes_UINT32_T, &e);
    dataframes_list__setvalue(&datalist4set, 5, dataframes_INT32_T, &f);
    dataframes_list__setvalue(&datalist4set, 6, dataframes_UINT64_T, &g);
    dataframes_list__setvalue(&datalist4set, 7, dataframes_INT64_T, &h);
    dataframes_list__setvalue(&datalist4set, 8, dataframes_FLOAT, &i);
    dataframes_list__setvalue(&datalist4set, 9, dataframes_DOUBLE, &j);
    dataframes_list__setvalue(&datalist4set, 10, dataframes_LONGDOUBLE, &k);
    dataframes_list__setvalue(&datalist4set, 11, dataframes_STRING, str);

    int zero = 0;
    dataframes_list__setvalue(&datalist4recv, 0, dataframes_UINT8_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 1, dataframes_INT8_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 2, dataframes_UINT16_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 3, dataframes_INT16_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 4, dataframes_UINT32_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 5, dataframes_INT32_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 6, dataframes_UINT64_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 7, dataframes_INT64_T, &zero);
    dataframes_list__setvalue(&datalist4recv, 8, dataframes_FLOAT, &zero);
    dataframes_list__setvalue(&datalist4recv, 9, dataframes_DOUBLE, &zero);
    dataframes_list__setvalue(&datalist4recv, 10, dataframes_LONGDOUBLE, &zero);
    dataframes_list__setvalue(&datalist4recv, 11, dataframes_STRING, "");

    TEST_RETURN(test_dataframes__init_encode_decode(&datalist4set, &datalist4recv));

    dataframes_list__init(&recv_null, 0);
    dataframes_list__init(&datalist4set, 0);
    dataframes_list__init(&datalist4recv, 0);
    return 0;
}
