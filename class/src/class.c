#include "class.h"

#include <string.h>
#include <stdlib.h>

#include <stdio.h>

struct class_t* class_create()
{
    struct class_t* ret = malloc(sizeof(struct class_t));
    class_init(ret);
    return ret;
}

void class_destroy(struct class_t *cls)
{
    if (cls) {
        class_call_func_clean(cls);
    }
    struct class_t *walk_cls = cls;
    while (walk_cls) {
        struct class_t *ptr = walk_cls;
        walk_cls = walk_cls->parent;
        free(ptr);
    }
}

int class_init(struct class_t *cls)
{
    cls->status.byte = 0x00;
    cls->obj = NULL;
    cls->size = 0;
    cls->parent = NULL;
    cls->methods.base.setup = NULL;
    cls->methods.base.clean = NULL;
    for (int j = 0; j < CLASS_MAX_USER_METHODS; ++j) {
        cls->methods.user.func[j] = NULL;
    }
    return CLASS_OK;
}

// set the outest level
int class_set_obj(struct class_t *cls, void* obj, const size_t size)
{
    cls->status.bits.lock = 1;
    cls->obj = obj;
    cls->size = size;
    cls->status.bits.lock = 0;
    return CLASS_OK;
}

int class_set_func_base(struct class_t *cls,
                        int (* setup)(void*, const size_t), int (* clean)(void*, const size_t))
{
    cls->status.bits.lock = 1;
    cls->methods.base.setup = setup;
    cls->methods.base.clean = clean;
    cls->status.bits.lock = 0;
    return CLASS_OK;
}

int class_set_func_user(struct class_t *cls, const size_t func_id,
                        int (* func)(void*, const size_t, void*, const size_t))
{
    if (func_id >= CLASS_MAX_USER_METHODS) {
        return CLASS_ERR_OVER_MAX_USER_METHODS;
    }
    cls->status.bits.lock = 1;
    cls->methods.user.func[func_id] = func;
    cls->status.bits.lock = 0;
    return CLASS_OK;
}

int class_fork_list(struct class_t *base, struct class_t *fork_list, const size_t list_size)
{
    const size_t base_level = class_get_level(base);
    if (base_level + 1 > list_size) {
        return CLASS_ERR_MEMORY_HAVE_NOT_ALLOCATED;
    }
    for (int i = 0; i < list_size; ++i) {
        class_init(&fork_list[i]);  // init all
    }
    struct class_t *walk_base = base;
    struct class_t *walk_fork = &fork_list[0];
    for (int i = 1; i < list_size && walk_base; ++i) {
        walk_fork->parent = &fork_list[i];
        memcpy(walk_fork->parent, walk_base, sizeof(struct class_t));
        walk_fork = walk_fork->parent;
        walk_base = walk_base->parent;
    }
    return CLASS_OK;
}

int class_fork_chain(struct class_t *base, struct class_t *fork)
{
    class_init(fork);       // init the new working obj
    struct class_t *walk_fork = fork;
    struct class_t *walk_base = base;
    while (walk_base) {
        // prepare the parent object for base's last node.*/
        walk_fork->parent = class_create();
        memcpy(walk_fork->parent, walk_base, sizeof(struct class_t));
        walk_fork = walk_fork->parent;
        walk_base = walk_base->parent;
    }
    return CLASS_OK;
}

// call the function from inside to outside
int class_call_func_setup(struct class_t *cls)
{
    struct class_t *walk_cls = cls;
    while (walk_cls) {
        int ret = 0;
        if (walk_cls->methods.base.setup) {
            ret = walk_cls->methods.base.setup(&walk_cls->obj, walk_cls->size);
            if (ret) {
                return ret;
            }
        }
        walk_cls = walk_cls->parent;
    }
    return CLASS_OK;
}

int class_call_func_clean(struct class_t *cls)
{
    struct class_t *walk_cls = cls;
    while (walk_cls) {
        int ret = 0;
        if (walk_cls->methods.base.clean) {
            ret = walk_cls->methods.base.clean(&walk_cls->obj, walk_cls->size);
            if (ret) {
                return ret;
            }
        }
        walk_cls = walk_cls->parent;
    }
    return CLASS_OK;
}

int class_call_func_user(struct class_t *cls, const size_t id,
                         void* param, const size_t param_size)
{
    if (id >= CLASS_MAX_USER_METHODS) {
        return CLASS_ERR_OVER_MAX_USER_METHODS;
    }
    struct class_t *walk_cls = cls;
    while (walk_cls) {
        int ret = 0;
        if (walk_cls->methods.user.func[id]) {
            ret = walk_cls->methods.user.func[id](
                    &walk_cls->obj, walk_cls->size, param, param_size
            );
            if (ret) {
                return ret;
            }
        }
        walk_cls = walk_cls->parent;
    }
    return CLASS_OK;
}

const size_t class_get_level(struct class_t *cls)
{
    size_t ret = 0;
    struct class_t *walk_cls = cls;
    while (walk_cls) {
        ret ++;
        walk_cls = walk_cls->parent;
    }
    return ret;
}

struct class_t* class_get_parent(struct class_t *cls)
{
    return cls->parent;
}

void* class_get_obj_instance(struct class_t *cls)
{
    return cls->obj;
}

size_t class_get_obj_size(struct class_t *cls)
{
    return cls->size;
}

