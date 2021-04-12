# RHMap

RHMap is a one file C implementation of a generic hash table using robin hood hashing.

Its primary focus is on flexibility, which is evident in several aspects of its design:

* The data type of the hash table's values is not prescribed.
  * The user is not stuck with the ugly `void *`, or with the limited solution of doing `#define VAL_TYPE whatever` before the `#include`.
* All memory is to be provided and managed by the caller.
  * The user is not stuck with dynamic memory, and can stick to statically allocated memory on embedded platforms.
* A hash function is intentionally left unprovided.
  * The user is not stuck with string hashing, and can perform lookups by any data type with whatever hash function they want.
* It is compatible with ANSI C, and C89.
  * The user is not stuck relying on modern compiler features like `typeof`, and can use older standards on more limited platforms.

It is also very simple to use, with consistent functions and data types which are essentially opaque.

Even memory management is made simple;
the remove function returns a pointer to the old value (which is not immediately erased), and another function is provided to simplify cleanup.

## Usage

To use, `#include "rhmap.h"` and declare the table's type with `DECLARE_RHMAP(name, type)`.

`struct name` will be defined to represent the hash table's type.

Likewise, several new functions will be defined to manipulate it:

    void name##_init(struct name *m, void *mem, size_t size);
    void name##_clear(struct name *m, void (*dtor)(type));
    type *name##_insert(struct name *m, size_t key, type val);
    type *name##_search(struct name *m, size_t key);
    type *name##_remove(struct name *m, size_t key);
    
The *init* function initializes the struct's members.
The `size` argument is in bytes so that the usage of `sizeof` on static arrays is appropriate.

The *insert,* *search,* and *remove* functions return a pointer to the value's location in memory, or `NULL` on error.

The *clear* function empties the table.
To simplify cleanup, it takes a function pointer to a destructor, which (if not `NULL`) will be called on every value in the table.

## Examples

The included test files demonstrate different uses of the hash table.
* `test1.c` demonstrates its use with static memory and simple data types.
* `test2.c` demonstrates its use with dynamic memory and complex data types.

## To-do List

* Resize function
* Benchmark
