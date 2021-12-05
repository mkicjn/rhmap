#ifndef EZRHMAP_H
#define EZRHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include "rhmap.h"

/*
 *	Where the main header aims to maximize flexibility, this header aims to maximize convenience.
 *	Thus, this header makes some assumptions, but greatly simplifies casual use.
 *
 *	It does remain generic, meaning you will still need to supply a hash function and destructor.
 *	However, everything else is managed automatically!
 *
 *	The following functions are provided in addition to the usual:
 *
 *		struct map *map_create(void);
 *		void map_destroy(struct map *m);
 *
 *		val *map_set(struct map *m, key k, val v);
 *		val *map_get(struct map *m, key k);
 *		val *map_del(struct map *m, key k);
 *
 *	TODO: Integrate this info into the README somehow.
 */


#ifndef EZRHMAP_MAX_LOAD
#define EZRHMAP_MAX_LOAD 0.9
#endif

#ifndef EZRHMAP_GROWTH_FN
#define EZRHMAP_GROWTH_FN(x) (x*2+1)
#endif

#define EZRHMAP_DECLARE(map, hash, key, val, dtor)			\
									\
RHMAP_DECLARE(map, val)							\
									\
struct map *map##_create(void)						\
{									\
	struct map *m = malloc(sizeof(*m));				\
	struct map##_bucket *b = malloc(sizeof(*b));			\
	map##_init(m, b, sizeof(*b));					\
	return m;							\
}									\
									\
void map##_destroy(struct map *m)					\
{									\
	map##_clear(m, dtor);						\
	free(m->buckets);						\
	free(m);							\
}									\
									\
val *map##_set(struct map *m, key k, val v)				\
{									\
	size_t h = hash(k);						\
	if (m->population >= EZRHMAP_MAX_LOAD * m->capacity) {		\
		size_t new_cap = EZRHMAP_GROWTH_FN(m->capacity);	\
		struct map##_bucket *b = malloc(new_cap * sizeof(*b));	\
		free(map##_rehash(m, b, new_cap * sizeof(*b)));		\
	}								\
	val *p = map##_search(m, h);					\
	if (p == NULL) { 						\
		p = map##_insert(m, h, v);				\
	} else {							\
		if (dtor != NULL)					\
			dtor(*p);					\
		*p = v;							\
	}								\
	return p;							\
}									\
									\
val *map##_get(struct map *m, key k)					\
{									\
	return map##_search(m, hash(k));				\
}									\
									\
val *map##_del(struct map *m, key k)					\
{									\
	val *v = map##_remove(m, hash(k));				\
	if (v != NULL)							\
		dtor(*v);						\
	return v;							\
}

#endif
