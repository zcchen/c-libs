#include "testing_helper.h"
#include "enumkv.h"

#include <stdio.h>

void enumkv_elem_print(struct enumkv_elem_t *elem)
{
    printf("--> enum: %d\t", elem->enum_k);
    printf("--> value(addr): %p\n", elem->value);
}

int test_enumkv_list_init__length_is_n(uint8_t n)
{
    printf("\n---- enumkv_list() test: --------\n");
    printf("n = %d\n", n);
    struct enumkv_elem_t enumkv_list[n];
    int ret = enumkv_list_init(enumkv_list, n);
    for (int i = 0; i < n; ++i) {
        enumkv_elem_print(&enumkv_list[i]);
    }
    printf("-----------------------------------\n");
    if (ret == ENUMKV_OK) {
        return 0;
    }
    else {
        return ret;
    }
}

int main(void)
{
    TEST_RETURN(test_enumkv_list_init__length_is_n(0));
    TEST_RETURN(test_enumkv_list_init__length_is_n(1));
    TEST_RETURN(test_enumkv_list_init__length_is_n(5));
    return 0;
}
