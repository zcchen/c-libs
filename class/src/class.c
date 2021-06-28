#include "class.h"

#include <string.h>

int class_init(struct class_t *cls)
{
    cls->status.byte = 0x00;
    for (int i = 0; i < CLASS_MAX_INHERIT_LEVEL; ++i) {
        cls->obj[i] = NULL;
        cls->size[i] = 0;
        cls->methods.base[i].setup = NULL;
        cls->methods.base[i].clean = NULL;
        for (int j = 0; j < CLASS_MAX_USER_METHODS; ++j) {
            cls->methods.user[i].func[j] = NULL;
        }
        cls->methods.func_used = 0;
    }
    return CLASS_OK;
}

// set the outest level
int class_set_obj(struct class_t *cls, void* obj, const size_t size)
{
    cls->obj[cls->inherit_level] = obj;
    cls->size[cls->inherit_level] = size;
    return CLASS_OK;
}

int class_set_func_base(struct class_t *cls,
                        int (* setup)(void*, const size_t), int (* clean)(void*, const size_t))
{
    cls->methods.base[cls->inherit_level].setup = setup;
    cls->methods.base[cls->inherit_level].clean = clean;
    return CLASS_OK;
}

int class_set_func_fork(struct class_t *cls, const size_t func_id,
                        int (* func)(void*, const size_t, void*, const size_t))
{
    if (func_id >= CLASS_MAX_USER_METHODS) {
        return CLASS_ERR_OVER_MAX_USER_METHODS;
    }
    cls->methods.user[cls->inherit_level].func[func_id] = func;
    return CLASS_OK;
}

int class_set_next(struct class_t *cls)
{
    if (cls->inherit_level + 1 < CLASS_MAX_INHERIT_LEVEL) {
        cls->inherit_level ++;
        return CLASS_OK;
    }
    else {
        return CLASS_ERR_OVER_MAX_INHERIT_LEVEL;
    }
}

// copy current from base to fork
int class_fork(struct class_t *base, struct class_t *fork)
{
    if (!base || !fork) {
        return CLASS_ERR_NULL_POINTER;
    }
    memcpy(fork, base, sizeof(struct class_t));
    return CLASS_OK;
}

// call the function from inside to outside
int class_call_func_setup(struct class_t *cls)
{
    for (int i = 0; i < cls->inherit_level; ++i) {
        int ret = 0;
        if (cls->methods.base[i].setup) {
            ret = cls->methods.base[i].setup(&cls->obj[i], cls->size[i]);
            if (ret) {
                return ret;
            }
        }
    }
    return CLASS_OK;
}

int class_call_func_clean(struct class_t *cls)
{
    for (int i = 0; i < cls->inherit_level; ++i) {
        int ret = 0;
        if (cls->methods.base[i].clean) {
            ret = cls->methods.base[i].clean(&cls->obj[i], cls->size[i]);
            if (ret) {
                return ret;
            }
        }
    }
    return CLASS_OK;
}

int class_call_func_user(struct class_t *cls, const size_t id,
                         void* param, const size_t param_size)
{
    for (int i = 0; i < cls->inherit_level; ++i) {
        int ret = 0;
        if (cls->methods.user[i].func[id]) {
            ret = cls->methods.user[i].func[id](&cls->obj[i], cls->size[i], param, param_size);
            if (ret) {
                return ret;
            }
        }
    }
    return CLASS_OK;
}

// get the max level which is already inherit.
const size_t class_get_maxlevel(struct class_t *cls)
{
    return cls->inherit_level;
}

void* class_get_obj_instance(struct class_t *cls, const size_t level)
{
    return cls->obj[level];
}

size_t class_get_obj_size(struct class_t *cls, const size_t level)
{
    return cls->size[level];
}

