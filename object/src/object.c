#include "object.h"

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
    object_purge(obj);
    free(obj);
}

int object_init(struct object_t *obj)
{
    if (!obj) {
        return OBJECT_ERR_NULL_POINTER;
    }
    obj->instance = NULL;
    obj->size = 0;
    obj->purge = NULL;
    return OBJECT_OK;
}

int object_purge(struct object_t *obj)
{
    if (!obj) {
        return OBJECT_ERR_NULL_POINTER;
    }
    if (obj->purge) {
        return obj->purge(obj->instance, obj->size);
    }
    return OBJECT_OK;
}

int object_set_pointer(struct object_t *obj, void* instance, const size_t size,
                       int (* purge)(void*, size_t))
{
    if (!obj) {
        return OBJECT_ERR_NULL_POINTER;
    }
    obj->instance = instance;
    obj->size = size;
    if (purge) {
        obj->purge = purge;
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

const size_t object_get_size(struct object_t *obj)
{
    return obj->size;
}
