#ifndef ZCCHEN_CLIBS__DATAFRAMES_H
#define ZCCHEN_CLIBS__DATAFRAMES_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dataframes_t {
    union {     // the status bits for this data
        uint8_t byte;
        struct {
            uint8_t lock:1;     // the data struct is being modify
            uint8_t init:1;     // the data struct is able to fill data
            uint8_t ready:1;    // the data struct is ready to use (send out / get data)
        } bits;
    } status;
    struct {
        uint8_t frame;  // the head frame should be the const frame
        union {
            uint8_t byte;
            struct {
            } bits;
        } rules;
    } head;
    struct {
        uint8_t value;
        union {
            uint8_t byte;
            struct {
                uint8_t include_head:1;     // length value includes the head frame
                uint8_t include_length:1;   // length value includes the length frames
                uint8_t include_data:1;     // length value includes the data frames
                uint8_t include_checksum:1; // length value includes the checksum frames
                uint8_t include_tail:1;     // length value includes the tail frames
            } bits;
        } rules;
    } length;
    struct {
        uint8_t *raw;
        size_t size;
        union {
            uint8_t byte;
            struct {
            } bits;
        } rules;
    } data;
    struct {
        uint16_t value;
        union {
            uint8_t byte;
            struct {
                uint8_t include_head:1;     // length value includes the head frame
                uint8_t include_length:1;   // length value includes the length frames
                uint8_t include_data:1;     // length value includes the data frames
            } bits;
        } rules;
        uint16_t (* calc)(uint8_t *raw_data, size_t size);
    } checksum;
    struct {
        uint8_t frame;
        union {
            uint8_t byte;
            struct {
            } bits;
        } rules;
    } tail;
};

struct dataframes_var_t {
    enum datafrases_type_t {
        dataframes_UINT8_T = 0,
        dataframes_INT8_T,
        dataframes_UINT16_T,
        dataframes_INT16_T,
        dataframes_UINT32_T,
        dataframes_INT32_T,
        dataframes_UINT64_T,
        dataframes_INT64_T,
        dataframes_FLOAT,
        dataframes_DOUBLE,
    } type;
    union dataframes_value_t {
        uint8_t uint8;
        int8_t int8;
        uint16_t uint16;
        int16_t int16;
        uint32_t uint32;
        int32_t int32;
        float float16;
        double double32;
    } value;
};

int dataframes_init(struct dataframes_t *frame);

int dataframes_decode(struct dataframes_t *frame, uint8_t* buffer,
                      const size_t buffer_len, size_t* decoded_len);

int dataframes_encode(struct dataframes_t *frame, uint8_t* buffer,
                      const size_t buffer_len, size_t* encoded_len);

int dataframes_set(struct dataframes_t *frame, void* data_struct);
int dataframes_get(struct dataframes_t *frame, void* data_struct);


#ifdef __cplusplus
extern }
#endif


#endif /* ifndef ZCCHEN_CLIBS__DATAFRAMES_H */
