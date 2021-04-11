#ifndef RHMAP_H
#define RHMAP_H

#ifndef RHMAP_KEY
#define RHMAP_KEY unsigned long long int
#endif

enum {UNUSED, TOMBSTONE};

#define DECLARE_RHMAP(map, type)					\
struct map {								\
	struct map##_bucket {						\
		RHMAP_KEY key;						\
		int dist;						\
		type val;						\
	} *buckets;							\
	int size;							\
	int pop;							\
	int max_dist;							\
};									\
									\
void map##_init(struct map *m, void *mem, int size);			\
void map##_clear(struct map *m, void (*dtor)(type));			\
type *map##_insert(struct map *m, RHMAP_KEY key, type val);		\
type *map##_remove(struct map *m, RHMAP_KEY key);			\
type *map##_search(struct map *m, RHMAP_KEY key);			\
									\
void map##_init(struct map *m, void *mem, int size)			\
{									\
	m->buckets = mem;						\
	m->size = size / sizeof(struct map##_bucket);			\
	m->pop = 0;							\
	m->max_dist = 0;						\
	for (int i = 0; i < m->size; i++)				\
		m->buckets[i].key = UNUSED;				\
}									\
									\
void map##_clear(struct map *m, void (*dtor)(type))			\
{									\
	for (int i = 0; i < m->size; i++) {				\
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
static struct map##_bucket *map##_index(struct map *m, RHMAP_KEY key)	\
{									\
	int i = key % m->size;						\
	int d = m->max_dist+1;						\
	while (d-- >= 0 && m->buckets[i].key != UNUSED) {		\
		if (m->buckets[i].key == key)				\
			return &m->buckets[i];				\
		i = (i+1) % m->size;					\
	}								\
	return NULL;							\
}									\
									\
type *map##_insert(struct map *m, RHMAP_KEY key, type val)		\
{									\
	int i = key % m->size;						\
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
			if (ins.dist > m->max_dist)			\
				m->max_dist = ins.dist+1;		\
			tmp = ins;					\
			ins = m->buckets[i];				\
			m->buckets[i] = tmp;				\
		}							\
		i = (i+1) % m->size;					\
		ins.dist++;						\
	}								\
	m->buckets[i] = ins;						\
	m->pop++;							\
	return &m->buckets[i].val;					\
}									\
									\
type *map##_remove(struct map *m, RHMAP_KEY key)			\
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
type *map##_search(struct map *m, RHMAP_KEY key)			\
{									\
	struct map##_bucket *b = map##_index(m, key);			\
	if (b != NULL)							\
		return &b->val;						\
	return NULL;							\
}

#endif
