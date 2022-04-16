#include "testing_helper.h"
#include "object.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define MY_BUF_SIZE 256

void object_print(const struct object_t *obj)
{
    printf("--> object         : %p\n", obj);
    printf("--> object.instance: %p\n", obj->instance);
    printf("--> object.size    : %ld\n", obj->size);
    printf("--> object.destroy : %p\n", obj->destroy);
    printf("-------------------------------\n");
}

void obj_purge__print(void** instance)
{
    printf(">>> purging print ...\n");
    printf(">>> instance ptr is %p\n", *instance);
}

void obj_purge__free(void** instance)
{
    printf(">>> purging free ...\n");
    free(*instance);
    *instance = NULL;
}

int test_object__create_destroy()
{
    printf("----- testing object create() & destroy() ------------------------\n");

    struct object_t *obj0 = object_create();
    object_print(obj0);
    object_destroy(obj0);

    struct object_t *obj1 = object_create();
    long long * val = malloc(sizeof(long long));
    *val = 123;
    object_set_pointer(obj1, val, sizeof(long long), obj_purge__free);
    object_print(obj1);
    printf(">>> obj value: %lld\n", *object_get(obj1, long long));
    object_destroy(obj1);

    printf("-------------------------------------------------------------------\n");
    return 0;
}

int test_object__init_set_purge_with_NULL()
{
    printf("----- testing object init(), set_pointer(), get() with NULL -------\n");

    struct object_t obj;
    assert(0 == object_init(&obj));
    printf(">>> obj init ...\n");

    assert(0 == object_set_pointer(&obj, NULL, 0, obj_purge__print));
    assert(NULL == obj.instance);
    assert(0 == obj.size);
    assert(NULL == object_get(&obj, void*));

    object_print(&obj);
    printf("-------------------------------------------------------------------\n");
    return 0;
}

int test_object__init_set_purge_with_int()
{
    printf("----- testing object init(), set_instance(), get() with INT -------\n");

    struct object_t obj;
    assert(0 == object_init(&obj));
    printf(">>> obj init ...\n");

    int val = 1;
    assert(0 == object_set_instance(&obj, val, obj_purge__print));
    assert(val == *(int*)obj.instance);    // check the value if same
    assert(sizeof(val) == obj.size);
    assert(obj.instance == object_get(&obj, int));

    object_print(&obj);
    printf("-------------------------------------------------------------------\n");
    return 0;
}


int main(void)
{
    TEST_RETURN(test_object__create_destroy());
    TEST_RETURN(test_object__init_set_purge_with_NULL());
    TEST_RETURN(test_object__init_set_purge_with_int());
    return 0;
}
