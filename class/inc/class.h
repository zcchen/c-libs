#ifndef CTOOLINGS_CLASS_H
#define CTOOLINGS_CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define CLASS_MAX_INHERIT_LEVEL 4
#define CLASS_MAX_USER_METHODS  16

struct class_t {
    union {
        uint8_t byte;
        struct {
            uint8_t lock: 1;
        } bits;
    } status;
    void *obj[CLASS_MAX_INHERIT_LEVEL];
    size_t size[CLASS_MAX_INHERIT_LEVEL];
    size_t inherit_level;
    struct {
        struct {
            int (* setup)(void* self, const size_t self_size);    // constructor
            int (* clean)(void* self, const size_t self_size);    // deconstructor
        } base[CLASS_MAX_INHERIT_LEVEL];
        struct {
            int (* func[CLASS_MAX_USER_METHODS])(void* self, const size_t self_size,
                                                 void* param, const size_t param_size);
        } user[CLASS_MAX_INHERIT_LEVEL];
        size_t func_used;
    } methods;
};

enum class_err_t {
    CLASS_OK = 0,
    CLASS_ERR_UNKNOWN = 1,
    CLASS_ERR_OVER_MAX_INHERIT_LEVEL,
    CLASS_ERR_OVER_MAX_USER_METHODS,
    CLASS_ERR_NULL_POINTER,
};

int class_init(struct class_t *cls);

// set the outest level
int class_set_obj(struct class_t *cls, void* obj, const size_t size);
int class_set_func_base(struct class_t *cls,
                        int (* setup)(void*, const size_t), int (* clean)(void*, const size_t));
int class_set_func_fork(struct class_t *cls, const size_t func_id,
                        int (* func)(void*, const size_t, void*, const size_t));
// finish the current level setup, go to next;
int class_set_next(struct class_t *cls);

// copy current from base to fork
int class_fork(struct class_t *base, struct class_t *fork);

// call the function from inside to outside
int class_call_func_setup(struct class_t *cls);
int class_call_func_clean(struct class_t *cls);
int class_call_func_user(struct class_t *cls, const size_t id, void* param, const size_t param_size);

// get the max level which is already inherit.
const size_t class_get_maxlevel(struct class_t *cls);

// get and convert the specified level objects
#define class_get_obj(cls, level, type)                 \
    (class_get_obj_size(cls, level) == sizeof(type) ?   \
            (type*)(class_get_obj_instance(cls, level)) : NULL)


// NOTE: usually don't use the following methods.
void* class_get_obj_instance(struct class_t *cls, const size_t level);
size_t class_get_obj_size(struct class_t *cls, const size_t level);

#ifdef __cplusplus
extern }
#endif

#endif /* ifndef CTOOLINGS_CLASS_H */
