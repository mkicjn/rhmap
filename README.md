# RHMap

RHMap is a full-featured, single-header-only, generic hash table with Robin Hood hashing. All in a little bit of ANSI C.

It comes in two flavors: the [original flexibility-oriented interface](#original_usage) and the [new simplified interface](#simplified_usage).

The primary design goal of this project is flexibility, which is evident in several aspects of the original interface design:

* The data type of the hash table's values is not prescribed.
  * The user is not stuck with the ugly `void *`, or with the hack solution of placing `#define VAL_TYPE whatever` before the `#include`.
* All memory in use by the table can be provided and managed manually.
  * The user is not stuck with dynamic memory, and can easily use statically allocated memory on embedded platforms, or even utilize multiple strategies *in the same codebase*.
* A hash function is intentionally left unprovided.
  * The user is not stuck with string hashing, and can perform lookups with any data type by using whatever hash function they want.
* It is compatible with ANSI C, i.e. C89.
  * The user is not stuck relying on modern compiler features like `typeof`, and can use older standards on more limited platforms.
* A function for rehashing tables is provided, but not used automatically (except by the simplified interface).
  * The user is not stuck with the scheme the library author likes best, and can resize their hash tables how and when they want.

Ease of Use may be considered the secondary design goal:

* The library is designed to have consistent usage semantics and a descriptive naming convention.
* A cleanup function is provided to aid in manual memory management using the original interface.
* A simplified interface is provided for general use which automatically resizes the table and manages value memory.

Furthermore, it should be rather efficient (in time and space) because Robin Hood hashing results in excellent worst case probe counts, even under high load.
In fact, this whole project is inspired by [this excellent article on Robin Hood hashing](https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/) which explains further.

## <a name="original_usage"></a>Usage (Original Interface)

To set up, simply `#include "rhmap.h"` and declare the table's type using the macro `RHMAP_DECLARE(name, type)`.

`struct name` will be defined as follows to represent the hash table:

    struct name {
            struct name##_bucket {
                    size_t key, distance;
                    type value;
            } *buckets;
            size_t population, capacity, max_distance;
    };

NONE of these fields should ever be *written* to directly by the user, but they may be useful to read sometimes.
Unfortunately, ANSI C provides no clean way to enforce this.

Take care to manipulate the structure only by using the provided functions:

    void name##_init(struct name *m, void *mem, size_t size);
    void name##_clear(struct name *m, void (*dtor)(type));
    type *name##_insert(struct name *m, size_t key, type val);
    type *name##_search(struct name *m, size_t key);
    type *name##_remove(struct name *m, size_t key);
    void *name##_rehash(struct name *m, void *mem, size_t size);

---

* The *init* function initializes the struct's members.
The `size` argument is in bytes so that the usage of `sizeof` on static arrays is appropriate.

* The *clear* function empties the table.
To simplify cleanup, it takes a function pointer to a destructor, which (if not `NULL`) will be called on every value in the table.

* The self-explanatory *insert,* *search,* and *remove* functions return a pointer to the value's location in memory, or `NULL` on error.

* The *rehash* function reinitializes the table to a new memory area, and re-inserts all previous entries.
It returns a pointer to the old memory area, or `NULL` on error.

---

Functions return a pointer to the value instead of the value itself for two reasons:
1. This approach avoids the need for separate *get* and *set* functions, and remove doesn't need to manage entry memory.
2. There is a consistent way to signal errors, i.e. differentiate between `NULL` as an error and `NULL` as a value.

However, there is one limitation: pointers returned by these functions should *not* be used after any insertions, because insertions may reorder table contents.

## <a name="simplified_usage"></a>Usage (Simplified Interface)

Where the main interface aims to maximize flexibility, this interface aims to maximize convenience. Thus, this adaptation makes some assumptions, but greatly simplifies casual use. To remain generic, you will still need to supply a hash function and destructor (albeit only once). Everything else is managed automatically, including table resizing and freeing value memory.

To set up, simply `#include "ezrhmap.h"` and declare the table's type using the macro `EZRHMAP_DECLARE(name, hash_function, key_t, val_t, destructor)`.

`hash_function` should be `size_t (*)(key_t)` and `destructor` should be `void (*)(val_t)` or `NULL`.

The following functions are provided:

    struct name *name##_create(void);
    void name##_destroy(struct name *m);
    val *name##_set(struct name *m, key_t k, val_t v);
    val *name##_get(struct name *m, key_t k);
    val *name##_del(struct name *m, key_t k);

All of these functions should be self-explanatory. Note that pointers are returned, with `NULL` indicating an error (e.g. key not found). In practice, the return values for `name##_set` and `name##_del` can be ignored.

## Examples

The included test files demonstrate different uses of the hash table.
* `test1.c` demonstrates its use with constant size, static memory, and simple data types.
* `test2.c` demonstrates its use with automatic resizing, dynamic memory, and complex data types.
* `test2b.c` behaves similarly to `test2.c` but is meant to demonstrate the simplified interface.
