# RHMap

RHMap is a full-featured, single-header-only, generic hash table with Robin Hood hashing, in only 112 lines of ANSI C.

Its primary focus is on flexibility, which is evident in several aspects of its design:

* The data type of the hash table's values is not prescribed.
  * The user is not stuck with the ugly `void *`, or with the limited solution of doing `#define VAL_TYPE whatever` before the `#include`.
* All memory is to be provided and managed by the caller.
  * The user is not stuck with dynamic memory, and can stick to statically allocated memory on embedded platforms.
* A hash function is intentionally left unprovided.
  * The user is not stuck with string hashing, and can perform lookups with any data type by using whatever hash function they want.
* It is compatible with ANSI C, i.e. C89.
  * The user is not stuck relying on modern compiler features like `typeof`, and can use older standards on more limited platforms.
* A function for rehashing tables is provided, but never used automatically.
  * The user is not stuck with the scheme the library author likes best, and can resize their hash tables up or down with arbitrary criteria.

It is also very simple to use, because it is designed to have consistent usage semantics and a descriptive naming convention.

Furthermore, it should be rather efficient (in time and space) because Robin Hood hashing results in excellent worst case probe counts, even under high load.
In fact, this whole project is inspired by [this excellent article on Robin Hood hashing](https://www.sebastiansylvan.com/post/robin-hood-hashing-should-be-your-default-hash-table-implementation/) which explains further.

## Usage

To set up, simply `#include "rhmap.h"` and declare the table's type with `DECLARE_RHMAP(name, type)`.

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

    void name##_init(struct name *m, void *mem, size_t cap);
    void name##_clear(struct name *m, void (*dtor)(type));
    type *name##_insert(struct name *m, size_t key, type val);
    type *name##_search(struct name *m, size_t key);
    type *name##_remove(struct name *m, size_t key);
    void *name##_rehash(struct name *m, void *mem, size_t cap);

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

## Examples

The included test files demonstrate different uses of the hash table.
* `test1.c` demonstrates its use with constant size, static memory, and simple data types.
* `test2.c` demonstrates its use with automatic resizing, dynamic memory, and complex data types.

## To-do List

* Benchmark
