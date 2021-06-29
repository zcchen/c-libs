#include "testing_helper.h"
#include "class.h"

#include <stdio.h>
#include <assert.h>

void class_obj_print(struct class_t *cls)
{
    printf("--> class obj           : %p\n", cls->obj);
    printf("--> class size          : %ld\n", cls->size);
    printf("--> class parent        : %p\n", cls->parent);
    printf("--> class base.setup    : %p\n", &cls->methods.base.setup);
    printf("--> class base.clean    : %p\n", &cls->methods.base.clean);
    for (int i = 0; i < CLASS_MAX_USER_METHODS; ++i) {
        if (!cls->methods.user.func[i]) {
            continue;
        }
        printf("--> class user.func[%d]  : %p\n", i, &cls->methods.user.func[i]);
    }
}

int class_obj__setup(void* self, size_t size)
{
    printf("-------------\n");
    printf(">>> setup...\n");
    printf("-------------\n");
    return 0;
}

int class_obj__clean(void* self, size_t size)
{
    printf("-------------\n");
    printf(">>> clean...\n");
    printf("-------------\n");
    return 0;
}

int class_obj__func0(void* self, size_t size, void* param, size_t param_size)
{
    printf("-------------\n");
    printf(">>> func0...\n");
    printf("-------------\n");
    return 0;
}


int test_class_init__base()
{
    struct class_t base;
    class_init(&base);
    class_set_func_base(&base, class_obj__setup, class_obj__clean);
    class_set_func_user(&base, 0, class_obj__func0);
    class_call_func_setup(&base);
    class_call_func_clean(&base);
    class_call_func_user(&base, 0, NULL, 0);
    return 0;
}

int test_class_init__create_and_destroy()
{
    struct class_t *base = class_create();
    class_destroy(base);
    return 0;
}

int main(void)
{
    TEST_RETURN(test_class_init__base());
    TEST_RETURN(test_class_init__create_and_destroy());
    return 0;
}
