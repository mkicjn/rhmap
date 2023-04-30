#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "rhmap.h"

RHMAP_DECLARE(map, char *)

/*
 * I was experimenting with hash functions and found this.
 * It seems weirdly good, based on my testing:
 * No collisions on 654k English words
 * No collisions on all combinations of 1 or 2-byte strings
 * No collisions on numeric strings 0-10000000 (10M)
 * Superior average search distance to djb2
 * Speed within margin of error from djb2
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

double avg_dist(struct map *m)
{
	size_t i, n = 0;
	double dist = 0.0;
	for (i = 0; i < m->capacity; i++) {
		struct map_bucket *b = &m->buckets[i];
		if (b->key == RHMAP_EMPTY || b->key == RHMAP_TOMB)
			continue;
		dist += m->buckets[i].distance;
		n++;
	}
	return dist/n;
}

double std_dist(struct map *m, double mean)
{
	size_t i, n = 0;
	double sq_dist = 0.0;
	for (i = 0; i < m->capacity; i++) {
		struct map_bucket *b = &m->buckets[i];
		double dist;
		if (b->key == RHMAP_EMPTY || b->key == RHMAP_TOMB)
			continue;
		dist = m->buckets[i].distance - mean;
		sq_dist += dist * dist;
		n++;
	}
	return sqrt(sq_dist/n);
}

double timer_us()
{
	// I'm accepting some risk of losing precision here for the sake of simplicity
	static clock_t then = 0;
	clock_t now = clock();
	double diff = (((double)(now - then)) * 1000000) / CLOCKS_PER_SEC;
	then = now;
	return diff;
}

int main(int argc, char **argv)
{
	// Usage: cat file | ./a.out
	struct map m;
	char buf[200];
	double mean_dist;
	size_t map_size = 1000000;
	struct map_bucket *map_mem;
	double hash_time = 0.0;
	double insert_time = 0.0;
	double search_time = 0.0;
	size_t num_entries = 0;
	size_t num_collisions = 0;
	if (argc > 1)
		sscanf(argv[1], "%lu", &map_size);
	map_mem = malloc(map_size * sizeof(*map_mem));
	map_init(&m, map_mem, map_size * sizeof(*map_mem));
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		char **str_ptr;
		size_t hash;
		chomp(buf);

		timer_us();
		hash = hash_str(buf);
		hash_time += timer_us();

		str_ptr = map_search(&m, hash);
		if (str_ptr != NULL) {
			if (strcmp(buf, *str_ptr) != 0) {
				printf("Collision: \"%s\" and \"%s\"\n", buf, *str_ptr);
				num_collisions++;
				free(*map_remove(&m, hash));
			}
		}

		timer_us();
		str_ptr = map_insert(&m, hash, strdup(buf));
		insert_time += timer_us();

		if (str_ptr == NULL) {
			printf("Hash table full\n");
			break;
		}

		timer_us();
		str_ptr = map_search(&m, hash);
		search_time += timer_us();

		num_entries++;
	}
	mean_dist = avg_dist(&m);
	printf("Map size: %lu\n", map_size);
	printf("Number of entries: %lu\n", num_entries);
	printf("Number of hash collisions: %lu\n", num_collisions);
	putchar('\n');
	printf("Average hash time (us): %f\n", hash_time / num_entries);
	printf("Average insertion time (us): %f\n", insert_time / num_entries);
	printf("Average search time (us): %f\n", search_time / num_entries);
	putchar('\n');
	printf("Max distance: %ld\n", m.max_distance);
	printf("Average distance: %f\n", mean_dist);
	printf("Standard deviation of distance: %f\n", std_dist(&m, mean_dist));
	map_clear(&m, (void (*)(char *))free);
	free(map_mem);
	return 0;
}
