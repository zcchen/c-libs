#include "dataframes.h"

#include <stdlib.h>

// private checksum funcions
static uint16_t _checksum_sum(uint8_t *raw_data, size_t size)
{
    uint16_t ret = 0;
    for (int i = 0; i < size; ++i) {
        ret += *(raw_data + i);
    }
    return ret;
}

struct dataframes_var_t* dataframes_var__create(void)
{
    struct dataframes_var_t* ret = malloc(sizeof(struct dataframes_var_t));
    if (!dataframes_var__init(ret)) {
        free(ret);
        return NULL;
    }
    return ret;
}

int dataframes_var__init(struct dataframes_var_t* frame)
{   // Set it to NULL LIST, a.k.a. empty data
    frame->type = dataframes_LIST_T;
    frame->value.list = NULL;
    return DATAFRAMES__OK;
}

void dataframes_var__destroy(struct dataframes_var_t* frame)
{
    if (frame->type == dataframes_LIST_T) {
        struct dataframes_list_t* l = frame->value.list;
        if (l) {
            dataframes_list__destroy(l);    // destroy the list child.
        }
    }
    free(frame);
}

int dataframes_var__set(struct dataframes_var_t* frame,
                         enum dataframes_type_t type, void* value)
{
    frame->type = type;
    switch (type) {
        case dataframes_LIST_T:
            frame->value.list = value;
            break;
        case dataframes_STRING:
            frame->value.strptr = value;
            break;
        case dataframes_UINT8_T:
            frame->value.uint8 = *(uint8_t*)(value);
            break;
        case dataframes_INT8_T:
            frame->value.int8 = *(int8_t*)(value);
            break;
        case dataframes_UINT16_T:
            frame->value.uint16 = *(uint16_t*)(value);
            break;
        case dataframes_INT16_T:
            frame->value.int16 = *(int16_t*)(value);
            break;
        case dataframes_UINT32_T:
            frame->value.uint32 = *(uint32_t*)(value);
            break;
        case dataframes_INT32_T:
            frame->value.int32 = *(int32_t*)(value);
            break;
        case dataframes_UINT64_T:
            frame->value.uint64 = *(uint64_t*)(value);
            break;
        case dataframes_INT64_T:
            frame->value.int64 = *(int64_t*)(value);
            break;
        case dataframes_FLOAT:
            frame->value.float16 = *(float*)(value);
            break;
        case dataframes_DOUBLE:
            frame->value.double32 = *(double*)(value);
            break;
        case dataframes_LONGDOUBLE:
            frame->value.longdouble64 = *(long double*)(value);
            break;
        default:
            dataframes_var__init(frame);
            return DATAFRAMES__OK;
    }
    return DATAFRAMES__VAR_TYPE_UNKNOWN;
}


struct dataframes_list_t* dataframes_list__create(size_t capacity)
{
    struct dataframes_list_t* ret = malloc(sizeof(struct dataframes_list_t));
    if (dataframes_list__init(ret, capacity)) {
        free(ret);
        return NULL;
    }
    return ret;
}

int dataframes_list__init(struct dataframes_list_t *l, size_t capacity)
{
    struct dataframes_var_t* var_list = malloc(sizeof(struct dataframes_var_t) * capacity);
    for (int i = 0; i < capacity; ++i) {
        if (dataframes_var__init(&var_list[i])) {
            free(var_list);
            return DATAFRAMES__INIT_LIST_FAILED;
        };
    }
    l->capacity = capacity;
    l->list = var_list;
    return DATAFRAMES__OK;
}

void dataframes_list__destroy(struct dataframes_list_t *l)
{
    for (int i = 0; i < l->capacity; ++i) {
        dataframes_var__destroy(&l->list[i]);
    }
    free(l->list);
    free(l);
}

size_t dataframes_list__getsize(struct dataframes_list_t *l)
{
    for (int i = 0; i < l->capacity; ++i) {
        if (l->list[i].type == dataframes_LIST_T && l->list[i].value.list == NULL) {
            return i;
        }
    }
    return l->capacity;
}

int dataframes_list__setvalue(struct dataframes_list_t *l, size_t index,
                              enum dataframes_type_t type, void* value)
{
    if (index >= l->capacity) {
        return DATAFRAMES__OVER_LIST_CAPACITY;
    }
    struct dataframes_var_t* frame = &l->list[index];
    return dataframes_var__set(frame, type, value);
}



struct dataframes_t* dataframes__create(const size_t capacity,
                     const uint8_t head, const uint8_t tail,
                     const enum dataframes_checksum_t checksum)
{
    struct dataframes_t* ret = malloc(sizeof(struct dataframes_t));
    if (dataframes__init(ret, capacity, head, tail, checksum)) {
        free(ret);
        return NULL;
    }
    return ret;
}

void dataframes__destroy(struct dataframes_t* frames)
{
    free(frames->data.frames);
    free(frames);
}

int dataframes__init(struct dataframes_t *frames, const size_t capacity,
                     const uint8_t head, const uint8_t tail,
                     const enum dataframes_checksum_t checksum)
{
    // init the status
    frames->status.byte = 0;
    frames->status.bits.lock = 1;

    // init the head frame
    frames->head.frame = head;
    frames->head.rules.byte = 0x00;
    // init the length frame
    frames->length.value = 0;
    frames->length.rules.byte = 0x00;       // set the rules
    frames->length.rules.bits.include_head = 1;
    frames->length.rules.bits.include_length = 1;
    frames->length.rules.bits.include_data = 1;
    frames->length.rules.bits.include_checksum = 1;
    frames->length.rules.bits.include_tail = 1;
    // init the data frames
    if (!frames->data.frames) {
        frames->data.frames = malloc(capacity * sizeof(uint8_t));
        frames->data.capacity = capacity;
    }
    for (int i = 0; i < frames->data.capacity; ++i) {
        *(frames->data.frames + i) = 0x00;
    }
    frames->data.rules.byte = 0;
    // init the checksum frames
    frames->checksum.value = 0;
    frames->checksum.rules.byte = 0x00;     // set the rules.
    frames->checksum.rules.bits.include_head = 1;
    frames->checksum.rules.bits.include_length = 1;
    frames->checksum.rules.bits.include_data = 1;
    switch (checksum) {
        case DATAFRAMES_CHECKSUM_SUM:
            frames->checksum.calc = _checksum_sum;
            break;
        default:
            frames->checksum.calc = NULL;
    }
    // init the tail frame
    frames->tail.frame = tail;
    frames->tail.rules.byte = 0x00;

    // init done
    frames->status.bits.init = 1;
    frames->status.bits.lock = 0;
    return DATAFRAMES__OK;
}

int dataframes__decode_list(struct dataframes_t *frames, volatile uint8_t* buffer,
                            const size_t buffer_len, size_t* decoded_len)
{
    return 0;
}

