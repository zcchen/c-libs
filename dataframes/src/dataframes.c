#include "dataframes.h"
#include "endian.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
                        const enum dataframes_type_t type, const void* value)
{
    frame->type = type;
    switch (type) {
        case dataframes_LIST_T:
            frame->value.list = (struct dataframes_list_t*)value;
            break;
        case dataframes_STRING:
            frame->value.strptr = (char*)value;
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

size_t dataframes_list__getsize(const struct dataframes_list_t *l)
{
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
            }
        }
        // 5. if tail or length is invalid, means the current head is not the REAL header.
        head_index ++;
    }
    // Assume the msg is found, check the checksum first.
    volatile uint8_t *checksum_buf = buffer + tail_index - \
                            sizeof(frames->tail.frame) - sizeof(frames->checksum.value) + 1;
    frames->checksum.value = clibs_ntohs(*(uint16_t*)(checksum_buf));
    size_t checksum_size = tail_index - head_index + 1 - \
                           sizeof(frames->checksum.value) - sizeof(frames->tail.frame);
        // assume checksum include all possibles
    if (!frames->checksum.rules.bits.include_head) {
        checksum_size -= sizeof(frames->head.frame);
    }
    if (!frames->checksum.rules.bits.include_length) {
        checksum_size -= sizeof(frames->length.value);
    }
    if (frames->checksum.calc) {
        if (frames->checksum.value != \
            frames->checksum.calc(0, buffer + head_index, checksum_size)) {
            goto INVALID_CHECKSUM_EXIT;
        }   // otherwise, skip it
    }
    // Checksum passed, msg structure is valid, docode it to the frame parts.
    volatile uint8_t *data_start_index = buffer + head_index + sizeof(frames->length.value) + 1;
    volatile uint8_t *data_end_index = buffer + tail_index - \
                              sizeof(frames->tail.frame) - sizeof(frames->checksum.value) - 1;
    if (frames->data.capacity < (data_end_index - data_start_index + 1)) {
        goto NOT_ENOUGH_DATA_CAPACITY;
    }
    frames->data.size = 0;
    for (int i = 0; i < (data_end_index - data_start_index + 1); ++i) {
        *(frames->data.frames + i) = *(data_start_index + i);
        frames->data.size ++;
    }
    goto NORMAL_EXIT;

NORMAL_EXIT:
    *decoded_len = tail_index;
    frames->status.bits.ready = 1;  // this dataframes is ready to be used.
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__OK;
INVALID_MSG_EXIT:
    *decoded_len = head_index;
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__BUFFER_WITHOUT_VALID_MSG;
NO_HEADER_MSG_EXIT:
    *decoded_len = buffer_len; frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__BUFFER_WITHOUT_VALID_MSG;
INVALID_CHECKSUM_EXIT:
    *decoded_len = head_index;
    frames->status.bits.lock = 0;   // unlock this dataframes
    return DATAFRAMES__MSG_WITHOUT_VALID_CHECKSUM;
NOT_ENOUGH_DATA_CAPACITY:
    *decoded_len = head_index;
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
    *encoded_len = buffer_index + 1;
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
    // 2. check the size and capacity available
    size_t data_size = dataframes_list__getsize(data);
    if (data_size > frames->data.capacity) {
        return DATAFRAMES__NOT_ENOUGH_FRAMES_CAPACITY;
    }
    frames->status.bits.lock = 1;
    // 3. set the dataframe size
    size_t dataframes_size = sizeof(frames->head.frame) + sizeof(frames->length.value) + \
            data_size + sizeof(frames->checksum.value) + sizeof(frames->tail.frame);
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
    // 4. set the data to frames
    int ret = dataframes_list__conv_to_buffer(data,
            frames->data.frames, frames->data.capacity, &frames->data.size);
    if (ret) {
        frames->status.bits.lock = 0;
        return ret;
    }
    // 5. caclculate the checksum
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
    // 6. done
    frames->status.bits.ready = 1;
    frames->status.bits.lock = 0;
    return DATAFRAMES__OK;
}

int dataframes__getdata(struct dataframes_t *frames, const struct dataframes_list_t* data)
{   // TODO
    return 0;
}

int dataframes_list__conv_to_buffer(const struct dataframes_list_t *l,
                                    uint8_t *buffer, const size_t maxlen, size_t* conv_len)
{
    *conv_len = 0;
    size_t conv_size = 0;
    for (int i = 0; i < dataframes_list__getsize(l); ++i) {
        struct dataframes_var_t *var = l->list + sizeof(struct dataframes_var_t) * i;
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
                try_to_conv_len = strlen(var->value.strptr);
                if (try_to_conv_len > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                strncpy((char*)(buffer + conv_size), var->value.strptr, try_to_conv_len);
                conv_size += try_to_conv_len;
            case dataframes_UINT8_T:
                if (sizeof(uint8_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint8_t*)(buffer + conv_size) = var->value.uint8;
                conv_size += sizeof(uint8_t);
            case dataframes_INT8_T:
                if (sizeof(int8_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int8_t*)(buffer + conv_size) = var->value.int8;
                conv_size += sizeof(int8_t);
            case dataframes_UINT16_T:
                if (sizeof(uint16_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint16_t*)(buffer + conv_size) = clibs_htons(var->value.uint16);
                conv_size += sizeof(uint16_t);
            case dataframes_INT16_T:
                if (sizeof(int16_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int16_t*)(buffer + conv_size) = clibs_htons(var->value.int16);
                conv_size += sizeof(int16_t);
            case dataframes_UINT32_T:
                if (sizeof(uint32_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint32_t*)(buffer + conv_size) = clibs_htonl(var->value.uint32);
                conv_size += sizeof(uint32_t);
            case dataframes_INT32_T:
                if (sizeof(int32_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int32_t*)(buffer + conv_size) = clibs_htonl(var->value.int32);
                conv_size += sizeof(int32_t);
            case dataframes_UINT64_T:
                if (sizeof(uint64_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(uint64_t*)(buffer + conv_size) = clibs_htonl(var->value.uint64);
                conv_size += sizeof(uint64_t);
            case dataframes_INT64_T:
                if (sizeof(int64_t) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(int64_t*)(buffer + conv_size) = clibs_htonll(var->value.int64);
                conv_size += sizeof(int64_t);
            case dataframes_FLOAT:
                if (sizeof(float) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(float*)(buffer + conv_size) = clibs_htons(var->value.float16);
                conv_size += sizeof(float);
            case dataframes_DOUBLE:
                if (sizeof(double) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(double*)(buffer + conv_size) = clibs_htonl(var->value.double32);
                conv_size += sizeof(double);
            case dataframes_LONGDOUBLE:
                if (sizeof(long double) > maxlen - conv_size) {
                    return DATAFRAMES__NOT_ENOUGH_BUFFER_CAPACITY;
                }
                *(long double*)(buffer + conv_size) = clibs_htonll(var->value.longdouble64);
                conv_size += sizeof(long double);
            default:
                return DATAFRAMES__VAR_TYPE_UNKNOWN;
        }
    }
    *conv_len = conv_size;
    return DATAFRAMES__OK;
}

int dataframes_list__conv_from_buffer(const struct dataframes_list_t *l,
                                      uint8_t *buffer, const size_t maxlen, size_t* conv_len)
{
    // TODO
    return DATAFRAMES__OK;
}
