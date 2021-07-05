#ifndef ZCCHEN_CLIBS__SERIALHEX2STRUCT_H
#define ZCCHEN_CLIBS__SERIALHEX2STRUCT_H

#include <stdint.h>
#include <stdlib.h>

typedef unsigned char uint8_t;

enum serialhex_2_struct_err_t {
    SERIALHEX_2_STRUCT_OK = 0,
    SERIALHEX_2_STRUCT_ERR_UNKNOWN = 1,
    SERIALHEX_2_STRUCT_ERR_EMPTY_INPUT = 2,
    SERIALHEX_2_STRUCT_ERR_SIZE_UNMATCHED = 3,
    SERIALHEX_2_STRUCT_ERR_COPY_FAILED = 4,
};

struct serialhex_var_t {
    enum serialhex_type_t {
        SERIALHEX_UINT8_T = 0,
        SERIALHEX_INT8_T = 1,
        SERIALHEX_UINT16_T = 2,
        SERIALHEX_INT16_T = 3,
        SERIALHEX_UINT32_T = 3,
        SERIALHEX_INT32_T = 4,
        SERIALHEX_FLOAT = 5,
        SERIALHEX_DOUBLE = 6,
    } type;
    union serialhex_value_t {
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

struct serialhex_list_t {
    size_t size;
    struct serialhex_var_t l[];
};

int _shadow_serialhex_2_struct(const uint8_t *data, const size_t data_len,
                               void* ret_struct, size_t struct_size);
// because the `ret_struct` is undefined yet, the endian-convection should be done by you.

int serialhex_2_struct(const uint8_t *data, const size_t data_len,
                       //const struct serialhex_styles *style,
                       void *ret_struct, size_t struct_size);

#define serialhex_2_struct(serial_data, data_len, ret_struct)   \
        _shadow_serialhex_2_struct(serial_data, data_len, &ret_struct, sizeof(ret_struct))

#endif /* ifndef ZCCHEN_CLIBS__SERIALHEX2STRUCT_H */
