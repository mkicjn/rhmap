#ifndef RHMAP_H
#define RHMAP_H

enum {UNUSED, TOMBSTONE};

#define DECLARE_RHMAP(map, type)					\
struct map {								\
	struct map##_bucket {						\
		size_t key;						\
		size_t dist;						\
		type val;						\
	} *buckets;							\
	size_t size;							\
	size_t pop;							\
	size_t max_dist;						\
};									\
									\
void map##_init(struct map *m, void *mem, int size);			\
void map##_clear(struct map *m, void (*dtor)(type));			\
type *map##_insert(struct map *m, size_t key, type val);		\
type *map##_remove(struct map *m, size_t key);				\
type *map##_search(struct map *m, size_t key);				\
									\
void map##_init(struct map *m, void *mem, int size)			\
{									\
	size_t i;							\
	m->buckets = mem;						\
	m->size = size / sizeof(struct map##_bucket);			\
	m->pop = 0;							\
	m->max_dist = 0;						\
	for (i = 0; i < m->size; i++)					\
		m->buckets[i].key = UNUSED;				\
}									\
									\
void map##_clear(struct map *m, void (*dtor)(type))			\
{									\
	size_t i;							\
	for (i = 0; i < m->size; i++) {					\
		struct map##_bucket *b = &m->buckets[i];		\
		if (b->key == UNUSED || b->key == TOMBSTONE)		\
			continue;					\
		if (dtor != NULL)					\
			dtor(b->val);					\
		b->key = UNUSED;					\
	}								\
	m->pop = 0;							\
	m->max_dist = 0;						\
}									\
									\
static struct map##_bucket *map##_index(struct map *m, size_t key)	\
{									\
	size_t i = key % m->size;					\
	size_t d = m->max_dist+1;					\
	while (d --> 0 && m->buckets[i].key != UNUSED) {		\
		if (m->buckets[i].key == key)				\
			return &m->buckets[i];				\
		i = (i+1) % m->size;					\
	}								\
	return NULL;							\
}									\
									\
type *map##_insert(struct map *m, size_t key, type val)			\
{									\
	size_t i = key % m->size;					\
	struct map##_bucket ins;					\
	if (m->pop+1 > m->size)						\
		return NULL;						\
	ins.key = key;							\
	ins.val = val;							\
	ins.dist = 0;							\
	while (m->buckets[i].key != UNUSED) {				\
		if (m->buckets[i].dist < ins.dist) {			\
			struct map##_bucket tmp;			\
			if (m->buckets[i].key == TOMBSTONE)		\
				break;					\
			tmp = ins;					\
			ins = m->buckets[i];				\
			m->buckets[i] = tmp;				\
		}							\
		i = (i+1) % m->size;					\
		ins.dist++;						\
			if (ins.dist > m->max_dist)			\
				m->max_dist = ins.dist;			\
	}								\
	m->buckets[i] = ins;						\
	m->pop++;							\
	return &m->buckets[i].val;					\
}									\
									\
type *map##_remove(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	if (b != NULL) {						\
		b->key = TOMBSTONE;					\
		m->pop--;						\
		return &b->val;						\
	}								\
	return NULL;							\
}									\
									\
type *map##_search(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	if (b != NULL)							\
		return &b->val;						\
	return NULL;							\
}

#endif
