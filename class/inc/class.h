#ifndef ZCCHEN_CLIBS__CLASS_H
#define ZCCHEN_CLIBS__CLASS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//#define CLASS_MAX_INHERIT_LEVEL 4
#define CLASS_MAX_USER_METHODS  16

struct class_t {
    union {
        uint8_t byte;
        struct {
            uint8_t lock: 1;
        } bits;
    } status;
    struct class_t *parent;
    struct class_t *child;
    void *obj;
    size_t size;
    void (* destroy)(void **obj, size_t *size); // the wrapper to destroy the obj object.
    struct {
        struct {
            int (* setup)(void* self, const size_t self_size);    // constructor
            int (* clean)(void* self, const size_t self_size);    // deconstructor
        } base;
        struct {
            int (* func[CLASS_MAX_USER_METHODS])(void* self, const size_t self_size,
                                                 void* param, const size_t param_size);
        } user;
    } methods;
};

enum class_err_t {
    CLASS_OK = 0,
    CLASS_ERR_UNKNOWN = 1,
    CLASS_ERR_OVER_MAX_USER_METHODS,
    CLASS_ERR_NULL_POINTER,
    CLASS_ERR_MEMORY_HAVE_NOT_ALLOCATED,
};

// memory operation the the class struct
struct class_t* class_create();
void class_destroy(struct class_t *cls);

int class_init(struct class_t *cls);

// set the outest level
int class_set_obj(struct class_t *cls, void* obj, const size_t size,
                  void (* destroy)(void **obj, size_t *size));
int class_set_func_base(struct class_t *cls,
                        int (* setup)(void*, const size_t), int (* clean)(void*, const size_t));
int class_set_func_user(struct class_t *cls, const size_t func_id,
                        int (* func)(void*, const size_t, void*, const size_t));

// copy the `base` and its parent chain to the `fork_list`, new child at 0, parent is +1.
// list size should be have at least 1 more empty element for new fork.
int class_fork_list(struct class_t *base, void* new_obj_from_base,
                    struct class_t *fork_list, const size_t list_size);

// initialize the `fork` pointer and set its parent to base.
int class_fork_chain(struct class_t *base, void* new_obj_from_base, struct class_t *fork);

// call the function from inside to outside
int class_call_func_setup(struct class_t *cls);
int class_call_func_clean(struct class_t *cls);
int class_call_func_user(struct class_t *cls, const size_t id, const bool include_parent,
                         void* param, const size_t param_size);

// return the inherit level from this cls to top.
const size_t class_get_level(struct class_t *cls);

struct class_t* class_get_parent(struct class_t *cls);

// get and convert the specified level objects
#define class_get_obj(cls, type)    \
    (class_get_obj_size(cls) == sizeof(type) ? (type*)(class_get_obj_instance(cls)) : NULL)


// NOTE: usually don't use the following methods.
void* class_get_obj_instance(struct class_t *cls);
const size_t class_get_obj_size(struct class_t *cls);

#ifdef __cplusplus
extern }
#endif

#endif /* ifndef ZCCHEN_CLIBS__CLASS_H */
