#ifndef RHMAP_H
#define RHMAP_H

enum {RHMAP_EMPTY, RHMAP_TOMB};

#define DECLARE_RHMAP(map, type)					\
struct map {								\
	struct map##_bucket {						\
		size_t key, distance;					\
		type value;						\
	} *buckets;							\
	size_t population, capacity, max_distance;			\
};									\
									\
void map##_init(struct map *m, void *mem, size_t size)			\
{									\
	size_t i;							\
	m->buckets = mem;						\
	m->capacity = size / sizeof(struct map##_bucket);		\
	m->population = 0;						\
	m->max_distance = 0;						\
	for (i = 0; i < m->capacity; i++)				\
		m->buckets[i].key = RHMAP_EMPTY;			\
}									\
									\
void map##_clear(struct map *m, void (*dtor)(type))			\
{									\
	size_t i;							\
	for (i = 0; i < m->capacity; i++) {				\
		struct map##_bucket *b = &m->buckets[i];		\
		if (b->key == RHMAP_EMPTY || b->key == RHMAP_TOMB)	\
			continue;					\
		if (dtor != NULL)					\
			dtor(b->value);					\
		b->key = RHMAP_EMPTY;					\
	}								\
	m->population = 0;						\
	m->max_distance = 0;						\
}									\
									\
static struct map##_bucket *map##_index(struct map *m, size_t key)	\
{									\
	size_t i = key % m->capacity, d = m->max_distance+1;		\
	while (d --> 0 && m->buckets[i].key != RHMAP_EMPTY) {		\
		if (m->buckets[i].key == key)				\
			return &m->buckets[i];				\
		i = (i+1) % m->capacity;				\
	}								\
	return NULL;							\
}									\
									\
type *map##_insert(struct map *m, size_t key, type val)			\
{									\
	size_t i = key % m->capacity;					\
	struct map##_bucket ins;					\
	if (m->population+1 > m->capacity)				\
		return NULL;						\
	ins.key = key;							\
	ins.value = val;						\
	ins.distance = 0;						\
	while (m->buckets[i].key != RHMAP_EMPTY) {			\
		if (m->buckets[i].distance < ins.distance) {		\
			struct map##_bucket tmp;			\
			if (m->buckets[i].key == RHMAP_TOMB)		\
				break;					\
			tmp = ins;					\
			ins = m->buckets[i];				\
			m->buckets[i] = tmp;				\
		}							\
		i = (i+1) % m->capacity;				\
		ins.distance++;						\
		if (ins.distance > m->max_distance)			\
			m->max_distance = ins.distance;			\
	}								\
	m->buckets[i] = ins;						\
	m->population++;						\
	return &m->buckets[i].value;					\
}									\
									\
type *map##_search(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	return b ? &b->value : NULL;					\
}									\
									\
type *map##_remove(struct map *m, size_t key)				\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	if (b == NULL)							\
		return NULL;						\
	b->key = RHMAP_TOMB;						\
	m->population--;						\
	return &b->value;						\
}									\
									\
void *map##_rehash(struct map *m, void *mem, size_t cap)		\
{									\
	size_t i, cap_old = m->capacity;				\
	struct map##_bucket *mem_old = m->buckets;			\
	if (m->population > cap)					\
		return NULL;						\
	map##_init(m, mem, cap);					\
	for (i = 0; i < cap_old; i++) {					\
		size_t key = mem_old[i].key;				\
		if (key == RHMAP_EMPTY || key == RHMAP_TOMB)		\
			continue;					\
		map##_insert(m, key, mem_old[i].value);			\
	}								\
	return mem_old;							\
}

#endif
