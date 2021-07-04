#ifndef ZCCHEN_CLIBS__DATAFRAMES_H
#define ZCCHEN_CLIBS__DATAFRAMES_H

#include <stdint.h>
#include <stddef.h>

#include "ringbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

struct dataframes_var_t {
    enum dataframes_type_t {
        dataframes_LIST_T = 0,
        dataframes_STRING,
        dataframes_UINT8_T,
        dataframes_INT8_T,
        dataframes_UINT16_T,
        dataframes_INT16_T,
        dataframes_UINT32_T,
        dataframes_INT32_T,
        dataframes_UINT64_T,
        dataframes_INT64_T,
        dataframes_FLOAT,
        dataframes_DOUBLE,
        dataframes_LONGDOUBLE,
    } type;
    union dataframes_value_t {
        struct dataframes_list_t* list;
        char* strptr;       // remember to free it before to call the destroy function().
        uint8_t uint8;
        int8_t int8;
        uint16_t uint16;
        int16_t int16;
        uint32_t uint32;
        int32_t int32;
        uint64_t uint64;
        int64_t int64;
        float float16;
        double double32;
        long double longdouble64;
    } value;
};

struct dataframes_list_t {
    size_t capacity;
    struct dataframes_var_t *list;
};

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
                uint8_t include_checksum:1; // length value includes the checksum frames
                uint8_t include_tail:1;     // length value includes the tail frames
            } bits;
        } rules;
    } length;   // length value MUST includes the data frames
    struct {
        uint8_t *frames;
        size_t capacity;
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
            } bits;
        } rules;
        uint16_t (* calc)(uint16_t last, volatile uint8_t *raw_data, const size_t size);
    } checksum;     // checksum value MUST includes the data frames
    struct {
        uint8_t frame;
        union {
            uint8_t byte;
            struct {
            } bits;
        } rules;
    } tail;
};

enum dataframes_err_code {
    DATAFRAMES__OK = 0,
    DATAFRAMES__VAR_TYPE_UNKNOWN,
    DATAFRAMES__INIT_LIST_FAILED,
    DATAFRAMES__OVER_LIST_CAPACITY,
    DATAFRAMES__BUFFER_WITHOUT_VALID_MSG,
    DATAFRAMES__MSG_WITHOUT_VALID_CHECKSUM,
    DATAFRAMES__FRAME_STRUCT_NOT_INIT,
    DATAFRAMES__FRAME_STRUCT_NOT_READY,
    DATAFRAMES__FRAME_STRUCT_IS_LOCKED,
    DATAFRAMES__NOT_ENOUGH_FRAMES_CAPACITY,
    DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY,
};

enum dataframes_checksum_t {
    DATAFRAMES_CHECKSUM_NONE = 0,
    DATAFRAMES_CHECKSUM_SUM,
};

// memory setup
struct dataframes_var_t* dataframes_var__create(void);
int dataframes_var__init(struct dataframes_var_t* frame);
void dataframes_var__destroy(struct dataframes_var_t* frame);

// set the dataframes_var
int dataframes_var__set(struct dataframes_var_t* frame,
                        const enum dataframes_type_t type, const void* value);

struct dataframes_list_t* dataframes_list__create(size_t capacity);
int dataframes_list__init(struct dataframes_list_t *l, size_t capacity);
void dataframes_list__destroy(struct dataframes_list_t *l);

// get the dataframes use size
size_t dataframes_list__getsize(const struct dataframes_list_t *l);
// set the dataframes value
int dataframes_list__setvalue(struct dataframes_list_t *l, const size_t index,
                              const enum dataframes_type_t type, const void* value);

int dataframes_list__conv_to_buffer(const struct dataframes_list_t *l,
                                    uint8_t *buffer, const size_t maxlen, size_t* conv_len);
int dataframes_list__conv_from_buffer(const struct dataframes_list_t *l,
                                      uint8_t *buffer, const size_t maxlen, size_t* conv_len);

struct dataframes_t* dataframes__create(const size_t capacity,
                     const uint8_t head, const uint8_t tail,
                     const enum dataframes_checksum_t checksum);
int dataframes__init(struct dataframes_t *frames, const size_t capacity,
                     const uint8_t head, const uint8_t tail,
                     const enum dataframes_checksum_t checksum);
void dataframes__destroy(struct dataframes_t* frames);

int dataframes__decode_list(struct dataframes_t *frames, volatile uint8_t* buffer,
                            const size_t buffer_len, size_t* decoded_len);
int dataframes__decode_ringbuf(struct dataframes_t *frames, volatile struct ringbuf_t *ringbuf);

int dataframes__encode_list(struct dataframes_t *frames, volatile uint8_t* buffer,
                            const size_t buffer_len, size_t* encoded_len);
int dataframes__encode_ringbuf(struct dataframes_t *frames, volatile struct ringbuf_t *ringbuf);

int dataframes__setdata(struct dataframes_t *frames, const struct dataframes_list_t* data);
int dataframes__getdata(struct dataframes_t *frames, const struct dataframes_list_t* data);


#ifdef __cplusplus
extern }
#endif


#endif /* ifndef ZCCHEN_CLIBS__DATAFRAMES_H */
