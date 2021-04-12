#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "rhmap.h"

DECLARE_RHMAP(map, char *)

/*
 * I was experimenting with hash functions and found this.
 * It seems weirdly good, based on my testing.
 * No collisions on 479k English words.
 * No collisions on numeric strings 0-99999
 *
 * TODO: Test further. There must be reason people don't use this.
 */
size_t hash(const char *str, size_t n)
{
	size_t k = 0;
	while (n --> 0)
		k = k * k + *str++;
	return k;
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
		if (str_ptr != NULL && strcmp(buf, *str_ptr) != 0) {
			printf("Collision: \"%s\" and \"%s\"\n", buf, *str_ptr);
			continue;
		}
		str_ptr = map_insert(&m, hash_str(buf), strdup(buf));
		if (str_ptr == NULL) {
			printf("Hash table full\n");
			break;
		}
	}
	printf("Max distance: %ld\n", m.max_dist);
	map_clear(&m, (void (*)(char *))free);
	return 0;
}
