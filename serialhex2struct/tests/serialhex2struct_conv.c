#include "testing_helper.h"
#include "serialhex2struct.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static union { char c[4]; unsigned long mylong; } endian_test = {{ 'l', '?', '?', 'b' }};
//! \brief ENDIANNESS is to show the CPU endian
#define ENDIANNESS ((char)endian_test.mylong)

struct conv_t {
    uint8_t first;
    uint16_t second;
    uint32_t third;
} __attribute__((packed));

void conv_data_init(struct conv_t *d)
{
    d->first = 0;
    d->second = 0;
    d->third = 0;
}

void conv_data_print(const struct conv_t *d, const char *s)
{
    if (s != NULL) {
        printf("conv_data (%p) - %s\n", d, s);
    }
    else {
        printf("conv_data (%p):\n", d);
    }
    printf("  sizeof: %ld bytes\n", sizeof(struct conv_t));
    printf("  first: 0x%x\n", d->first);
    printf("  second: 0x%x\n", d->second);
    printf("  third: 0x%x\n", d->third);
}

void uint8_hex_print(const uint8_t *d, size_t len)
{
    printf("uint8_hex (%p): <%s>\n", d, (char*)d);
    for (int i = 0; i < len; ++i) {
        printf("  [%d]: 0x%x", i, d[i]);
    }
    printf("\n");
}


int test__serialhex_2_struct__just_matched()
{
    printf("\n---- serialhex_2_struct test() ---------------------\n");
    printf("------ the char and the struct is just matched.\n");
    char *raw = "1121234";
    struct conv_t conv;
    conv_data_init(&conv);
    uint8_hex_print((uint8_t*)raw, strlen(raw));
    conv_data_print(&conv, "just init");

    int ret = serialhex_2_struct((uint8_t*)raw, strlen(raw), conv);

    conv_data_print(&conv, "After init");
    printf(">>> serialhex_2_struct ret: <%d>\n", ret);
    printf("--------------------------------------------------------\n");

    assert(conv.first == 0x31);
    if (ENDIANNESS == 'b') {
        assert(conv.second == 0x3132);
    }
    else if (ENDIANNESS == 'l') {
        assert(conv.second == 0x3231);
    }
    else {
        return 1;
    }
    if (ENDIANNESS == 'b') {
        assert(conv.third == 0x31323334);
    }
    else if (ENDIANNESS == 'l') {
        assert(conv.third == 0x34333231);
    }
    else {
        return 2;
    }
    return 0;
}

int test__serialhex_2_struct__null_data()
{
    printf("\n---- serialhex_2_struct test() ---------------------\n");
    printf("------ null-data is passed in...\n");
    char *raw = NULL;
    void *conv;
    int ret = serialhex_2_struct((uint8_t*)raw, 0, conv);

    printf(">>> serialhex_2_struct ret: <%d>\n", ret);
    printf("--------------------------------------------------------\n");
    if (ret == SERIALHEX_2_STRUCT_ERR_EMPTY_INPUT) {
        return 0;
    }
    else {
        return ret;
    }
}


int main(void)
{
    TEST_RETURN(test__serialhex_2_struct__just_matched());
    TEST_RETURN(test__serialhex_2_struct__null_data());
    return 0;
}
