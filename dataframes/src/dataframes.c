#include "dataframes.h"
#include "endian.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <stdio.h>

// private checksum funcions
static uint16_t _checksum_sum(uint16_t last, volatile uint8_t *raw_data, const size_t size)
{
    uint16_t ret = last;
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

// destroy the nested child which in special types
void __dataframes_var__destroy(struct dataframes_var_t* frame)
{
    if (frame->type == dataframes_LIST_T) {
        struct dataframes_list_t* l = frame->value.list;
        if (l) {
            dataframes_list__destroy(l);    // destroy the list child.
        }
    }
    else if (frame->type == dataframes_STRING) {
        char* s = frame->value.strptr;
        if (s) {
            free(s);
        }
    }
}

void dataframes_var__destroy(struct dataframes_var_t* frame)
{
    __dataframes_var__destroy(frame);
    free(frame);
}

int dataframes_var__set(struct dataframes_var_t* frame,
                        const enum dataframes_type_t type, const void* value)
{
    frame->type = type;
    size_t string_size = 0;
    switch (type) {
        case dataframes_LIST_T:
            frame->value.list = (struct dataframes_list_t*)value;
            break;
        case dataframes_STRING:
            string_size = strlen((char*)value);
            frame->value.strptr = malloc(string_size + 1);
            if (!strncpy(frame->value.strptr, value, string_size + 1)) {
                // no chars copy to frame->value.strptr,
                free(frame->value.strptr);
                frame->value.strptr = NULL;
            }
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
    ret->capacity = 0;
    if (dataframes_list__init(ret, capacity)) {
        free(ret);
        return NULL;
    }
    return ret;
}

int dataframes_list__init(struct dataframes_list_t *l, size_t capacity)
{
    if (l->capacity && l->capacity != capacity) {
        for (int i = 0; i < l->capacity; ++i) {
            __dataframes_var__destroy(&l->list[i]);
        }
        free(l->list);
        l->capacity = 0;
        l->list = NULL;
    }
    if (capacity && l->capacity == 0) {
        struct dataframes_var_t* var_list = malloc(sizeof(struct dataframes_var_t) * capacity);
        l->capacity = capacity;
        l->list = var_list;
    }
    for (int i = 0; i < capacity; ++i) {
        if (dataframes_var__init(&l->list[i])) {
            free(l->list);
            l->capacity = 0;
            l->list = NULL;
            return DATAFRAMES__INIT_LIST_FAILED;
        }
    }
    return DATAFRAMES__OK;
}

void dataframes_list__destroy(struct dataframes_list_t *l)
{
    dataframes_list__init(l, 0);
    free(l);
}

size_t dataframes_list__get_var_num(const struct dataframes_list_t *l)
{
    size_t ret = 0;
    if (!l) {       // for the just NULL
        return 0;
    }
    for (int i = 0; i < l->capacity; ++i) {
        if (l->list[i].type == dataframes_LIST_T) {
            if (l->list[i].value.list) {
                ret += dataframes_list__get_var_num(l->list[i].value.list);
            }
            else {
                break;
            }
        }
        else {
            ret ++;
        }
    }
    return ret;
}

size_t dataframes_list__getsize(const struct dataframes_list_t *l)
{
    if (!l) {       // for the just NULL
        return 0;
    }
    for (int i = 0; i < l->capacity; ++i) {
        if (l->list[i].type == dataframes_LIST_T && l->list[i].value.list == NULL) {
            return i;
        }
    }
    return l->capacity;
}

int dataframes_list__setvalue(struct dataframes_list_t *l, const size_t index,
                              const enum dataframes_type_t type, const void* value)
{
    if (index >= l->capacity) {
        return DATAFRAMES__OVER_LIST_CAPACITY;
    }
    struct dataframes_var_t* frame = &l->list[index];
    return dataframes_var__set(frame, type, value);
}



struct dataframes_t* dataframes__create(const uint8_t head, const uint8_t tail,
                     const enum dataframes_checksum_t checksum)
{
    struct dataframes_t* ret = malloc(sizeof(struct dataframes_t));
    if (dataframes__init(ret, head, tail, checksum)) {
        dataframes__destroy(ret);
        return NULL;
    }
    return ret;
}

void dataframes__destroy(struct dataframes_t* frames)
{
    free(frames->data.frames);
    free(frames);
}

int dataframes__init(struct dataframes_t *frames,
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
    frames->length.rules.bits.include_checksum = 1;
    frames->length.rules.bits.include_tail = 1;
    // init the data frames
    frames->data.capacity = 255;
    for (int i = 0; i < frames->data.capacity; ++i) {
        frames->data.frames[i] = 0x00;
    }
    frames->data.size = 0;
    frames->data.rules.byte = 0;
    // init the checksum frames
    frames->checksum.value = 0;
    frames->checksum.rules.byte = 0x00;     // set the rules.
    frames->checksum.rules.bits.include_head = 1;
    frames->checksum.rules.bits.include_length = 1;
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
    *decoded_len = 0;        // set the decoded result to 0 first;

    if (!frames->status.bits.init) {
        return DATAFRAMES__FRAME_STRUCT_NOT_INIT;
    }
    if (frames->status.bits.lock) {
        return DATAFRAMES__FRAME_STRUCT_IS_LOCKED;
    }
    frames->status.bits.lock = 1;   // lock this dataframes
    frames->status.bits.ready = 0;

    size_t head_index = 0;  // the head frame index in buffer list
    size_t tail_index = 0;  // the head frame index in buffer list
    bool found_whole_msg = false;
    while (!found_whole_msg) {
        // 1. try to find the header
        for (int i = head_index; i < buffer_len; ++i) {
            if (buffer[i] == frames->head.frame) {
                head_index = i;
                break;
            }
            else {
                goto NO_HEADER_MSG_EXIT;
            }
        }
        // 2. header found, then find the length frame
        if (head_index + sizeof(frames->head.frame) >= buffer_len) {
            // just find the header, but the other frames do NOT contain valid msg.
            goto INVALID_MSG_EXIT;
        }
        frames->length.value = buffer[head_index + sizeof(frames->head.frame)];
        // 3. length frame found, then find the tail frame.
        tail_index = head_index + frames->length.value - 1; // assume length frame includs all
        if (!frames->length.rules.bits.include_head) {
            tail_index -= sizeof(frames->head.frame);
        }
        if (!frames->length.rules.bits.include_length) {
            tail_index -= sizeof(frames->length.value);
        }
        if (!frames->length.rules.bits.include_checksum) {
            tail_index -= sizeof(frames->checksum.value);
        }
        if (!frames->length.rules.bits.include_tail) {
            tail_index -= sizeof(frames->tail.frame);
        }
        // 4. check the tail frame.
        if (tail_index < buffer_len) {
            if (buffer[tail_index] == frames->tail.frame) {
                found_whole_msg = true;     // whole msg is found, bingo.
                break;
            }
        }
        head_index ++; // 5. if tail or length is invalid, means current head is a fake head.
    }
    // copy the buffer data msg to frames->data.frames
    size_t data_index_start = head_index + sizeof(frames->head.frame) + \
                              sizeof(frames->length.value);
    size_t data_index_end = tail_index - sizeof(frames->tail.frame) - \
                            sizeof(frames->checksum.value);
    if (frames->data.capacity < (data_index_end - data_index_start + 1)) {
        goto NOT_ENOUGH_DATA_CAPACITY;
    }
    frames->data.size = 0;
    for (int i = 0; i < (data_index_end - data_index_start + 1); ++i) {
        frames->data.frames[i] = *(buffer + data_index_start + i);
        frames->data.size ++;
    }
    for (int i = frames->data.size; i < frames->data.capacity; ++i) {
        frames->data.frames[i] = '\0';
    }
    // Assume the msg is correct, check the checksum.
    volatile uint8_t *checksum_buf = buffer + tail_index - \
                            sizeof(frames->tail.frame) - sizeof(frames->checksum.value) + 1;
    frames->checksum.value = clibs_ntohs(*(uint16_t*)(checksum_buf));
    uint16_t checksum_calc = 0;
    if (frames->checksum.calc) {
        if (frames->checksum.rules.bits.include_head) {
            checksum_calc = frames->checksum.calc(checksum_calc,
                    buffer + head_index,
                    sizeof(frames->head.frame));
        }
        if (frames->checksum.rules.bits.include_length) {
            checksum_calc = frames->checksum.calc(checksum_calc,
                    buffer + head_index + sizeof(frames->head.frame),
                    sizeof(frames->length.value));
        }
        checksum_calc = frames->checksum.calc(checksum_calc,
            buffer + head_index + sizeof(frames->head.frame) + sizeof(frames->length.value),
            frames->data.size);
        if (frames->checksum.value != checksum_calc) { // checksum failed, drop all
            goto INVALID_CHECKSUM_EXIT;
        }
    }   // otherwise, skip it
    goto NORMAL_EXIT;

NORMAL_EXIT:
    *decoded_len = tail_index + 1;
    frames->status.bits.ready = 1;  // this dataframes is ready to be used.
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__OK;
INVALID_MSG_EXIT:
    *decoded_len = head_index;
    dataframes__init(frames, frames->head.frame, frames->tail.frame, DATAFRAMES_CHECKSUM_SUM);
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__BUFFER_WITHOUT_VALID_MSG;
NO_HEADER_MSG_EXIT:
    *decoded_len = buffer_len;
    dataframes__init(frames, frames->head.frame, frames->tail.frame, DATAFRAMES_CHECKSUM_SUM);
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__BUFFER_WITHOUT_VALID_MSG;
INVALID_CHECKSUM_EXIT:
    *decoded_len = head_index;
    dataframes__init(frames, frames->head.frame, frames->tail.frame, DATAFRAMES_CHECKSUM_SUM);
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__MSG_WITHOUT_VALID_CHECKSUM;
NOT_ENOUGH_DATA_CAPACITY:
    *decoded_len = head_index;
    dataframes__init(frames, frames->head.frame, frames->tail.frame, DATAFRAMES_CHECKSUM_SUM);
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__NOT_ENOUGH_FRAMES_CAPACITY;
}

int dataframes__encode_list(struct dataframes_t *frames, volatile uint8_t* buffer,
                            const size_t buffer_len, size_t* encoded_len)
{
    *encoded_len = 0;        // set the encoded_len to 0, which will be sent out;
    // 1. check the struct dataframes_t status
    if (!frames->status.bits.ready) {
        return DATAFRAMES__FRAME_STRUCT_NOT_READY;
    }
    if (frames->status.bits.lock) {
        return DATAFRAMES__FRAME_STRUCT_IS_LOCKED;
    }
    // 2. check output buffer size
    size_t dataframes_size = sizeof(frames->head.frame) + sizeof(frames->length.value) + \
            frames->data.size + sizeof(frames->checksum.value) + sizeof(frames->tail.frame);
    if (dataframes_size > buffer_len) {
        return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
    }
    frames->status.bits.lock = 1;
    // 3. encode to the output buffer
    size_t buffer_index = 0;    // buffer encoded index
    *(buffer + buffer_index) = frames->head.frame;  // 3.a. head
    buffer_index += sizeof(frames->head.frame);
    *(buffer + buffer_index) = frames->length.value; // 3.b. length
    buffer_index += sizeof(frames->length.value);
    for (int i = 0; i < frames->data.size; ++i) {    // 3.c. data
        *(buffer + buffer_index + i) = frames->data.frames[i];
    }
    buffer_index += frames->data.size;
    *(uint16_t*)(buffer + buffer_index) = clibs_htons(frames->checksum.value); // 3.d. checksum
    buffer_index += sizeof(frames->checksum.value);
    *(buffer + buffer_index) = frames->tail.frame;  // 3.e. tail
    buffer_index += sizeof(frames->head.frame);
    // 4. return the encoded length
    *encoded_len = buffer_index;
    // 5. done
    frames->status.bits.lock = 0;
    return DATAFRAMES__OK;
}

int dataframes__setdata(struct dataframes_t *frames, const struct dataframes_list_t* data)
{
    // 1. check the struct dataframes_t status
    if (!frames->status.bits.init) {
        return DATAFRAMES__FRAME_STRUCT_NOT_INIT;
    }
    if (frames->status.bits.lock) {
        return DATAFRAMES__FRAME_STRUCT_IS_LOCKED;
    }
    frames->status.bits.lock = 1;
    frames->status.bits.ready = 0;
    // 2. set the data to frames
    if (data) {
        int ret = dataframes_list__conv_to_buffer(data, frames->data.frames,
                            frames->data.capacity, &frames->data.size);
        if (ret) {
            frames->status.bits.lock = 0;
            return ret;
        }
    }
    else {
        for (int i = 0; i < frames->data.capacity; ++i) {
            frames->data.frames[i] = '\0';
        }
        frames->data.size = 0;
    }
    // 3. set the dataframe size
    size_t dataframes_size = sizeof(frames->head.frame) + sizeof(frames->length.value) + \
            frames->data.size + sizeof(frames->checksum.value) + sizeof(frames->tail.frame);
    if (!frames->length.rules.bits.include_head) {
        dataframes_size -= sizeof(frames->head.frame);
    }
    if (!frames->length.rules.bits.include_length) {
        dataframes_size -= sizeof(frames->length.value);
    }
    if (!frames->length.rules.bits.include_checksum) {
        dataframes_size -= sizeof(frames->checksum.value);
    }
    if (!frames->length.rules.bits.include_tail) {
        dataframes_size -= sizeof(frames->tail.frame);
    }
    frames->length.value = dataframes_size;
    // 4. caclculate the checksum
    frames->checksum.value = 0;
    if (frames->checksum.calc) {
        if (frames->checksum.rules.bits.include_head) {
            frames->checksum.value = frames->checksum.calc(frames->checksum.value,
                    &frames->head.frame, sizeof(frames->head.frame));
        }
        if (frames->checksum.rules.bits.include_length) {
            frames->checksum.value = frames->checksum.calc(frames->checksum.value,
                    &frames->length.value, sizeof(frames->length.value));
        }
        frames->checksum.value = frames->checksum.calc(frames->checksum.value,
                frames->data.frames, frames->data.size);
    }
    // 5. done
    frames->status.bits.ready = 1;
    frames->status.bits.lock = 0;
    return DATAFRAMES__OK;
}

int dataframes__getdata(const struct dataframes_t *frames, struct dataframes_list_t* data)
{
    // 1. check the struct dataframes_t status
    if (!frames->status.bits.ready) {
        return DATAFRAMES__FRAME_STRUCT_NOT_READY;
    }
    if (frames->status.bits.lock) {
        return DATAFRAMES__FRAME_STRUCT_IS_LOCKED;
    }
    if (!data) {
        return DATAFRAMES__DATAFRAME_LIST_IS_NULL;
    }
    // 2. get data from data->frames, since it is allocate and checked.
    size_t tmp_decoded_len = 0;
    int ret = dataframes_list__conv_from_buffer(data, frames->data.frames,
                                      frames->data.capacity, &tmp_decoded_len);
    if (ret) {
        return ret;
    }
    if (tmp_decoded_len != frames->data.size) {
        return DATAFRAMES__MSG_DECODED_LENGTH_ERROR;
    }
    return DATAFRAMES__OK;
}

int dataframes_list__conv_to_buffer(const struct dataframes_list_t *l,
                                    uint8_t *buffer, const size_t maxlen, size_t* conv_len)
{
    *conv_len = 0;
    size_t conv_size = 0;
    for (int i = 0; i < dataframes_list__getsize(l); ++i) {
        struct dataframes_var_t *var = &l->list[i];
        size_t try_to_conv_len = 0;
        int tmp_ret = 0;
        switch (var->type) {
            case dataframes_LIST_T:
                tmp_ret = dataframes_list__conv_to_buffer(var->value.list,
                            buffer + conv_size, maxlen - conv_size, &try_to_conv_len);
                if (tmp_ret) {
                    return tmp_ret;
                }
                conv_size += try_to_conv_len;
                break;
            case dataframes_STRING:
                if (var->value.strptr) {
                    try_to_conv_len = strlen(var->value.strptr);
                    if (try_to_conv_len + 1 > maxlen - conv_size) {
                        return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                    }
                    strncpy((char*)(buffer + conv_size), var->value.strptr, try_to_conv_len);
                    conv_size += try_to_conv_len + 1;   // including '\0' char
                }
                break;
            case dataframes_UINT8_T:
                if (sizeof(uint8_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint8_t*)(buffer + conv_size) = var->value.uint8;
                conv_size += sizeof(uint8_t);
                break;
            case dataframes_INT8_T:
                if (sizeof(int8_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int8_t*)(buffer + conv_size) = var->value.int8;
                conv_size += sizeof(int8_t);
                break;
            case dataframes_UINT16_T:
                if (sizeof(uint16_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint16_t*)(buffer + conv_size) = clibs_htons(var->value.uint16);
                conv_size += sizeof(uint16_t);
                break;
            case dataframes_INT16_T:
                if (sizeof(int16_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int16_t*)(buffer + conv_size) = clibs_htons(var->value.int16);
                conv_size += sizeof(int16_t);
                break;
            case dataframes_UINT32_T:
                if (sizeof(uint32_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint32_t*)(buffer + conv_size) = clibs_htonl(var->value.uint32);
                conv_size += sizeof(uint32_t);
                break;
            case dataframes_INT32_T:
                if (sizeof(int32_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int32_t*)(buffer + conv_size) = clibs_htonl(var->value.int32);
                conv_size += sizeof(int32_t);
                break;
            case dataframes_UINT64_T:
                if (sizeof(uint64_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint64_t*)(buffer + conv_size) = clibs_htonl(var->value.uint64);
                conv_size += sizeof(uint64_t);
                break;
            case dataframes_INT64_T:
                if (sizeof(int64_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int64_t*)(buffer + conv_size) = clibs_htonll(var->value.int64);
                conv_size += sizeof(int64_t);
                break;
            case dataframes_FLOAT:
                if (sizeof(float) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(float*)(buffer + conv_size) = clibs_htons(var->value.float16);
                conv_size += sizeof(float);
                break;
            case dataframes_DOUBLE:
                if (sizeof(double) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(double*)(buffer + conv_size) = clibs_htonl(var->value.double32);
                conv_size += sizeof(double);
                break;
            case dataframes_LONGDOUBLE:
                if (sizeof(long double) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(long double*)(buffer + conv_size) = clibs_htonll(var->value.longdouble64);
                conv_size += sizeof(long double);
                break;
            default:
                return DATAFRAMES__VAR_TYPE_UNKNOWN;
        }
    }
    *conv_len = conv_size;
    return DATAFRAMES__OK;
}

int dataframes_list__conv_from_buffer(struct dataframes_list_t *l,
                            const uint8_t *buffer, const size_t maxlen, size_t *decoded_len)
{
    *decoded_len = 0;
    size_t pri_decoded_len = 0;
    for (int i = 0; i < dataframes_list__getsize(l); ++i) {
        struct dataframes_var_t *var = &l->list[i];
        size_t try_decoding_len = 0;
        int tmp_ret = 0;
        switch (var->type) {
            case dataframes_LIST_T:
                tmp_ret = dataframes_list__conv_from_buffer(var->value.list,
                        buffer + pri_decoded_len, maxlen - pri_decoded_len, &try_decoding_len);
                if (tmp_ret) {
                    return tmp_ret;
                }
                pri_decoded_len += try_decoding_len;
                break;
            case dataframes_STRING:
                try_decoding_len = strlen((char*)(buffer + pri_decoded_len));
                if (try_decoding_len + 1 > maxlen - pri_decoded_len) {
                    return DATAFRAMES__BUFFER_CHAR_OVERFLOW; // buffer char* overflow
                }
                if (var->value.strptr) {
                    free(var->value.strptr);
                }
                var->value.strptr = malloc(try_decoding_len + 1);
                strncpy(var->value.strptr, (char*)(buffer+pri_decoded_len), try_decoding_len);
                pri_decoded_len += try_decoding_len + 1;    // including the '\0' char.
                break;
            case dataframes_UINT8_T:
                if (sizeof(uint8_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.uint8 = *(uint8_t*)(buffer + pri_decoded_len);
                pri_decoded_len += sizeof(uint8_t);
                break;
            case dataframes_INT8_T:
                if (sizeof(int8_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.uint8 = *(int8_t*)(buffer + pri_decoded_len);
                pri_decoded_len += sizeof(int8_t);
                break;
            case dataframes_UINT16_T:
                if (sizeof(uint16_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.uint16 = clibs_htons(*(uint16_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(uint16_t);
                break;
            case dataframes_INT16_T:
                if (sizeof(int16_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.int16 = clibs_htons(*(int16_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(int16_t);
                break;
            case dataframes_UINT32_T:
                if (sizeof(uint32_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.uint32 = clibs_htonl(*(uint32_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(uint32_t);
                break;
            case dataframes_INT32_T:
                if (sizeof(int32_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.int32 = clibs_htonl(*(int32_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(int32_t);
                break;
            case dataframes_UINT64_T:
                if (sizeof(uint64_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.uint64 = clibs_htonll(*(uint64_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(uint64_t);
                break;
            case dataframes_INT64_T:
                if (sizeof(int64_t) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.int64 = clibs_htonll(*(int64_t*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(int64_t);
                break;
            case dataframes_FLOAT:
                if (sizeof(float) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.float16 = clibs_htons(*(float*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(float);
                break;
            case dataframes_DOUBLE:
                if (sizeof(double) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.double32 = clibs_htonl(*(double*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(double);
                break;
            case dataframes_LONGDOUBLE:
                if (sizeof(long double) > maxlen - pri_decoded_len) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                var->value.longdouble64 = clibs_htonll(*(long double*)(buffer + pri_decoded_len));
                pri_decoded_len += sizeof(long double);
                break;
            default:
                return DATAFRAMES__VAR_TYPE_UNKNOWN;
        }
    }
    *decoded_len = pri_decoded_len;
    return DATAFRAMES__OK;
}
