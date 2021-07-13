#ifndef ZCCHEN_CLIBS__CHAIN_H
#define ZCCHEN_CLIBS__CHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

struct chain_t {
    struct chain_t *prev;
    struct chain_t *next;
    void *obj;      // the object holder
    size_t size;    // the object size
    void (* destroy)(void **obj, size_t *size); // the wrapper to destroy the obj object.
};

enum chain_error {
    CHAIN_NO_ERROR = 0,
    CHAIN_ERR_NULL_NODE,
};

// append new obj at tail
#define chain_append_obj(chain, obj, destroyMethod) do {                                      \
    if (!chain) chain = chainnode_create(NULL, NULL, &obj, sizeof(obj), destroyMethod);       \
    else chainnode_create(chain_find_tail(chain), NULL, &obj, sizeof(obj), destroyMethod);    \
} while (0)
#define chain_append_ptr(chain, ptr, size, destroyMethod) do {                        \
    if (!chain) chain = chainnode_create(NULL, NULL, ptr, size, destroyMethod);       \
    else chainnode_create(chain_find_tail(chain), NULL, ptr, size, destroyMethod);    \
} while (0)

// insert new obj at head
#define chain_insert_obj(chain, obj, destroyMethod) do {                                         \
    if (!chain) chain = chainnode_create(NULL, NULL, &obj, sizeof(obj), destroyMethod);          \
    else chain = chainnode_create(NULL, chain_find_head(chain), &obj, sizeof(obj), destroyMethod);\
} while (0)
#define chain_insert_ptr(chain, ptr, size, destroyMethod) do {                             \
    if (!chain) chain = chainnode_create(NULL, NULL, ptr, size, destroyMethod);            \
    else chain = chainnode_create(NULL, chain_find_head(chain), ptr, size, destroyMethod); \
} while (0)


//int chain_append_ptr(struct chain_t *chain, const void* obj, const size_t size);

// trim the head obj
#define chain_trim(chain) do {                                  \
    if (chain) {                                                \
        if (chain_find_tail(chain) == chain_find_head(chain)) { \
            chainnode_destroy(chain_find_head(chain));          \
            chain = NULL;                                       \
        }                                                       \
        else {                                                  \
            chain = chain->next;                                \
            chainnode_destroy(chain_find_head(chain));          \
        }                                                       \
    }                                                           \
} while (0)

// trunc the tail obj
#define chain_trunc(chain) do {                         \
    if (chain) {                                        \
        if (chain == chain_find_tail(chain)) {          \
            chainnode_destroy(chain);                   \
            chain = NULL;                               \
        }                                               \
        else {                                          \
            chainnode_destroy(chain_find_tail(chain));  \
        }                                               \
    }                                                   \
} while (0)

#define chain_conv(chain, type) \
    (sizeof(type) != chain->size ? NULL : (type*)(chain->obj))

struct chain_t* chain_find_head(struct chain_t *chain);
struct chain_t* chain_find_tail(struct chain_t *chain);

// find the first obj which matches the condition func.
struct chain_t* chain_find_condition(struct chain_t *chain, bool (*condition)
        (const struct chain_t* chain, const void* cmp, const size_t cmp_s),
        const void* cmp, const size_t cmp_s);

struct chain_t* chainnode_create(struct chain_t* prev, struct chain_t* next,
                                 const void* obj, const size_t size,
                                 void (* destroy)(void **obj, size_t *size));
int chainnode_destroy(struct chain_t* node);

#ifdef __cplusplus
extern }
#endif

#endif /* ifndef ZCCHEN_CLIBS__CHAIN_H */
