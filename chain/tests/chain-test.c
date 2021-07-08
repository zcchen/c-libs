#include "testing_helper.h"
#include "chain.h"

#include <stdio.h>
#include <assert.h>

bool tmp_cond(const void* obj, const size_t size)
{
    if (size != sizeof(int)) {
        return false;
    }
    return (0 == *(int*)(obj));
}

int test_chain__append_and_trim()
{
    printf("---- testing chain: append & trim ------\n");
    struct chain_t *chain = NULL;

    printf(">>> try to append the objects to chain...\n");
    for (int i = 0; i < 3; ++i) {
        chain_append(chain, i);
    }
    assert(chain == chain_find_condition(chain, tmp_cond));
    for (int i = 0; i < 3; ++i) {
        chain_insert(chain, i);
    }

    printf(">>> printing the objects in chain...\n");
    int i = 0;
    struct chain_t *node = chain;
    while (node) {
        printf("   chain[%d]: %p\n", i, node);
        int *node_obj = node->obj;
        printf("   chain[%d]: %d\n", i, *node_obj);
        i++;
        node = node->next;
    }

    printf(">>> try to trune the tail from chain...\n");
    chain_trunc(chain);

    printf(">>> try to trim the head from chain...\n");
    while (chain) {
        chain_trim(chain);
    }
    printf(">>> try to trune / trim the tail from empty chain...\n");
    chain_trunc(chain);
    chain_trim(chain);

    printf("----------------------------------------\n");
    return 0;
}

int main(void)
{
    TEST_RETURN(test_chain__append_and_trim());
    return 0;
}
