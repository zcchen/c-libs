#include "object.h"

#include <stdlib.h>
#include <string.h>

struct object_t* object_create()
{
    struct object_t *ret = malloc(sizeof(struct object_t));
    if (object_init(ret)) {
        free(ret);
        return NULL;
    }
    return ret;
}

void object_destroy(struct object_t *obj)
{
    if (!obj) {
        return;
    }
    if (obj->destroy) {
        obj->destroy(&obj->instance);
        obj->destroy = NULL;
    }
    if (obj->instance) {
        free(obj->instance);
        obj->instance = NULL;
        obj->size = 0;
    }
    free(obj);
}

int object_init(struct object_t *obj)
{
    if (!obj) {
        return OBJECT_ERR_NULL_POINTER;
    }
    obj->instance = NULL;
    obj->size = 0;
    obj->destroy = NULL;
    return OBJECT_OK;
}

int object_set_pointer(struct object_t *obj, void* instance, const size_t size,
                       void (* destroy)(void** instance))
{
    if (!obj) {
        return OBJECT_ERR_NULL_POINTER;
    }
    if (size) {
        obj->instance = malloc(size);
        memcpy(obj->instance, instance, size);
        obj->size = size;
    }
    else {
        obj->instance = NULL;
        obj->size = 0;
    }
    if (destroy) {
        obj->destroy = destroy;
    }
    return OBJECT_OK;
}

void* object_get_instance(struct object_t *obj)
{
    if (!obj) {
        return NULL;
    }
    return obj->instance;
}

size_t object_get_size(struct object_t *obj)
{
    return obj->size;
}
