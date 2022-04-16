#include "testing_helper.h"
#include "chain.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void free_wrapper(void **obj)
{
    if (*obj) {
        printf("ready4free obj: %p\n", *obj);
        free(*obj);
        *obj = NULL;
    }
    printf("afterFree obj: %p\n", *obj);
}

int test_chain__append_and_trim()
{
    printf("---- testing chain: append & trim ------\n");
    struct chain_t *chain = NULL;
    for (int i = 0; i < 3; ++i) {
        chain_append_obj(chain, i, free_wrapper);
        printf(">>> append: %d\n", i);
    }

    while (chain) {
        printf(">>> trimming...\n");
        chain_trim(chain);
    }
    printf("----------------------------------------\n");
    return 0;
}

int main(void)
{
    TEST_RETURN(test_chain__append_and_trim());
    return 0;
}
