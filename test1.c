#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rhmap.h"

DECLARE_RHMAP(map, int)

unsigned long long djb2(const char *str, size_t n)
{
	unsigned long long k = 5381;
	while (n --> 0)
		k = (k << 5) + k + *str++;
	return k;
}


void map_print(struct map *m)
{
	for (int i = 0; i < m->size; i++) {
		struct map_bucket *b = &m->buckets[i];
		printf("%d: ", i);
		printf("[Key: %llu, ", b->key);
		printf("Val: %d, ", b->val);
		printf("Dist: %d]\n", b->dist);
	}
}

void test_search(struct map *m, const char *s, int i)
{
	printf("Lookup %s: ", s);
	int *res = map_search(m, djb2(s, strlen(s)));
	if (res != NULL) {
		printf("%d", *res);
		if (*res != i)
			printf(" (FAILED)");
	} else
		printf("FAILED!");
	putchar('\n');
}

int main()
{
	//struct map_bucket map_mem[32] = {0};
	char map_mem[512] = {0};
	struct map m;
	map_init(&m, map_mem, sizeof(map_mem));
#define INS(x,y) map_insert(&m, djb2(x, sizeof(x)-1), y)
	INS("Alfa", 1);
	INS("Bravo", 2);
	INS("Charlie", 3);
	INS("Delta", 4);
	INS("Echo", 5);
	INS("Foxtrot", 6);
	INS("Golf", 7);
	INS("Hotel", 8);
	INS("Indigo", 9);
	INS("Julia", 10);
	INS("Kilo", 11);
	INS("Lima", 12);
	INS("Mike", 13);
	INS("November", 14);
	INS("Oscar", 15);
	INS("Papa", 16);
	INS("Quebec", 17);
	INS("Romeo", 18);
	INS("Sierra", 19);
	INS("Tango", 20);
	INS("Uniform", 21);
	INS("Victor", 22);
	INS("Whiskey", 23);
	INS("X-ray", 24);
	INS("Yankee", 25);
	INS("Zulu", 26);

	map_print(&m);

	test_search(&m, "Alfa", 1);
	test_search(&m, "Bravo", 2);
	test_search(&m, "Charlie", 3);
	test_search(&m, "Delta", 4);
	test_search(&m, "Echo", 5);
	test_search(&m, "Foxtrot", 6);
	test_search(&m, "Golf", 7);
	test_search(&m, "Hotel", 8);
	test_search(&m, "Indigo", 9);
	test_search(&m, "Julia", 10);
	test_search(&m, "Kilo", 11);
	test_search(&m, "Lima", 12);
	test_search(&m, "Mike", 13);
	test_search(&m, "November", 14);
	test_search(&m, "Oscar", 15);
	test_search(&m, "Papa", 16);
	test_search(&m, "Quebec", 17);
	test_search(&m, "Romeo", 18);
	test_search(&m, "Sierra", 19);
	test_search(&m, "Tango", 20);
	test_search(&m, "Uniform", 21);
	test_search(&m, "Victor", 22);
	test_search(&m, "Whiskey", 23);
	test_search(&m, "X-ray", 24);
	test_search(&m, "Yankee", 25);
	test_search(&m, "Zulu", 26);

	return 0;
}
