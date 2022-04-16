#include "testing_helper.h"
#include "enumkv.h"

#include <stdio.h>
#include <assert.h>

void enumkv_elem_print(struct enumkv_elem_t *elem, char *s)
{
    if (s) {
        printf("--> %s\n", s);
    }
    printf("--> enum: %d\t", elem->enum_k);
    printf("--> value(addr): %p\n", elem->value);
}

int test_enumkv_list_find__at_normal_list(const size_t list_len, uint8_t key, int value)
{
    printf("\n---- enumkv_find() test: at normal length --------\n");

    struct enumkv_elem_t enumkv_list[list_len];
    enumkv_list_init(enumkv_list, list_len);
    int val = value;
    struct enumkv_elem_t elem = {.enum_k = key, .value=&val};
    int add_ret = enumkv_list_add(enumkv_list, list_len, &elem);
    if (list_len == 0 && add_ret == ENUMKV_ERR_NO_SPACE) {
        return 0;
    }
    else if (key == 0 && add_ret == ENUMKV_ERR_KEY_INVALID) {
        return 0;
    }

    for (size_t i = 0; i < list_len; ++i) {
        enumkv_elem_print(&enumkv_list[i], NULL);
    }

    void *found_val = NULL;
    int ret = enumkv_list_find(enumkv_list, list_len, elem.enum_k, &found_val);

    assert(found_val == &val);
    printf("---- ret: <%d> ---------------------------------\n", ret);
    printf("--> val: %d;\tfound_val: %d\n", val, *(int*)found_val);

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

int test_enumkv_list_find__at_not_found_list(const size_t list_len, uint8_t key, int value)
{
    printf("\n---- enumkv_find() test: at not-found length --------\n");

    struct enumkv_elem_t enumkv_list[list_len];
    enumkv_list_init(enumkv_list, list_len);
    int val_added = value;
    struct enumkv_elem_t elem_added = {.enum_k = key, .value=&val_added};
    int val_searched = value;
    struct enumkv_elem_t elem_searched = {.enum_k = (key & 0x7F) + 1, .value=&val_searched};

    int add_ret = enumkv_list_add(enumkv_list, list_len, &elem_added);
    if (list_len == 0 && add_ret == ENUMKV_ERR_NO_SPACE) {
        return 0;
    }
    else if (key == 0 && add_ret == ENUMKV_ERR_KEY_INVALID) {
        return 0;
    }

    for (size_t i = 0; i < list_len; ++i) {
        enumkv_elem_print(&enumkv_list[i], NULL);
    }
    void *found_val = NULL;
    int ret = enumkv_list_find(enumkv_list, list_len, elem_searched.enum_k, &found_val);

    assert(found_val == NULL);
    printf("---- ret: <%d> ---------------------------------\n", ret);

    if (list_len == 0 && ret == ENUMKV_ERR_NO_SPACE) {
        return 0;
    }
    else if (key == 0 && ret == ENUMKV_ERR_KEY_INVALID) {
        return 0;
    }
    else if (ret == ENUMKV_ERR_KEY_NOT_FOUND) {
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
    TEST_RETURN(test_enumkv_list_find__at_normal_list(0, 1, 2));
    TEST_RETURN(test_enumkv_list_find__at_normal_list(1, 1, 2));
    TEST_RETURN(test_enumkv_list_find__at_normal_list(5, 1, 2));

    TEST_RETURN(test_enumkv_list_find__at_not_found_list(0, 1, 2));
    TEST_RETURN(test_enumkv_list_find__at_not_found_list(1, 1, 2));
    TEST_RETURN(test_enumkv_list_find__at_not_found_list(5, 1, 2));
    return 0;
}
