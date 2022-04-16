#include "testing_helper.h"
#include "enumkv.h"

#include <stdio.h>

void enumkv_elem_print(struct enumkv_elem_t *elem, char *s)
{
    if (s) {
        printf("--> %s\n", s);
    }
    printf("--> enum: %d\t", elem->enum_k);
    printf("--> value(addr): %p\n", elem->value);
}

int test_enumkv_list_add__add_new_elem_at_length_N(const size_t list_len, uint8_t key, int value)
{
    printf("\n---- enumkv_add() test: add new elem --------\n");
    printf("list_len: %ld\t", list_len);
    printf("key: %d\t", key);
    printf("value: %d\n", value);

    int val = value;
    struct enumkv_elem_t elem = {.enum_k = key, .value=&val};
    enumkv_elem_print(&elem, "The elem will be added.");

    struct enumkv_elem_t enumkv_list[list_len];
    enumkv_list_init(enumkv_list, list_len);
    int ret = enumkv_list_add(enumkv_list, list_len, &elem);
    printf("---- ret: <%d> ---------------------------------\n", ret);

    printf(">>> enum list added result: --------------------\n");
    for (size_t i = 0; i < list_len; ++i) {
        enumkv_elem_print(&enumkv_list[i], NULL);
    }

    if (list_len == 0 && ret == ENUMKV_ERR_NO_SPACE) {
        return 0;
    }
    else if (key == 0 && ret == ENUMKV_ERR_KEY_INVALID) {
        return 0;
    }
    else {
        return ret;
    }
}

int test_enumkv_list_add__add_existed_elem_at_length_N(const size_t list_len,
        uint8_t key, int value)
{
    printf("\n---- enumkv_add() test: add existed elem ------\n");
    printf("list_len: %ld\t", list_len);
    printf("key: %d\t", key);
    printf("value: %d\n", value);

    int val = value;
    struct enumkv_elem_t elem = {.enum_k = key, .value=&val};
    enumkv_elem_print(&elem, "The elem will be added.");

    struct enumkv_elem_t enumkv_list[list_len];
    enumkv_list_init(enumkv_list, list_len);
    enumkv_list_add(enumkv_list, list_len, &elem);
    int ret = enumkv_list_add(enumkv_list, list_len, &elem);
    printf("---- ret: <%d> ---------------------------------\n", ret);

    printf(">>> enum list added result: --------------------\n");
    for (size_t i = 0; i < list_len; ++i) {
        enumkv_elem_print(&enumkv_list[i], NULL);
    }

    if (list_len == 0 && ret == ENUMKV_ERR_NO_SPACE) {
        return 0;
    }
    else if (key == 0 && ret == ENUMKV_ERR_KEY_INVALID) {
        return 0;
    }
    else if (ret == ENUMKV_ERR_KEY_EXISTS) {
        return 0;
    }
    else if (ret == ENUMKV_OK) {
        return -1;
    }
    else {
        return ret;
    }
}

int main(void)
{
    TEST_RETURN(test_enumkv_list_add__add_new_elem_at_length_N(0, 0, 1));
    TEST_RETURN(test_enumkv_list_add__add_new_elem_at_length_N(0, 1, 1));
    TEST_RETURN(test_enumkv_list_add__add_new_elem_at_length_N(1, 1, 1));
    TEST_RETURN(test_enumkv_list_add__add_new_elem_at_length_N(2, 1, 1));

    TEST_RETURN(test_enumkv_list_add__add_existed_elem_at_length_N(0, 0, 1));
    TEST_RETURN(test_enumkv_list_add__add_existed_elem_at_length_N(0, 1, 1));
    TEST_RETURN(test_enumkv_list_add__add_existed_elem_at_length_N(1, 1, 1));
    TEST_RETURN(test_enumkv_list_add__add_existed_elem_at_length_N(2, 1, 1));
    return 0;
}
