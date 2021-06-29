#include "testing_helper.h"
#include "class.h"

#include <stdio.h>
#include <assert.h>

int test_class_fork__chain(size_t level)
{
    printf("---- testing the class_t fork_chains() with level <%ld> ------\n", level);

    printf(">>> preparing the base class ...\n");
    struct class_t *base = NULL;        // setup multi level class_t object
    for (size_t i = 0; i < level; ++i) {
        struct class_t *child = class_create();
        if (base) {
            class_fork_chain(base, child);  // fork the new one.
            class_destroy(base);            // destroy the old ones
        }
        base = child;                       // move the pointer to old ones for loop
    }

    printf(">>> preparing the fork working object ...\n");
    struct class_t *fork = class_create();

    printf(">>> forking ...\n");
    class_fork_chain(base, fork);

    printf(">>> Asserting the class inherited levels ...\n");
    printf(">>> class_get_level(base): %ld\n", class_get_level(base));
    printf(">>> class_get_level(fork): %ld\n", class_get_level(fork));
    assert(class_get_level(base) + 1 == class_get_level(fork));

    printf(">>> destroying the base and fork ...\n");
    class_destroy(base);
    class_destroy(fork);

    printf("------------------------------------------------------------\n");
    return 0;
}

int test_class_fork__list(size_t level)
{
    printf("---- testing the class_t fork_list() with level <%ld> ------\n", level);

    printf(">>> preparing the base class ...\n");
    struct class_t *base = NULL;        // setup multi level class_t object
    for (size_t i = 0; i < level; ++i) {
        struct class_t *child = class_create();
        if (base) {
            class_fork_chain(base, child);  // fork the new one.
            class_destroy(base);            // destroy the old ones
        }
        base = child;                       // move the pointer to old ones for loop
    }

    printf(">>> preparing the fork list with [%ld] ...\n", level + 1);
    struct class_t fork[level + 1];

    printf(">>> forking ...\n");
    class_fork_list(base, fork, level + 1);

    printf(">>> Asserting the class inherited levels ...\n");
    printf(">>> class_get_level(base): %ld\n", class_get_level(base));
    printf(">>> class_get_level(fork): %ld\n", class_get_level(fork));
    assert(class_get_level(base) + 1 == class_get_level(fork));

    printf(">>> destroying the base and fork ...\n");
    class_destroy(base);

    printf("----------------------------------------------------------\n");
    return 0;
}

int main(void)
{
    TEST_RETURN(test_class_fork__chain(0));
    TEST_RETURN(test_class_fork__chain(1));
    TEST_RETURN(test_class_fork__chain(5));

    TEST_RETURN(test_class_fork__list(0));
    TEST_RETURN(test_class_fork__list(1));
    TEST_RETURN(test_class_fork__list(5));
    return 0;
}
