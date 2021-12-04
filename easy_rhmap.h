#ifndef EASY_RHMAP_H
#define EASY_RHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include "rhmap.h"

/*
 *	This version makes assumptions about use case to maximize convenience.
 *	Namely, all memory is dynamically allocated and managed automatically.
 *
 *	The following functions are provided in addition to the usual:
 *
 *		struct name *name_create(void);
 *		void name_destroy(struct name *m, void (*dtor)(type));
 *		type *name_set(struct name *m, size_t key, type val);
 *		static inline type *name_get(struct name *m, size_t key);
 *		static inline type *name_del(struct name *m, size_t key);
 *
 *	TODO: Integrate this info into the README somehow.
 *	TODO: Consider having the user provide a hash function and to DECLARE
 */


#ifndef EASY_RHMAP_MAX_LOAD
#define EASY_RHMAP_MAX_LOAD 0.9
#endif

#ifndef EASY_RHMAP_GROWTH
#define EASY_RHMAP_GROWTH(x) (x*2+1)
#endif

#define EASY_RHMAP_DECLARE(map, type)					\
									\
RHMAP_DECLARE(map, type)						\
									\
struct map *map##_create(void)						\
{									\
	struct map *m = malloc(sizeof(*m));				\
	struct map##_bucket *b = malloc(sizeof(*b));			\
	map##_init(m, b, sizeof(*b));					\
	return m;							\
}									\
									\
void map##_destroy(struct map *m, void (*dtor)(type))			\
{									\
	map##_clear(m, dtor);						\
	free(m->buckets);						\
	free(m);							\
}									\
									\
type *map##_set(struct map *m, size_t key, type val)			\
{									\
	if (m->population >= EASY_RHMAP_MAX_LOAD * m->capacity) {	\
		size_t new_cap = EASY_RHMAP_GROWTH(m->capacity);	\
		struct map##_bucket *b = malloc(new_cap * sizeof(*b));	\
		void *mem = map##_rehash(m, b, new_cap * sizeof(*b));	\
		if (mem == NULL) {					\
			fputs(#map"_set: Failed to rehash\n", stderr);	\
			exit(1);					\
		}							\
		free(mem);						\
	}								\
	type *p = map##_search(m, key);					\
	if (p != NULL)							\
		*p = val;						\
	else								\
		p = map##_insert(m, key, val);				\
	return p;							\
}									\
									\
static inline type *map##_get(struct map *m, size_t key)		\
{									\
	return map##_search(m, key);					\
}									\
									\
static inline type *map##_del(struct map *m, size_t key)		\
{									\
	return map##_remove(m, key);					\
}

#endif
