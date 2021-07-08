#include "chain.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

struct chain_t* chainnode_create(struct chain_t* prev, struct chain_t* next,
                                 const void* obj, const size_t size)
{
    struct chain_t *ret = malloc(sizeof(struct chain_t));
    ret->next = next;
    ret->prev = prev;
    ret->obj = malloc(size);
    ret->size = size;
    if (!memcpy(ret->obj, obj, size)) { // memcpy failed
        free(ret->obj);
        free(ret);
        return NULL;
    };
    if (prev) {
        prev->next = ret;
    }
    if (next) {
        next->prev = ret;
    }
    return ret;
}

int chainnode_destroy(struct chain_t* node)
{
    if (!node) {
        return 0;       // empty node, already destroyed.
    }
    if (node->next) {
        node->next->prev = node->prev;
    }
    if (node->prev) {
        node->prev->next = node->next;
    }
    free(node->obj);
    node->size = 0;
    free(node);
    node = NULL;
    return CHAIN_NO_ERROR;
}

struct chain_t* chain_find_head(struct chain_t *chain)
{
    if (!chain) {
        return NULL;
    }
    while (chain->prev) {
        chain = chain->prev;
    }
    return chain;
}

struct chain_t* chain_find_tail(struct chain_t *chain)
{
    if (!chain) {
        return NULL;
    }
    while (chain->next) {
        chain = chain->next;
    }
    return chain;
}

struct chain_t* chain_find_condition(struct chain_t *chain,
                                     bool (*condition)(const void* obj, const size_t size))
{
    if (!chain) {
        return NULL;
    }
    chain = chain_find_head(chain);
    while (chain->next) {
        if (condition(chain->obj, chain->size)) {
            break;
        }
        chain = chain->next;
    }
    return chain;
}
