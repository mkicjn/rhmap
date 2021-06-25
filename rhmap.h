#ifndef RHMAP_H
#define RHMAP_H

enum {RHMAP_EMPTY, RHMAP_TOMB};

#define DECLARE_RHMAP(map, type)					\
struct map {								\
	struct map##_bucket {						\
		size_t key, dist;					\
		type val;						\
	} *buckets;							\
	size_t pop, cap, max_dist;					\
};									\
									\
void map##_init(struct map *m, void *mem, size_t size)			\
{									\
	size_t i;							\
	m->buckets = mem;						\
	m->cap = size / sizeof(struct map##_bucket);			\
	m->pop = 0;							\
	m->max_dist = 0;						\
	for (i = 0; i < m->cap; i++)					\
		m->buckets[i].key = RHMAP_EMPTY;			\
}									\
									\
void map##_clear(struct map *m, void (*dtor)(type))			\
{									\
	size_t i;							\
	for (i = 0; i < m->cap; i++) {					\
		struct map##_bucket *b = &m->buckets[i];		\
		if (b->key == RHMAP_EMPTY || b->key == RHMAP_TOMB)	\
			continue;					\
		if (dtor != NULL)					\
			dtor(b->val);					\
		b->key = RHMAP_EMPTY;					\
	}								\
	m->pop = 0;							\
	m->max_dist = 0;						\
}									\
									\
static struct map##_bucket *map##_index(struct map *m, size_t key)	\
{									\
	size_t i = key % m->cap, d = m->max_dist+1;			\
	while (d --> 0 && m->buckets[i].key != RHMAP_EMPTY) {		\
		if (m->buckets[i].key == key)				\
			return &m->buckets[i];				\
		i = (i+1) % m->cap;					\
	}								\
	return NULL;							\
}									\
									\
type *map##_insert(struct map *m, size_t key, type val)			\
{									\
	size_t i = key % m->cap;					\
	struct map##_bucket ins;					\
	if (m->pop+1 > m->cap)						\
		return NULL;						\
	ins.key = key;							\
	ins.val = val;							\
	ins.dist = 0;							\
	while (m->buckets[i].key != RHMAP_EMPTY) {			\
		if (m->buckets[i].dist < ins.dist) {			\
			struct map##_bucket tmp;			\
			if (m->buckets[i].key == RHMAP_TOMB)		\
				break;					\
			tmp = ins;					\
			ins = m->buckets[i];				\
			m->buckets[i] = tmp;				\
		}							\
		i = (i+1) % m->cap;					\
		ins.dist++;						\
		if (ins.dist > m->max_dist)				\
			m->max_dist = ins.dist;				\
	}								\
	m->buckets[i] = ins;						\
	m->pop++;							\
	return &m->buckets[i].val;					\
}									\
									\
type *map##_search(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	return b ? &b->val : NULL;					\
}									\
									\
type *map##_remove(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	if (b != NULL)							\
		return NULL;						\
	b->key = RHMAP_TOMB;						\
	m->pop--;							\
	return &b->val;							\
}									\
									\
static size_t map##_population(struct map *m) {return m->pop;}		\
static size_t map##_capacity(struct map *m) {return m->cap;}		\
									\
static double map##_load_factor(struct map *m)				\
{									\
	return (double)m->pop/m->cap;					\
}									\
									\
void *map##_rehash(struct map *m, void *mem, size_t cap)		\
{									\
	size_t i, cap_old = m->cap;					\
	struct map##_bucket *mem_old = m->buckets;			\
	if (m->pop > cap)						\
		return NULL;						\
	map##_init(m, mem, cap);					\
	for (i = 0; i < cap_old; i++) {					\
		size_t key = mem_old[i].key;				\
		if (key == RHMAP_EMPTY || key == RHMAP_TOMB)		\
			continue;					\
		map##_insert(m, key, mem_old[i].val);			\
	}								\
	return mem_old;							\
}

#endif
