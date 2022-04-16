#ifndef ZCCHEN_CLIBS__OBJECT_H
#define ZCCHEN_CLIBS__OBJECT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


enum object_err_t {
    OBJECT_OK = 0,
    OBJECT_ERR_UNKNOWN = 1,
    OBJECT_ERR_NULL_POINTER,
};

struct object_t {
    void* instance;
    size_t size;
    void (* destroy)(void** instance);
};

// memory operation functions
struct object_t* object_create();
void object_destroy(struct object_t *obj);

// do the default init operation
int object_init(struct object_t *obj);

// fill the data to `struct object_t`
// Please setup `free()` method to free the obj->instance if it is allocate by malloc.
#define object_set_instance(obj, instance, purge_func)  \
    ( &instance == NULL ?                                \
        object_set_pointer(obj, NULL, 0, purge_func) :  \
        object_set_pointer(obj, (void*)(&instance), sizeof(instance), purge_func) )
int object_set_pointer(struct object_t *obj, void* instance, const size_t size,
                       void (* destroy)(void** instance));

#define object_get(obj, type)   \
    (object_get_size(obj) == sizeof(type) ? (type*)object_get_instance(obj) : NULL)

// raw function for above macros
void* object_get_instance(struct object_t *obj);
size_t object_get_size(struct object_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* ifndef ZCCHEN_CLIBS__OBJECT_H */
