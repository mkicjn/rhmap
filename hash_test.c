#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "rhmap.h"

DECLARE_RHMAP(map, char *)

/*
 * I was experimenting with hash functions and found this.
 * It seems weirdly good, based on my testing:
 * No collisions on 479k English words
 * No collisions on numeric strings 0-499999
 *
 * TODO: Test further. There must be a reason nobody uses this.
 * If there are issues, maybe a better seed value would help.
 */
size_t hash(const char *str, size_t n)
{
	// mine
	size_t k = 52711;
	while (n --> 0)
		k = k * k + *str++;
	return k;
	/*
	// djb2
	size_t k = 5381;
	int c;
	while ((c = *str++))
		k = ((k << 5) + k) + c;
	return k;
	*/
}

size_t hash_str(char *s)
{
	return hash(s, strlen(s));
}

void chomp(char *s)
{
	char *nl = strchr(s, '\n');
	if (nl != NULL)
		*nl = '\0';
}

size_t avg_dist(struct map *m)
{
	size_t i, dist = 0, n = 0;
	for (i = 0; i < m->capacity; i++) {
		struct map_bucket *b = &m->buckets[i];
		if (b->key == RHMAP_EMPTY || b->key == RHMAP_TOMB)
			continue;
		dist += m->buckets[i].distance;
		n++;
	}
	return dist/n;
}

static struct map_bucket map_mem[500000];

int main()
{
	// Usage: cat file | ./a.out
	struct map m;
	char buf[200];
	map_init(&m, map_mem, sizeof(map_mem));
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		char **str_ptr;
		chomp(buf);
		str_ptr = map_search(&m, hash_str(buf));
		if (str_ptr != NULL) {
			if (strcmp(buf, *str_ptr) != 0)
				printf("Collision: \"%s\" and \"%s\"\n", buf, *str_ptr);
			continue;
		}
		str_ptr = map_insert(&m, hash_str(buf), strdup(buf));
		if (str_ptr == NULL) {
			printf("Hash table full\n");
			break;
		}
	}
	printf("Max distance: %ld\n", m.max_distance);
	printf("Average distance: %ld\n", avg_dist(&m));
	map_clear(&m, (void (*)(char *))free);
	return 0;
}
